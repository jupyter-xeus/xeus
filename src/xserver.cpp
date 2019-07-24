/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <memory>
#include <iostream>

#include "xeus/xserver.hpp"
#include "xeus/xserver_zmq.hpp"
#include "xeus/xserver_control_main.hpp"
#include "xeus/xserver_shell_main.hpp"

namespace xeus
{
    void xserver::send_shell(zmq::multipart_t& message)
    {
        send_shell_impl(message);
    }

    void xserver::send_control(zmq::multipart_t& message)
    {
        send_control_impl(message);
    }

    void xserver::send_stdin(zmq::multipart_t& message)
    {
        send_stdin_impl(message);
    }

    void xserver::publish(zmq::multipart_t& message, channel c)
    {
        publish_impl(message, c);
    }

    void xserver::start(zmq::multipart_t& message)
    {   
        std::clog << "Run with XEUS " << XEUS_VERSION_MAJOR << "."
                                      << XEUS_VERSION_MINOR << "."
                                      << XEUS_VERSION_PATCH << std::endl;
        start_impl(message);
    }

    void xserver::abort_queue(const listener& l, long polling_interval)
    {
        abort_queue_impl(l, polling_interval);
    }

    void xserver::stop()
    {
        stop_impl();
    }

    void xserver::update_config(xconfiguration& config) const
    {
        update_config_impl(config);
    }

    void xserver::register_shell_listener(const listener& l)
    {
        m_shell_listener = l;
    }

    void xserver::register_control_listener(const listener& l)
    {
        m_control_listener = l;
    }

    void xserver::register_stdin_listener(const listener& l)
    {
        m_stdin_listener = l;
    }

    void xserver::notify_shell_listener(zmq::multipart_t& message)
    {
        m_shell_listener(message);
    }

    void xserver::notify_control_listener(zmq::multipart_t& message)
    {
        m_control_listener(message);
    }

    void xserver::notify_stdin_listener(zmq::multipart_t& message)
    {
        m_stdin_listener(message);
    }

    std::unique_ptr<xserver> make_xserver(zmq::context_t& context, const xconfiguration& config)
    {
        return std::make_unique<xserver_zmq>(context, config);
    }

    std::unique_ptr<xserver> make_xserver_control_main(zmq::context_t& context, const xconfiguration& config)
    {
        return std::make_unique<xserver_control_main>(context, config);
    }

    std::unique_ptr<xserver> make_xserver_shell_main(zmq::context_t& context, const xconfiguration& config)
    {
        return std::make_unique<xserver_shell_main>(context, config);
    }
}
