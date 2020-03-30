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

#include "xeus/xserver_zmq_split.hpp"
#include "xcontrol.hpp"
#include "xserver_utils.hpp"

namespace xeus
{
    xcontrol::xcontrol(zmq::context_t& context,
                       const std::string& transport,
                       const std::string& ip,
                       const std::string& control_port,
                       xserver_zmq_split* server)
        : m_control(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_messenger(context)
        , p_server(server)
        , m_request_stop(false)
    {
        bind_socket("xcontrol", "control", m_control, transport, ip, control_port);
        connect_socket("xcontrol", "publisher_pub", m_publisher_pub, get_publisher_end_point());
    }
    
    xcontrol::~xcontrol()
    {
    }

    std::string xcontrol::get_port() const
    {
        return get_socket_port(m_control);
    }

    void xcontrol::connect_messenger()
    {
        m_messenger.connect();
    }

    xcontrol_messenger& xcontrol::get_messenger()
    {
        return m_messenger;
    }

    void xcontrol::run()
    {
        console_log("xcontrol started");
        m_request_stop = false;

        while (!m_request_stop)
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_control);
            p_server->notify_control_listener(wire_msg);
        }

        console_log("xcontrol stopped");
        m_messenger.stop_channels();
        p_server->notify_control_stopped();
    }

    void xcontrol::stop()
    {
        m_request_stop = true;
    }

    void xcontrol::send_control(zmq::multipart_t& message)
    {
        message.send(m_control);
    }

    void xcontrol::publish(zmq::multipart_t& message)
    {
        message.send(m_publisher_pub);
    }
}

