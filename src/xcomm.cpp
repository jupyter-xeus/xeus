#include "xcomm.hpp"
#include "xkernel_core.hpp"

namespace xeus
{
    void xtarget::publish_message(const std::string& msg_type, xjson metadata, xjson content)
    {
        p_manager->kernel().publish_message(msg_type, std::move(metadata), std::move(content));
    }

    xcomm_manager::xcomm_manager(xkernel_core* kernel)
    {
        p_kernel = kernel;
    }

    xjson xcomm_manager::get_metadata() const
    {
        // TODO: handle duplication
        xjson metadata;
        metadata["started"] = iso8601_now();
        return metadata;
    }

    void xcomm_manager::register_comm_target(const std::string& target_name, const target_function_type& callback)
    {
        m_targets[target_name] = xtarget(target_name, callback, this);
    }

    void xcomm_manager::unregister_comm_target(const std::string& target_name)
    {
        m_targets.erase(target_name);
    }

    void xcomm_manager::comm_open(const xmessage& request)
    {
        const xjson& content = request.content();
        std::string target_name = content["target_name"];
        auto position = m_targets.find(target_name);

        if (position == m_targets.end())
        {
            throw std::runtime_error("No such comm target registered: " + target_name);
        }
        else
        {
            xtarget& target = position->second;
            std::string sid = content["comm_id"];
            xguid id = hex_to_guid(sid.c_str());
            xcomm comm = xcomm(&target, id);
            auto p = m_comms.emplace(id, std::move(comm));
            target(p.first->second, request);
            // Open comm
            comm.open(get_metadata(), content["data"]);
        }
    }

    void xcomm_manager::comm_close(const xmessage& request)
    {
        const xjson& content = request.content();
        std::string sid = content["comm_id"];
        xguid id = hex_to_guid(sid.c_str());
        m_comms.erase(id);
    }

    void xcomm_manager::comm_msg(const xmessage& request)
    {
        const xjson& content = request.content();
        std::string sid = content["comm_id"];
        xguid id = hex_to_guid(sid.c_str());
        auto position = m_comms.find(id);
        if (position == m_comms.end())
        {
            throw std::runtime_error("No such comm registered: " + guid_to_hex(id));
        }
        else
        {
            position->second.handle_message(request);
        }
    }

    xkernel_core& xcomm_manager::kernel() noexcept
    {
        return *p_kernel;
    }

    const xkernel_core& xcomm_manager::kernel() const noexcept
    {
        return *p_kernel;
    }
}
