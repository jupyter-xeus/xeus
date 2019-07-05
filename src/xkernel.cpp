/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <random>

#include "xeus/xkernel.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xhistory_manager.hpp"
#include "xkernel_core.hpp"

#if (defined(__linux__) || defined(__unix__))
#define LINUX_PLATFORM
#elif (defined(_WIN32) || defined(_WIN64))
#define WINDOWS_PLATFORM
#endif

#if defined(LINUX_PLATFORM)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace xeus
{
    std::string get_user_name()
    {
#if defined(LINUX_PLATFORM)
        struct passwd* pws;
        pws = getpwuid(geteuid());
        std::string res = pws->pw_name;
        return res;
#else
        return "unspecified user";
#endif
    }

    xkernel::xkernel(const xconfiguration& config,
                     const std::string& user_name,
                     interpreter_ptr interpreter,
                     history_manager_ptr history_manager,
                     server_builder sbuilder,
                     debugger_builder dbuilder)
        : m_config(config)
        , m_user_name(user_name)
        , p_interpreter(std::move(interpreter))
        , p_history_manager(std::move(history_manager))
        , m_server_builder(sbuilder)
        , m_debugger_builder(dbuilder)
    {
        init();
    }

    xkernel::xkernel(const std::string& user_name,
                     interpreter_ptr interpreter,
                     history_manager_ptr history_manager,
                     server_builder sbuilder,
                     debugger_builder dbuilder)
        : m_user_name(user_name)
        , p_interpreter(std::move(interpreter))
        , p_history_manager(std::move(history_manager))
        , m_server_builder(sbuilder)
        , m_debugger_builder(dbuilder)
    {
        init();
    }

    xkernel::~xkernel()
    {
    }

    void xkernel::init()
    {
        m_kernel_id = new_xguid();
        m_session_id = new_xguid();

        if (m_config.m_key.size() == 0)
        {
            m_config.m_key = new_xguid();
        }

        using authentication_ptr = xkernel_core::authentication_ptr;
        authentication_ptr auth = make_xauthentication(m_config.m_signature_scheme, m_config.m_key);

        p_server = m_server_builder(m_context, m_config);
        p_server->update_config(m_config);

        p_debugger = m_debugger_builder(m_context, m_config, m_user_name, m_session_id);

        p_core = kernel_core_ptr(new xkernel_core(m_kernel_id,
                                                  m_user_name,
                                                  m_session_id,
                                                  std::move(auth),
                                                  p_server.get(),
                                                  p_interpreter.get(),
                                                  p_history_manager.get(),
                                                  p_debugger.get()));

        p_interpreter->configure();
    }

    void xkernel::start()
    {
        zmq::multipart_t start_msg = p_core->build_start_msg();
        p_server->start(start_msg);
    }

    const xconfiguration& xkernel::get_config()
    {
        return m_config;
    }
}
