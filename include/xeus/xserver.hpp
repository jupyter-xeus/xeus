/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_HPP
#define XEUS_SERVER_HPP

#include <functional>

#include "xeus.hpp"
#include "xkernel_configuration.hpp"
#include "xcontrol_messenger.hpp"
#include "xmessage.hpp"

namespace xeus
{
    enum class channel
    {
        SHELL,
        CONTROL
    };

    class XEUS_API xserver
    {
    public:

        using listener = std::function<void(xmessage)>;
        using internal_listener = std::function<nl::json(nl::json)>;

        virtual ~xserver() = default;

        xserver(const xserver&) = delete;
        xserver& operator=(const xserver&) = delete;

        xserver(xserver&&) = delete;
        xserver& operator=(xserver&&) = delete;

        xcontrol_messenger& get_control_messenger();

        void send_shell(xmessage message);
        void send_control(xmessage message);
        void send_stdin(xmessage message);
        void publish(xpub_message message, channel c);

        void start(xpub_message message);
        void abort_queue(const listener& l, long polling_interval);
        void stop();
        void update_config(xconfiguration& config) const;

        void register_shell_listener(const listener& l);
        void register_control_listener(const listener& l);
        void register_stdin_listener(const listener& l);
        void register_internal_listener(const internal_listener& l);

    protected:

        xserver() = default;

        void notify_shell_listener(xmessage msg);
        void notify_control_listener(xmessage msg);
        void notify_stdin_listener(xmessage msg);
        nl::json notify_internal_listener(nl::json msg);

    private:

        virtual xcontrol_messenger& get_control_messenger_impl() = 0;

        virtual void send_shell_impl(xmessage message) = 0;
        virtual void send_control_impl(xmessage message) = 0;
        virtual void send_stdin_impl(xmessage message) = 0;
        virtual void publish_impl(xpub_message message, channel c) = 0;

        virtual void start_impl(xpub_message message) = 0;
        virtual void abort_queue_impl(const listener& l, long polling_interval) = 0;
        virtual void stop_impl() = 0;
        virtual void update_config_impl(xconfiguration& config) const = 0;

        listener m_shell_listener;
        listener m_control_listener;
        listener m_stdin_listener;
        internal_listener m_internal_listener;
    };
}

#endif
