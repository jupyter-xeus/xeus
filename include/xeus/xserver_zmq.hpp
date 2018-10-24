/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XSERVER_IMPL_HPP
#define XSERVER_IMPL_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xpublisher;
    class xheartbeat;

    class XEUS_API xserver_zmq : public xserver
    {

    public:

        using publisher_ptr = std::unique_ptr<xpublisher>;
        using heartbeat_ptr = std::unique_ptr<xheartbeat>;

        xserver_zmq(zmq::context_t& context,
                     const xconfiguration& config);

        virtual ~xserver_zmq();

    protected:

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void send_stdin_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message) override;

        void start_impl(zmq::multipart_t& message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;

        void poll(long timeout);
        void start_publisher_thread();
        void start_heartbeat_thread();
        void stop_channels();

        zmq::socket_t m_shell;
        zmq::socket_t m_controller;
        zmq::socket_t m_stdin;
        zmq::socket_t m_publisher_pub;
        zmq::socket_t m_publisher_controller;
        zmq::socket_t m_heartbeat_controller;

        publisher_ptr p_publisher;
        heartbeat_ptr p_heartbeat;

        bool m_request_stop;
    };

}

#endif
