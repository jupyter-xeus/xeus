/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <thread>
#include <chrono>
#include <iostream>

#include "xeus/xserver_shell_main.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xshell.hpp"

namespace xeus
{

    xserver_shell_main::xserver_shell_main(zmq::context_t& context,
                                           const xconfiguration& config,
                                           nl::json::error_handler_t eh)
        : xserver_zmq_split(context, config, eh)
    {
    }

    xserver_shell_main::~xserver_shell_main()
    {
    }

    void xserver_shell_main::start_server(zmq::multipart_t& wire_msg)
    {
        xserver_zmq_split::start_publisher_thread();
        xserver_zmq_split::start_heartbeat_thread();
        xserver_zmq_split::start_control_thread();

        xserver_zmq_split::get_shell().publish(wire_msg);
        xserver_zmq_split::get_shell().run();

        while(!xserver_zmq_split::is_control_stopped())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::exit(0);
    }

    std::unique_ptr<xserver> make_xserver_shell_main(xcontext& context,
                                                     const xconfiguration& config,
                                                     nl::json::error_handler_t eh)
    {
        return std::make_unique<xserver_shell_main>(context.get_wrapped_context<zmq::context_t>(), config, eh);
    }
}
