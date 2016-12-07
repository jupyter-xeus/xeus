/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xserver.hpp"

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

    void xserver::publish(zmq::multipart_t& message)
    {
        publish_impl(message);
    }

    void xserver::abort_queue(const listener& l, long polling_interval)
    {
        abort_queue_impl(l, polling_interval);
    }

    void xserver::stop(bool restart)
    {
        stop_impl(restart);
    }

    void xserver::register_shell_listener(const listener& l)
    {
        m_shell_listener = l;
    }

    void xserver::register_control_listener(const listener& l)
    {
        m_control_listener = l;
    }

    void xserver::notify_shell_listener(zmq::multipart_t& message)
    {
        m_shell_listener(message);
    }

    void xserver::notify_control_listener(zmq::multipart_t& message)
    {
        m_control_listener(message);
    }

}
