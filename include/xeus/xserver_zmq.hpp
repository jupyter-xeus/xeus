/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_IMPL_HPP
#define XEUS_SERVER_IMPL_HPP

#include "zmq.hpp"

#include "xeus.hpp"
#include "xeus_context.hpp"
#include "xserver.hpp"
#include "xauthentication.hpp"
#include "xkernel_configuration.hpp"

namespace xeus
{
    class xpublisher;
    class xheartbeat;
    class xtrivial_messenger;

    class XEUS_API xserver_zmq : public xserver
    {
    public:

        using publisher_ptr = std::unique_ptr<xpublisher>;
        using heartbeat_ptr = std::unique_ptr<xheartbeat>;

        xserver_zmq(zmq::context_t& context,
                    const xconfiguration& config,
                    nl::json::error_handler_t eh);

        virtual ~xserver_zmq();

        using xserver::notify_internal_listener;

    protected:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(xmessage msg) override;
        void send_control_impl(xmessage msg) override;
        void send_stdin_impl(xmessage msg) override;
        void publish_impl(xpub_message msg, channel c) override;

        void start_impl(xpub_message msg) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

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

        using trivial_messenger_ptr = std::unique_ptr<xtrivial_messenger>;
        trivial_messenger_ptr p_messenger;

        using authentication_ptr = std::unique_ptr<xauthentication>;
        authentication_ptr p_auth;
        nl::json::error_handler_t m_error_handler;
        
        bool m_request_stop;
    };

    XEUS_API
    std::unique_ptr<xserver> make_xserver_zmq(xcontext& context,
                                              const xconfiguration& config,
                                              nl::json::error_handler_t eh = nl::json::error_handler_t::strict);
}

#endif
