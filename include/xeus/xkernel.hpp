/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_KERNEL_HPP
#define XEUS_KERNEL_HPP

#include "xdebugger.hpp"
#include "xeus.hpp"
#include "xeus_context.hpp"
#include "xhistory_manager.hpp"
#include "xinterpreter.hpp"
#include "xkernel_configuration.hpp"
#include "xserver.hpp"
#include "xlogger.hpp"
#include <memory>
#include <string>

namespace xeus
{
    class xkernel_core;

    XEUS_API
    std::string get_user_name();

    class XEUS_API xkernel
    {
    public:

        using context_ptr = std::unique_ptr<xcontext>;
        using history_manager_ptr = std::unique_ptr<xhistory_manager>;
        using interpreter_ptr = std::unique_ptr<xinterpreter>;
        using kernel_core_ptr = std::unique_ptr<xkernel_core>;
        using logger_ptr = std::unique_ptr<xlogger>;
        using server_ptr = std::unique_ptr<xserver>;
        using debugger_ptr = std::unique_ptr<xdebugger>;
        using server_builder = server_ptr (*)(xcontext& context,
                                              const xconfiguration& config,
                                              nl::json::error_handler_t eh);
        using debugger_builder = debugger_ptr (*)(xcontext& context,
                                                  const xconfiguration& config,
                                                  const std::string&,
                                                  const std::string&,
                                                  const nl::json&);

        xkernel(const xconfiguration& config,
                const std::string& user_name,
                context_ptr context,
                interpreter_ptr interpreter,
                server_builder sbuilder,
                history_manager_ptr history_manager = make_in_memory_history_manager(),
                logger_ptr logger = nullptr,
                debugger_builder dbuilder = make_null_debugger,
                nl::json debugger_config = nl::json::object(),
                nl::json::error_handler_t eh = nl::json::error_handler_t::strict);

        xkernel(const std::string& user_name,
                context_ptr context,
                interpreter_ptr interpreter,
                server_builder sbuilder,
                history_manager_ptr history_manager = make_in_memory_history_manager(),
                logger_ptr logger = nullptr,
                debugger_builder dbuilder = make_null_debugger,
                nl::json debugger_config = nl::json::object(),
                nl::json::error_handler_t eh = nl::json::error_handler_t::strict);

        ~xkernel();

        void start();
        void stop();

        const xconfiguration& get_config();
        xserver& get_server();

    private:

        void init(server_builder sbuilder,
                  debugger_builder dbuilder);

        xconfiguration m_config;
        std::string m_kernel_id;
        std::string m_session_id;
        std::string m_user_name;
        // The context must be declared before any other
        // middleware component since it must be destroyed
        // last
        context_ptr p_context;
        interpreter_ptr p_interpreter;
        history_manager_ptr p_history_manager;
        logger_ptr p_logger;
        server_ptr p_server;
        debugger_ptr p_debugger;
        kernel_core_ptr p_core;
        nl::json m_debugger_config;
        nl::json::error_handler_t m_error_handler;
    };
}

#endif
