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
                msg_type, std::move(metadata), std::move(content), std::move(buffers), channel::SHELL);
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

    void xcomm_manager::comm_open(const xmessage& request)
    {
        const nl::json& content = request.content();
        std::string target_name = content["target_name"];
        auto position = m_targets.find(target_name);

        if (position == m_targets.end())
        {
            throw std::runtime_error("No such comm target registered: " + target_name);
        }
        else
        {
            xtarget& target = position->second;
            xguid id = content["comm_id"];
            xcomm comm = xcomm(&target, id);
            target(std::move(comm), request);
        }
    }

    void xcomm_manager::comm_close(const xmessage& request)
    {
        const nl::json& content = request.content();
        xguid id = content["comm_id"];
        auto position = m_comms.find(id);
        if (position == m_comms.end())
        {
            throw std::runtime_error("No such comm registered: " + id);
        }
        else
        {
            position->second->handle_close(request);
        }
        m_comms.erase(id);
    }

    void xcomm_manager::comm_msg(const xmessage& request)
    {
        const nl::json& content = request.content();
        xguid id = content["comm_id"];
        auto position = m_comms.find(id);
        if (position == m_comms.end())
        {
            throw std::runtime_error("No such comm registered: " + id);
        }
        else
        {
            position->second->handle_message(request);
        }
    }
}
