/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_MOCK_SERVER_HPP
#define XEUS_MOCK_SERVER_HPP

#include <cstddef>
#include <memory>
#include <queue>

#include "xeus/xcontrol_messenger.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xserver.hpp"

namespace xeus
{
    class xmock_server;

    class xmock_messenger : public xcontrol_messenger
    {
    public:

        xmock_messenger(xmock_server* server);
        virtual ~xmock_messenger() = default;

    private:

        nl::json send_to_shell_impl(const nl::json& message) override;

        xmock_server* p_server;
    };

    class xmock_server : public xserver
    {
    public:

        xmock_server();
        virtual ~xmock_server() = default;

        xmock_server(const xmock_server&) = delete;
        xmock_server& operator=(const xmock_server&) = delete;

        xmock_server(xmock_server&&) = delete;
        xmock_server& operator=(xmock_server&&) = delete;

        void receive_shell(xmessage message);
        void receive_control(xmessage message);
        void receive_stdin(xmessage message);

        std::size_t shell_size() const;
        xmessage read_shell();

        std::size_t control_size() const;
        xmessage read_control();

        std::size_t stdin_size() const;
        xmessage read_stdin();

        std::size_t iopub_size() const;
        xpub_message read_iopub();

        using xserver::notify_internal_listener;

    private:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(xmessage message) override;
        void send_control_impl(xmessage message) override;
        void send_stdin_impl(xmessage message) override;
        void publish_impl(xpub_message message, channel c) override;

        void start_impl(xpub_message message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

        using message_queue = std::queue<xmessage>;
        xmessage read_impl(message_queue& q);

        xmock_messenger m_messenger;

        message_queue m_shell_messages;
        message_queue m_control_messages;
        message_queue m_stdin_messages;
        std::queue<xpub_message> m_iopub_messages;
    };

    std::unique_ptr<xserver> make_mock_server(xcontext& context,
                                              const xconfiguration& config,
                                              nl::json::error_handler_t eh);

    class xmock_context : public xcontext
    {
    public:

        xmock_context();
        ~xmock_context() = default;

    };

    std::unique_ptr<xcontext> make_mock_context();
}

#endif

