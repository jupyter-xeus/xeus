/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xkernel.hpp"
#include "xguid.hpp"
#include "xkernel_core.hpp"

namespace xeus
{
    void build_start_msg(xkernel_core::authentication_ptr& auth,
                         const std::string& kernel_id,
                         const std::string& user_name,
                         const std::string& session_id,
                         zmq::multipart_t& wire_msg)
    {
        std::string topic = "kernel_core." + kernel_id + ".status";
        xjson content;
        content.set_value("/execution_state", "starting");

        xpub_message msg(topic,
                         make_header("status", user_name, session_id),
                         xjson(),
                         xjson(),
                         std::move(content));
        msg.serialize(wire_msg, *auth);
    }

    xkernel::xkernel(const xconfiguration& config,
                     const std::string& user_name,
                     interpreter_ptr interpreter,
                     server_builder builder)
        : m_config(config),
          m_user_name(user_name),
          p_interpreter(std::move(interpreter)),
          m_builder(builder)
    {
    }

    void xkernel::start()
    {
        zmq::context_t context;
        server_ptr server = m_builder(context, m_config);

        std::string kernel_id = new_xguid().to_string();
        std::string session_id = new_xguid().to_string();

        using authentication_ptr = xkernel_core::authentication_ptr;
        authentication_ptr auth = make_xauthentication(m_config.m_signature_scheme, m_config.m_key);

        zmq::multipart_t start_msg;
        build_start_msg(auth, kernel_id, m_user_name, session_id, start_msg);

        xkernel_core core(kernel_id, m_user_name, session_id,
                          std::move(auth), server.get(), p_interpreter.get());

        server->start(start_msg);
    }

}
