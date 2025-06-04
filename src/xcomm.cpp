/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"

#include "xeus/xcomm.hpp"

#include "xkernel_core.hpp"

namespace xeus
{
    /**************************
     * xtarget implementation *
     **************************/

    xtarget::xtarget(const std::string& name,
                     const function_type& callback,
                     xcomm_manager* manager)
        : m_name(name)
        , m_callback(callback)
        , p_manager(manager)
    {
    }

    const std::string& xtarget::name() const noexcept
    {
        return m_name;
    }

    void xtarget::operator()(xcomm&& comm, xmessage message) const
    {
        return m_callback(std::move(comm), std::move(message));
    }

    void xtarget::register_comm(xguid id, xcomm* comm) const
    {
        p_manager->register_comm(id, comm);
    }

    void xtarget::unregister_comm(xguid id) const
    {
        p_manager->unregister_comm(id);
    }

    void xtarget::publish_message(const std::string& msg_type,
                                  nl::json metadata,
                                  nl::json content,
                                  buffer_sequence buffers) const
    {
        if (p_manager->p_kernel != nullptr)
        {
            p_manager->p_kernel->publish_message(
                msg_type, p_manager->p_kernel->parent_header(),
                std::move(metadata), std::move(content), 
                std::move(buffers), channel::SHELL
            );
        }
    }

    /************************
     * xcomm implementation *
     ************************/

    const xtarget& xcomm::target() const noexcept
    {
        return *p_target;
    }

    void xcomm::handle_close(xmessage message)
    {
        if (m_close_handler)
        {
            m_close_handler(std::move(message));
        }
    }

    void xcomm::handle_message(xmessage message)
    {
        if (m_message_handler)
        {
            m_message_handler(std::move(message));
        }
    }

    void xcomm::send_comm_message(const std::string& msg_type,
                                         nl::json metadata,
                                         nl::json data,
                                         buffer_sequence buffers) const
    {
        nl::json content;
        content["comm_id"] = m_id;
        content["data"] = std::move(data);
        target().publish_message(msg_type, std::move(metadata), std::move(content), std::move(buffers));
    }

    void xcomm::send_comm_message(const std::string& msg_type,
                                         nl::json metadata,
                                         nl::json data,
                                         buffer_sequence buffers,
                                         const std::string& target_name) const
    {
        nl::json content;
        content["comm_id"] = m_id;
        content["target_name"] = target_name;
        content["data"] = std::move(data);
        target().publish_message(msg_type, std::move(metadata), std::move(content), std::move(buffers));
    }

    xcomm::xcomm(xcomm&& comm)
        : m_close_handler(std::move(comm.m_close_handler))
        , m_message_handler(std::move(comm.m_message_handler))
        , p_target(std::move(comm.p_target))
        , m_id(std::move(comm.m_id))
        , m_moved_from(false)
    {
        comm.m_moved_from = true;
        p_target->register_comm(m_id,
                                this);  // Replacing the address of the moved comm with `this`.
    }

    xcomm::xcomm(const xcomm& comm)
        : p_target(comm.p_target)
        , m_id(xeus::new_xguid())
        , m_moved_from(false)
    {
        p_target->register_comm(m_id, this);
    }

    xcomm& xcomm::operator=(xcomm&& comm)
    {
        m_close_handler = std::move(comm.m_close_handler);
        m_message_handler = std::move(comm.m_message_handler);
        p_target = std::move(comm.p_target);
        p_target->unregister_comm(m_id);
        m_id = std::move(comm.m_id);
        m_moved_from = false;
        comm.m_moved_from = true;
        p_target->register_comm(m_id,
                                this);  // Replacing the address of the moved comm with `this`.
        return *this;
    }

    xcomm& xcomm::operator=(const xcomm& comm)
    {
        p_target = comm.p_target;
        p_target->unregister_comm(m_id);
        m_id = new_xguid();
        m_moved_from = false;
        p_target->register_comm(m_id, this);
        return *this;
    }

    xcomm::xcomm(const xtarget* target, xguid id)
        : p_target(target)
        , m_id(id)
    {
        if (!p_target)
            throw std::runtime_error("Cannot initialize comm with null target");
        p_target->register_comm(m_id, this);
    }

    xcomm::~xcomm()
    {
        if (!m_moved_from)
        {
            p_target->unregister_comm(m_id);
        }
    }

    void xcomm::open(nl::json metadata, nl::json data, buffer_sequence buffers)
    {
        send_comm_message("comm_open",
                          std::move(metadata),
                          std::move(data),
                          std::move(buffers),
                          p_target->name());
    }

    void xcomm::close(nl::json metadata, nl::json data, buffer_sequence buffers)
    {
        send_comm_message("comm_close", std::move(metadata), std::move(data), std::move(buffers));
    }

    void xcomm::send(nl::json metadata, nl::json data, buffer_sequence buffers) const
    {
        send_comm_message("comm_msg", std::move(metadata), std::move(data), std::move(buffers));
    }

    xguid xcomm::id() const noexcept
    {
        return m_id;
    }

    /********************************
     * xcomm_manager implementation *
     ********************************/

    xcomm_manager::xcomm_manager(xkernel_core* kernel)
    {
        p_kernel = kernel;
    }

    nl::json xcomm_manager::get_metadata() const
    {
        // TODO: handle duplication
        nl::json metadata;
        metadata["started"] = iso8601_now();
        return metadata;
    }

    void xcomm_manager::register_comm_target(const std::string& target_name,
                                             const target_function_type& callback)
    {
        m_targets.insert_or_assign(target_name, xtarget(target_name, callback, this));
    }

    void xcomm_manager::unregister_comm_target(const std::string& target_name)
    {
        m_targets.erase(target_name);
    }

    void xcomm_manager::register_comm(xguid id, xcomm* comm)
    {
        m_comms[id] = comm;
    }

    void xcomm_manager::unregister_comm(xguid id)
    {
        m_comms.erase(id);
    }

    void xcomm_manager::comm_open(xmessage request)
    {
        const nl::json& content = request.content();
        std::string target_name = content["target_name"];
        auto position = m_targets.find(target_name);

        if (position == m_targets.end())
        {
            // Directly close the comm, as specified in the protocol
            if (p_kernel != nullptr)
            {
                p_kernel->publish_message(
                    "comm_close", request.header(), nl::json::object(), content, buffer_sequence(), channel::SHELL
                );
            }
        }
        else
        {
            xtarget& target = position->second;
            xguid id = content["comm_id"];
            xcomm comm = xcomm(&target, id);
            target(std::move(comm), std::move(request));
        }
    }

    void xcomm_manager::comm_close(xmessage request)
    {
        const nl::json& content = request.content();
        xguid id = content["comm_id"];
        auto position = m_comms.find(id);
        if (position == m_comms.end())
        {
            throw std::runtime_error("No such comm registered: " + std::string(id));
        }
        else
        {
            position->second->handle_close(std::move(request));
        }
        m_comms.erase(id);
    }

    void xcomm_manager::comm_msg(xmessage request)
    {
        const nl::json& content = request.content();
        xguid id = content["comm_id"];
        auto position = m_comms.find(id);
        if (position == m_comms.end())
        {
            throw std::runtime_error("No such comm registered: " + std::string(id));
        }
        else
        {
            position->second->handle_message(std::move(request));
        }
    }

    const xtarget* xcomm_manager::target(const std::string& target_name) const
    {
        auto iter = m_targets.find(target_name);
        return iter == m_targets.end() ? nullptr : &(iter->second);
    }

    const std::map<xguid, xcomm*>& xcomm_manager::comms() const noexcept
    {
        return m_comms;
    }
}
