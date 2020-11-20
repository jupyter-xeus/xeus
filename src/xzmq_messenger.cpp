/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"

#include "xeus/xmiddleware.hpp"

#include "xzmq_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xzmq_messenger::xzmq_messenger(zmq::context_t& context)
        : m_shell_controller(context, zmq::socket_type::req)
        , m_publisher_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
    {
    }

    xzmq_messenger::~xzmq_messenger()
    {
    }

    void xzmq_messenger::connect()
    {
        m_shell_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_shell_controller.connect(get_controller_end_point("shell"));
        m_publisher_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_publisher_controller.connect(get_controller_end_point("publisher"));
        m_heartbeat_controller.set(zmq::sockopt::linger, get_socket_linger());
        m_heartbeat_controller.connect(get_controller_end_point("heartbeat"));
    }

    void xzmq_messenger::stop_channels()
    {
        zmq::message_t stop_msg("stop", 4);
        zmq::message_t response;

        // Wait for shell answer
        m_shell_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_shell_controller.recv(response);

        // Wait for publisher answer
        m_publisher_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_publisher_controller.recv(response);

        // Wait for heartbeat answer
        m_heartbeat_controller.send(stop_msg, zmq::send_flags::none);
        (void)m_heartbeat_controller.recv(response);
    }

    nl::json xzmq_messenger::send_to_shell_impl(const nl::json& message)
    {
        zmq::multipart_t wire_msg(message.dump());
        wire_msg.send(m_shell_controller);
        zmq::multipart_t wire_reply;
        wire_reply.recv(m_shell_controller);
        return nl::json::parse(wire_reply.popstr());
    }
}

