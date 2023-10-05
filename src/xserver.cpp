/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "xeus/xserver.hpp"

namespace xeus
{
    xcontrol_messenger& xserver::get_control_messenger()
    {
        return get_control_messenger_impl();
    }

    void xserver::send_shell(xmessage message)
    {
        send_shell_impl(std::move(message));
    }

    void xserver::send_control(xmessage message)
    {
        send_control_impl(std::move(message));
    }

    void xserver::send_stdin(xmessage message)
    {
        send_stdin_impl(std::move(message));
    }

    void xserver::publish(xpub_message message, channel c)
    {
        publish_impl(std::move(message), c);
    }

    void xserver::start(xpub_message message)
    {   
#ifndef EMSCRIPTEN
        std::clog << "Run with XEUS " << XEUS_VERSION_MAJOR << "."
                                      << XEUS_VERSION_MINOR << "."
                                      << XEUS_VERSION_PATCH << std::endl;
#endif
        start_impl(std::move(message));
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

    void xserver::register_internal_listener(const internal_listener& l)
    {
        m_internal_listener = l;
    }

    void xserver::notify_shell_listener(xmessage msg)
    {
        m_shell_listener(std::move(msg));
    }

    void xserver::notify_control_listener(xmessage msg)
    {
        m_control_listener(std::move(msg));
    }

    void xserver::notify_stdin_listener(xmessage msg)
    {
        m_stdin_listener(std::move(msg));
    }

    nl::json xserver::notify_internal_listener(nl::json msg)
    {
        return m_internal_listener(std::move(msg));
    }
}
