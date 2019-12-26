/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_ZMQ_SPLIT_HPP
#define XEUS_SERVER_ZMQ_SPLIT_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xeus
{
    class xcontrol;
    class xheartbeat;
    class xpublisher;
    class xshell;

    class XEUS_API xserver_zmq_split : public xserver
    {
    public:

        using controller_ptr = std::unique_ptr<xcontrol>;
        using heartbeat_ptr = std::unique_ptr<xheartbeat>;
        using publisher_ptr = std::unique_ptr<xpublisher>;
        using shell_ptr = std::unique_ptr<xshell>;

        xserver_zmq_split(zmq::context_t& context, const xconfiguration& config);
        virtual ~xserver_zmq_split();

        // The xcontrol object needs to call this method
        using xserver::notify_control_listener;
        // The xshell object needs to call these methods
        using xserver::notify_shell_listener;
        using xserver::notify_stdin_listener;
        using xserver::notify_internal_listener;

        void notify_control_stopped();

    protected:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(zmq::multipart_t& message) override;
        void send_control_impl(zmq::multipart_t& message) override;
        void send_stdin_impl(zmq::multipart_t& message) override;
        void publish_impl(zmq::multipart_t& message, channel c) override;

        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

        void start_control_thread();
        void start_heartbeat_thread();
        void start_publisher_thread();
        void start_shell_thread();

        xcontrol& get_controller();
        xshell& get_shell();

        bool is_control_stopped() const;

    private:

        controller_ptr p_controller;
        heartbeat_ptr p_heartbeat;
        publisher_ptr p_publisher;
        shell_ptr p_shell;

        bool m_control_stopped;
    };
}

#endif

