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
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xcontrol.hpp"
#include "xheartbeat.hpp"
#include "xpublisher.hpp"
#include "xshell.hpp"
#include "xzmq_messenger.hpp"

namespace xeus
{
    
    xserver_zmq_split::xserver_zmq_split(zmq::context_t& context, const xconfiguration& config)
        : p_controller(new xcontrol(context, config.m_transport, config.m_ip ,config.m_control_port, this))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , p_publisher(new xpublisher(context, config.m_transport, config.m_ip, config.m_iopub_port))
        , p_shell(new xshell(context, config.m_transport, config.m_ip ,config.m_shell_port, config.m_stdin_port, this))
        , m_control_stopped(false)
    {
        p_controller->connect_messenger();
    }

    xserver_zmq_split::~xserver_zmq_split()
    {
    }
    
    void xserver_zmq_split::notify_control_stopped()
    {
        m_control_stopped = true;
    }

    xcontrol_messenger& xserver_zmq_split::get_control_messenger_impl()
    {
        return p_controller->get_messenger();
    }

    void xserver_zmq_split::send_shell_impl(zmq::multipart_t& message)
    {
        p_shell->send_shell(message);
    }

    void xserver_zmq_split::send_control_impl(zmq::multipart_t& message)
    {
        p_controller->send_control(message);
    }

    void xserver_zmq_split::send_stdin_impl(zmq::multipart_t& message)
    {
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
            p_controller->publish(message);
        }
    }

    void xserver_zmq_split::abort_queue_impl(const listener& l, long polling_interval)
    {
        p_shell->abort_queue(l, polling_interval);
    }
    
    void xserver_zmq_split::stop_impl()
    {
        p_controller->stop();
    }
    
    void xserver_zmq_split::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = p_controller->get_port();
        config.m_shell_port = p_shell->get_shell_port();
        config.m_stdin_port = p_shell->get_stdin_port();
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_zmq_split::start_control_thread()
    {
        std::thread control_thread(&xcontrol::run, p_controller.get());
        control_thread.detach();
    }

    void xserver_zmq_split::start_heartbeat_thread()
    {
        std::thread hb_thread(&xheartbeat::run, p_heartbeat.get());
        hb_thread.detach();
    }

    void xserver_zmq_split::start_publisher_thread()
    {
        std::thread iopub_thread(&xpublisher::run, p_publisher.get());
        iopub_thread.detach();
    }

    void xserver_zmq_split::start_shell_thread()
    {
        std::thread shell_thread(&xshell::run, p_shell.get());
        shell_thread.detach();
    }

    xcontrol& xserver_zmq_split::get_controller()
    {
        return *p_controller;
    }

    xshell& xserver_zmq_split::get_shell()
    {
        return *p_shell;
    }

    bool xserver_zmq_split::is_control_stopped() const
    {
        return m_control_stopped;
    }
}

