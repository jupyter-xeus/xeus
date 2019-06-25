/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <thread>
#include <chrono>

#include "xeus/xserver_zmq.hpp"
#include "xeus/xguid.hpp"
#include "xeus/xmiddleware.hpp"
#include "zmq_addon.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"

namespace xeus
{

    xserver_zmq::xserver_zmq(zmq::context_t& context, const xconfiguration& config)
        : m_shell(context, zmq::socket_type::router)
        , m_controller(context, zmq::socket_type::router)
        , m_stdin(context, zmq::socket_type::router)
        , m_publisher_pub(context, zmq::socket_type::pub)
        , m_publisher_controller(context, zmq::socket_type::req)
        , m_heartbeat_controller(context, zmq::socket_type::req)
        , p_publisher(new xpublisher(context, config.m_transport, config.m_ip, config.m_iopub_port))
        , p_heartbeat(new xheartbeat(context, config.m_transport, config.m_ip, config.m_hb_port))
        , m_request_stop(false)
    {
        init_socket(m_shell, config.m_transport, config.m_ip, config.m_shell_port);
        init_socket(m_controller, config.m_transport, config.m_ip, config.m_control_port);
        init_socket(m_stdin, config.m_transport, config.m_ip, config.m_stdin_port);
        m_publisher_pub.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_pub.connect(get_publisher_end_point());

        m_publisher_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_publisher_controller.connect(get_controller_end_point("publisher"));
        m_heartbeat_controller.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_heartbeat_controller.connect(get_controller_end_point("heartbeat"));
    }

    xserver_zmq::~xserver_zmq()
    {
    }

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

    void xserver_zmq::publish_impl(zmq::multipart_t& message, channel)
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
        zmq::pollitem_t items[]
            = { { m_controller, 0, ZMQ_POLLIN, 0 }, { m_shell, 0, ZMQ_POLLIN, 0 } };

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

    void xserver_zmq::update_config_impl(xconfiguration& config) const
    {
        config.m_control_port = get_socket_port(m_controller);
        config.m_shell_port = get_socket_port(m_shell);
        config.m_stdin_port = get_socket_port(m_stdin);
        config.m_iopub_port = p_publisher->get_port();
        config.m_hb_port = p_heartbeat->get_port();
    }

    void xserver_zmq::stop_channels()
    {
        zmq::message_t stop_msg("stop", 4);
        zmq::message_t response;

        // Wait for publisher answer
        m_publisher_controller.send(stop_msg);
        m_publisher_controller.recv(&response);

        // Wait for heartbeat answer
        m_heartbeat_controller.send(stop_msg);
        m_heartbeat_controller.recv(&response);
    }
}
