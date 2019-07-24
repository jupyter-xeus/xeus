/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_SHELL_MAIN_HPP
#define XEUS_SERVER_SHELL_MAIN_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xcontrol;
    class xpublisher;
    class xheartbeat;

    class XEUS_API xserver_shell_main : public xserver
    {
    public:

        using controller_ptr = std::unique_ptr<xcontrol>;
        using publisher_ptr = std::unique_ptr<xpublisher>;
        using heartbeat_ptr = std::unique_ptr<xheartbeat>;

        xserver_shell_main(zmq::context_t& context, const xconfiguration& config);

        virtual ~xserver_shell_main();

        // The xcontrol object needs to call this method
        using xserver::notify_control_listener;

        void notify_control_stopped();

    protected:

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void send_stdin_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message, channel c) override;

        void start_impl(zmq::multipart_t& message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

        void start_control_thread();
        void start_publisher_thread();
        void start_heartbeat_thread();

        // External socket for shell and stdin channels
        zmq::socket_t m_shell;
        zmq::socket_t m_stdin;
        // Internal socket for pusblishing
        zmq::socket_t m_publisher_pub;
        // Internal socket for communication with controller
        zmq::socket_t m_controller;

        controller_ptr p_controller;
        publisher_ptr p_publisher;
        heartbeat_ptr p_heartbeat;

        bool m_control_stopped;
    };
}

#endif

