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

        using history_manager_ptr = std::unique_ptr<xhistory_manager>;
        using interpreter_ptr = std::unique_ptr<xinterpreter>;
        using kernel_core_ptr = std::unique_ptr<xkernel_core>;
        using logger_ptr = std::unique_ptr<xlogger>;
        using server_ptr = std::unique_ptr<xserver>;
        using debugger_ptr = std::unique_ptr<xdebugger>;
        using server_builder = server_ptr (*)(zmq::context_t& context,
                                              const xconfiguration& config);
        using debugger_builder = debugger_ptr (*)(zmq::context_t& context,
                                                  const xconfiguration& config,
                                                  const std::string&,
                                                  const std::string&);

        xkernel(const xconfiguration& config,
                const std::string& user_name,
                interpreter_ptr interpreter,
                history_manager_ptr history_manager = make_in_memory_history_manager(),
                logger_ptr logger = nullptr,
                server_builder sbuilder = make_xserver,
                debugger_builder dbuilder = make_null_debugger);

        xkernel(const std::string& user_name,
                interpreter_ptr interpreter,
                history_manager_ptr history_manager = make_in_memory_history_manager(),
                logger_ptr logger = nullptr,
                server_builder sbuilder = make_xserver,
                debugger_builder dbuilder = make_null_debugger);

        ~xkernel();

        void start();

        const xconfiguration& get_config();
        xserver& get_server();

    private:

        void init();

        xconfiguration m_config;
        std::string m_kernel_id;
        std::string m_session_id;
        std::string m_user_name;
        interpreter_ptr p_interpreter;
        history_manager_ptr p_history_manager;
        logger_ptr p_logger;
        server_builder m_server_builder;
        server_ptr p_server;
        debugger_builder m_debugger_builder;
        debugger_ptr p_debugger;
        zmq::context_t m_context;
        kernel_core_ptr p_core;
    };
}

#endif
