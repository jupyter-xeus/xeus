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

#include "xeus/xserver_shell_main.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"
#include "xcontrol.hpp"

namespace xeus
{

    xserver_shell_main::xserver_shell_main(zmq::context_t& context, const xconfiguration& config)
        : m_shell(context, zmq::socket_type::router)
        , m_stdin(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_controller(context, zmq::socket_type::rep)
        , p_controller(new xcontrol(context, config.m_transport, config.m_ip ,config.m_control_port, this))
        , p_publisher(new xpublisher(context, config.m_transport, config.m_ip, config.m_iopub_port))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , m_control_stopped(false)
    {
        init_socket(m_shell, config.m_transport, config.m_ip, config.m_shell_port);
        init_socket(m_stdin, config.m_transport, config.m_ip, config.m_stdin_port);
        m_publisher_pub.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());

        m_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_controller.bind(get_controller_end_point("shell"));

        p_controller->connect_messenger();
    }

    xserver_shell_main::~xserver_shell_main()
    {
    }


    void xserver_shell_main::notify_control_stopped()
    {
        m_control_stopped = true;
    }
    
    void xserver_shell_main::send_shell_impl(zmq::multipart_t& message)
    {
        message.send(m_shell);
    }

    void xserver_shell_main::send_control_impl(zmq::multipart_t& message)
    {
        p_controller->send_control(message);
    }

    void xserver_shell_main::send_stdin_impl(zmq::multipart_t& message)
    {
        message.send(m_stdin);
        zmq::multipart_t wire_msg;
        wire_msg.recv(m_stdin);
        xserver::notify_stdin_listener(wire_msg);
    }

    void xserver_shell_main::publish_impl(zmq::multipart_t& message, channel c)
    {
        if(c == channel::SHELL)
        {
            message.send(m_publisher_pub);
        }
        else
        {
            p_controller->publish(message);
        }
    }

    void xserver_shell_main::start_impl(zmq::multipart_t& message)
    {
        start_publisher_thread();
        start_heartbeat_thread();
        start_control_thread();

        publish(message, channel::SHELL);

        zmq::pollitem_t items[] = {
            { m_shell, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 }
        };

        while (true)
        {
            zmq::poll(&items[0], 2, -1);

            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_shell);
                xserver::notify_shell_listener(wire_msg);
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                // stop message
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_controller);
                wire_msg.send(m_controller);
                break;
            }
        }

        while(!m_control_stopped)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::exit(0);
    }

    void xserver_shell_main::abort_queue_impl(const listener& l, long polling_interval)
    {
        while (true)
        {
            zmq::multipart_t wire_msg;
            bool msg = wire_msg.recv(m_shell, ZMQ_NOBLOCK);
            if (!msg)
            {
                return;
            }

            l(wire_msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(polling_interval));
        }
    }

    void xserver_shell_main::stop_impl()
    {
        p_controller->stop();
    }
        
    void xserver_shell_main::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = p_controller->get_port();
        config.m_shell_port = get_socket_port(m_shell);
        config.m_stdin_port = get_socket_port(m_stdin);
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_shell_main::start_control_thread()
    {
        std::thread control_thread(&xcontrol::run, p_controller.get());
        control_thread.detach();
    }

    void xserver_shell_main::start_publisher_thread()
    {
        std::thread iopub_thread(&xpublisher::run, p_publisher.get());
        iopub_thread.detach();
    }

    void xserver_shell_main::start_heartbeat_thread()
    {
        std::thread hb_thread(&xheartbeat::run, p_heartbeat.get());
        hb_thread.detach();
    }
}
