/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONTROL_MESSENGER_HPP
#define XEUS_CONTROL_MESSENGER_HPP

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus/xeus.hpp"

namespace xeus
{
    class XEUS_API xcontrol_messenger
    {
    public:

        explicit xcontrol_messenger(zmq::context_t& context);

        void stop_channels();

    private:

        zmq::socket_t m_shell_controller;
        zmq::socket_t m_publisher_controller;
        zmq::socket_t m_heartbeat_controller;
    };
}

#endif

