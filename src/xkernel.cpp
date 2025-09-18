/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <string>
#include <random>

#include "xeus/xkernel.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xhistory_manager.hpp"
#include "xeus/xcontrol_messenger.hpp"
#include "xkernel_core.hpp"
#include "xlogger_impl.hpp"

#if (defined(__linux__) || defined(__unix__))
#define LINUX_PLATFORM
#elif (defined(_WIN32) || defined(_WIN64))
#define WINDOWS_PLATFORM
#elif defined(__APPLE__)
#define APPLE_PLATFORM
#endif

#if (defined(LINUX_PLATFORM) || defined(APPLE_PLATFORM))
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif (defined(_WIN32) || defined(_WIN64))
#include <windows.h>
#include <Lmcons.h>
#endif

namespace xeus
{
    std::string get_user_name(){
#if defined(XEUS_EMSCRIPTEN_WASM_BUILD)
    return "unspecified user";
#elif (defined(LINUX_PLATFORM) || defined(APPLE_PLATFORM))
        struct passwd* pws;
        pws = getpwuid(geteuid());
        if (pws != nullptr)
        {
            std::string res = pws->pw_name;
            return res;
        }
        else
        {
            const char* user = std::getenv("USER");
            if (user != nullptr)
            {
                std::string res = user;
                return res;
            }
            else
            {
                return "unspecified user";
            }
        }
#elif defined(WINDOWS_PLATFORM)
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        GetUserName(username, &username_len);
        return username;
#else
        return "unspecified user";
#endif
    }

    xkernel::xkernel(const xconfiguration& config,
                     const std::string& user_name,
                     context_ptr context,
                     interpreter_ptr interpreter,
                     server_builder sbuilder,
                     history_manager_ptr history_manager,
                     logger_ptr logger,
                     debugger_builder dbuilder,
                     nl::json debugger_config,
                     nl::json::error_handler_t eh)
        : m_config(config)
        , m_user_name(user_name)
        , p_context(std::move(context))
        , p_interpreter(std::move(interpreter))
        , p_history_manager(std::move(history_manager))
        , p_logger(std::move(logger))
        , m_debugger_config(debugger_config)
        , m_error_handler(eh)
    {
        init(sbuilder, dbuilder);
    }

    xkernel::xkernel(const std::string& user_name,
                     context_ptr context,
                     interpreter_ptr interpreter,
                     server_builder sbuilder,
                     history_manager_ptr history_manager,
                     logger_ptr logger,
                     debugger_builder dbuilder,
                     nl::json debugger_config,
                     nl::json::error_handler_t eh)
        : m_user_name(user_name)
        , p_context(std::move(context))
        , p_interpreter(std::move(interpreter))
        , p_history_manager(std::move(history_manager))
        , p_logger(std::move(logger))
        , m_debugger_config(debugger_config)
        , m_error_handler(eh)
    {
        init(sbuilder, dbuilder);
    }

    xkernel::~xkernel()
    {
    }

    void xkernel::init(server_builder sbuilder, debugger_builder dbuilder)
    {
        m_kernel_id = new_xguid();
        m_session_id = new_xguid();

        if (m_config.m_key.size() == 0)
        {
            m_config.m_key = new_xguid();
        }

        if(p_logger == nullptr || std::getenv("XEUS_LOG") == nullptr)
        {
            p_logger = std::make_unique<xlogger_nolog>();
        }

        p_server = sbuilder(*p_context, m_config, m_error_handler);
        p_server->update_config(m_config);

        p_debugger = dbuilder(*p_context, m_config, m_user_name, m_session_id, m_debugger_config);

        p_core = std::make_unique<xkernel_core>(m_kernel_id,
                                                m_user_name,
                                                m_session_id,
                                                p_logger.get(),
                                                p_server.get(),
                                                p_interpreter.get(),
                                                p_history_manager.get(),
                                                p_debugger.get());

        xcontrol_messenger& messenger = p_server->get_control_messenger();

        if(p_debugger != nullptr)
        {
            p_debugger->register_control_messenger(messenger);
        }

        p_interpreter->register_control_messenger(messenger);
        p_interpreter->register_history_manager(*p_history_manager);
        p_interpreter->configure();
    }

    void xkernel::start()
    {
        xpub_message start_msg = p_core->build_start_msg();
        p_server->start(std::move(start_msg));
    }

    void xkernel::stop()
    {
        p_interpreter->shutdown_request();
        p_server->stop();
    }

    const xconfiguration& xkernel::get_config()
    {
        return m_config;
    }

    xserver& xkernel::get_server()
    {
        return *p_server;
    }
}
