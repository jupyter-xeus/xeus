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

        xkernel_core kernel_core(kernel_id, m_user_name, session_id,
                                 std::move(auth), server.get(), p_interpreter.get());

        server->start();
    }

}
