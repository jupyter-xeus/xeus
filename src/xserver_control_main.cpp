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

#include "xeus/xserver_control_main.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"
#include "xshell.hpp"

namespace xeus
{
    xserver_control_main::xserver_control_main(zmq::context_t& context, const xconfiguration& config)
        : m_controller(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_messenger(context)
        , p_shell(new xshell(context, config.m_transport, config.m_ip ,config.m_shell_port, config.m_stdin_port, this))
        , p_publisher(new xpublisher(context, config.m_transport, config.m_ip, config.m_iopub_port))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , m_request_stop(false)
    {
        init_socket(m_controller, config.m_transport, config.m_ip, config.m_control_port);
        m_publisher_pub.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());
    }

    xserver_control_main::~xserver_control_main()
    {
    }

    void xserver_control_main::send_shell_impl(zmq::multipart_t& message)
    {
        // Shell thread only
        p_shell->send_shell(message);
    }

    void xserver_control_main::send_control_impl(zmq::multipart_t& message)
    {
        // Control (or main) thread only
        message.send(m_controller);
    }

    void xserver_control_main::send_stdin_impl(zmq::multipart_t& message)
    {
        // Shell thread only
        p_shell->send_stdin(message);
    }
    
    void xserver_control_main::publish_impl(zmq::multipart_t& message, channel c)
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
    
    void xserver_control_main::start_impl(zmq::multipart_t& message)
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

    void xserver_control_main::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_shell->abort_queue(l, polling_interval);
    }

    void xserver_control_main::stop_impl()
    {
        m_request_stop = true;
    }

    void xserver_control_main::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = get_socket_port(m_controller);
        config.m_shell_port = p_shell->get_shell_port();
        config.m_stdin_port = p_shell->get_stdin_port();
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_control_main::start_shell_thread()
    {
        std::thread shell_thread(&xshell::run, p_shell.get());
        shell_thread.detach();
    }

    void xserver_control_main::start_publisher_thread()
    {
        std::thread iopub_thread(&xpublisher::run, p_publisher.get());
        iopub_thread.detach();
    }

    void xserver_control_main::start_heartbeat_thread()
    {
        std::thread hb_thread(&xheartbeat::run, p_heartbeat.get());
        hb_thread.detach();
    }

    void xserver_control_main::stop_channels()
    {
        m_messenger.stop_channels();
    }
}

