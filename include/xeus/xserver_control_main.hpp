/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_CONTROL_MAIN_HPP
#define XEUS_SERVER_CONTROL_MAIN_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xshell;
    class xpublisher;
    class xheartbeat;

    class XEUS_API xserver_control_main : public xserver
    {
    public:

        using shell_ptr = std::unique_ptr<xshell>;
        using publisher_ptr = std::unique_ptr<xpublisher>;
        using heartbeat_ptr = std::unique_ptr<xheartbeat>;

        xserver_control_main(zmq::context_t& context, const xconfiguration& config);

        virtual ~xserver_control_main();

        // The xshell object needs to call these methods
        using xserver::notify_shell_listener;
        using xserver::notify_stdin_listener;

    protected:

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void send_stdin_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message, channel c) override;

        void start_impl(zmq::multipart_t& message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

        void start_shell_thread();
        void start_publisher_thread();
        void start_heartbeat_thread();
        void stop_channels();

        // External socket for controller channel
        zmq::socket_t m_controller;
        // Internal socket for pusblishing
        zmq::socket_t m_publisher_pub;
        // Internal sockets for controlling other threads
        zmq::socket_t m_shell_controller;
        zmq::socket_t m_publisher_controller;
        zmq::socket_t m_heartbeat_controller;

        shell_ptr p_shell;
        publisher_ptr p_publisher;
        heartbeat_ptr p_heartbeat;

        bool m_request_stop;
    };
}

#endif

