/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <thread>
#include <chrono>
#include <iostream>

#include "xeus/xserver_zmq_split.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"
#include "xshell.hpp"

namespace xeus
{
    xserver_zmq_split::xserver_zmq_split(zmq::context_t& context, const xconfiguration& config)
        : m_controller(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_shell_controller(context, zmq::socket_type::req)
        , m_publisher_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
        , p_shell(new xshell(context, config.m_transport, config.m_ip ,config.m_shell_port, config.m_stdin_port, this))
        , p_publisher(new xpublisher(context, config.m_transport, config.m_ip, config.m_iopub_port))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , m_request_stop(false)
    {
        init_socket(m_controller, config.m_transport, config.m_ip, config.m_control_port);
        m_publisher_pub.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());

        m_shell_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_shell_controller.connect(get_controller_end_point("shell"));
        m_publisher_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_controller.connect(get_controller_end_point("publisher"));
        m_heartbeat_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_heartbeat_controller.connect(get_controller_end_point("heartbeat"));
    }

    xserver_zmq_split::~xserver_zmq_split()
    {
    }

    void xserver_zmq_split::send_shell_impl(zmq::multipart_t& message)
    {
        // Shell thread only
        p_shell->send_shell(message);
    }

    void xserver_zmq_split::send_control_impl(zmq::multipart_t& message)
    {
        // Control (or main) thread only
        message.send(m_controller);
    }

    void xserver_zmq_split::send_stdin_impl(zmq::multipart_t& message)
    {
        // Shell thread only
        p_shell->send_stdin(message);
    }
    
    void xserver_zmq_split::publish_impl(zmq::multipart_t& message, channel c)
    {
        if(c == channel::SHELL)
        {
            p_shell->publish(message);
        }
        else
        {
            message.send(m_publisher_pub);
        }
    }
    
    zmq::multipart_t xserver_zmq_split::send_internal_request_impl(zmq::multipart_t& message)
    {
        message.send(m_shell_controller);
        zmq::multipart_t reply;
        reply.recv(m_shell_controller);
        return reply;
    }

    void xserver_zmq_split::send_internal_reply_impl(zmq::multipart_t& message)
    {
        p_shell->send_internal(message);
    }

    void xserver_zmq_split::start_impl(zmq::multipart_t& message)
    {
        start_publisher_thread();
        start_heartbeat_thread();
        start_shell_thread();

        m_request_stop = false;

        publish(message, channel::CONTROL);

        while (!m_request_stop)
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_controller);
            xserver::notify_control_listener(wire_msg);
        }

        stop_channels();

        std::exit(0);
    }

    void xserver_zmq_split::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_shell->abort_queue(l, polling_interval);
    }

    void xserver_zmq_split::stop_impl()
    {
        m_request_stop = true;
    }

    void xserver_zmq_split::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = get_socket_port(m_controller);
        config.m_shell_port = p_shell->get_shell_port();
        config.m_stdin_port = p_shell->get_stdin_port();
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_zmq_split::start_shell_thread()
    {
        std::thread shell_thread(&xshell::run, p_shell.get());
        shell_thread.detach();
    }

    void xserver_zmq_split::start_publisher_thread()
    {
        std::thread iopub_thread(&xpublisher::run, p_publisher.get());
        iopub_thread.detach();
    }

    void xserver_zmq_split::start_heartbeat_thread()
    {
        std::thread hb_thread(&xheartbeat::run, p_heartbeat.get());
        hb_thread.detach();
    }

    void xserver_zmq_split::stop_channels()
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

