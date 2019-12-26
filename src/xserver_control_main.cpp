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

#include "xeus/xserver_control_main.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xcontrol.hpp"

namespace xeus
{
    xserver_control_main::xserver_control_main(zmq::context_t& context, const xconfiguration& config)
        : xserver_zmq_split(context, config)
    {
    }

    xserver_control_main::~xserver_control_main()
    {
    }

    void xserver_control_main::start_impl(zmq::multipart_t& message)
    {
        xserver_zmq_split::start_publisher_thread();
        xserver_zmq_split::start_heartbeat_thread();
        xserver_zmq_split::start_shell_thread();

        xserver_zmq_split::get_controller().publish(message);
        xserver_zmq_split::get_controller().run();

        std::exit(0);
    }
}

