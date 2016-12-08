/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XSERVER_IMPL_HPP
#define XSERVER_IMPL_HPP

#include "xserver.hpp"
#include "xpublisher.hpp"
#include "xheartbeat.hpp"
#include "xconfiguration.hpp"

namespace xeus
{

    class xserver_impl : public xserver
    {

    public:

        xserver_impl(zmq::context_t& context,
                     const xconfiguration& config);

        virtual ~xserver_impl() = default;

        void run();
        bool need_restart() const;

    private:

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message) override;

        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl(bool restart) override;

        void stop_channels();

        zmq::socket_t m_shell;
        zmq::socket_t m_controller;
        zmq::socket_t m_stdin;
        zmq::socket_t m_publisher_pub;
        zmq::socket_t m_controller_pub;

        xpublisher m_publisher;
        xheartbeat m_heartbeat;

        bool m_request_stop;
        bool m_need_restart;
    };

}

#endif
