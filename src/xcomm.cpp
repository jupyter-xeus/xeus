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
        m_targets[target_name] = xtarget(target_name, callback, this);
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
                    "comm_close", request.header(), nl::json::object(), std::move(content), buffer_sequence(), channel::SHELL
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
}
