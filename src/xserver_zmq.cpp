/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus/xserver_zmq.hpp"
#include <thread>
#include <chrono>
#include "zmq_addon.hpp"
#include "xmiddleware.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"

namespace xeus
{

    void init_socket(zmq::socket_t& socket,
        const std::string& end_point)
    {
        socket.setsockopt(ZMQ_LINGER, get_socket_linger());
        socket.bind(end_point);
    }

    xserver_zmq::xserver_zmq(zmq::context_t& context,
                               const xconfiguration& c)
        : m_shell(context, zmq::socket_type::router),
          m_controller(context, zmq::socket_type::router),
          m_stdin(context, zmq::socket_type::router),
          m_publisher_pub(context, zmq::socket_type::pub),
          m_controller_pub(context, zmq::socket_type::pub),
          p_publisher(new xpublisher(context, c.m_transport, c.m_ip, c.m_iopub_port)),
          p_heartbeat(new xheartbeat(context, c.m_transport, c.m_ip, c.m_hb_port)),
          m_request_stop(false)
    {
        init_socket(m_shell, get_end_point(c.m_transport, c.m_ip, c.m_shell_port));
        init_socket(m_controller, get_end_point(c.m_transport, c.m_ip, c.m_control_port));
        init_socket(m_stdin, get_end_point(c.m_transport, c.m_ip, c.m_stdin_port));
        init_socket(m_publisher_pub, get_publisher_end_point());
        init_socket(m_controller_pub, get_controller_end_point());
    }

    xserver_zmq::~xserver_zmq(){}

    void xserver_zmq::send_shell_impl(zmq::multipart_t& message)
    {
        message.send(m_shell);
    }

    void xserver_zmq::send_control_impl(zmq::multipart_t& message)
    {
        message.send(m_controller);
    }

    void xserver_zmq::send_stdin_impl(zmq::multipart_t& message)
    {
        message.send(m_stdin);
        zmq::multipart_t wire_msg;
        wire_msg.recv(m_stdin);
        xserver::notify_stdin_listener(wire_msg);
    }

    void xserver_zmq::publish_impl(zmq::multipart_t& message)
    {
        message.send(m_publisher_pub);
    }

    void xserver_zmq::start_impl(zmq::multipart_t& message)
    {
        start_publisher_thread();
        start_heartbeat_thread();

        m_request_stop = false;

        publish(message);

        while (!m_request_stop)
        {
            poll(-1);
        }

        stop_channels();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::exit(0);
    }

    void xserver_zmq::start_publisher_thread()
    {
      std::thread iopub_thread(&xpublisher::run, p_publisher.get());
      iopub_thread.detach();
    }

    void xserver_zmq::start_heartbeat_thread()
    {
      std::thread hb_thread(&xheartbeat::run, p_heartbeat.get());
      hb_thread.detach();
    }

    void xserver_zmq::poll(long timeout)
    {
        zmq::pollitem_t items[] = {
            { m_controller, 0, ZMQ_POLLIN, 0 },
            { m_shell, 0, ZMQ_POLLIN, 0 }
        };

        zmq::poll(&items[0], 2, std::chrono::milliseconds(timeout));

        if (items[0].revents & ZMQ_POLLIN)
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_controller);
            xserver::notify_control_listener(wire_msg);
        }

        if (!m_request_stop && (items[1].revents & ZMQ_POLLIN))
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_shell);
            xserver::notify_shell_listener(wire_msg);
        }
    }

    void xserver_zmq::abort_queue_impl(const listener& l, long polling_interval)
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

    void xserver_zmq::stop_impl()
    {
        m_request_stop = true;
    }

    void xserver_zmq::stop_channels()
    {
        zmq::message_t stop_msg("stop", 4);
        m_controller_pub.send(stop_msg);
    }

}
