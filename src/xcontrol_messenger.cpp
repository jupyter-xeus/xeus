/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus/xcontrol_messenger.hpp"
#include "xeus/xmiddleware.hpp"

namespace xeus
{
    xcontrol_messenger::xcontrol_messenger(zmq::context_t& context)
        : m_shell_controller(context, zmq::socket_type::req)
        , m_publisher_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
    {
        m_shell_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_shell_controller.connect(get_controller_end_point("shell"));
        m_publisher_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_controller.connect(get_controller_end_point("publisher"));
        m_heartbeat_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_heartbeat_controller.connect(get_controller_end_point("heartbeat"));
    }

    void xcontrol_messenger::stop_channels()
    {
        zmq::message_t stop_msg("stop", 4);
        zmq::message_t response;

        // Wait for shell answer
        m_shell_controller.send(stop_msg);
        m_shell_controller.recv(&response);

        // Wait for publisher answer
        m_publisher_controller.send(stop_msg);
        m_publisher_controller.recv(&response);

        // Wait for heartbeat answer
        m_heartbeat_controller.send(stop_msg);
        m_heartbeat_controller.recv(&response);
    }
}

