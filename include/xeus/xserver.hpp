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
#include <memory>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus.hpp"
#include "xkernel_configuration.hpp"
#include "xcontrol_messenger.hpp"

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

        using listener = std::function<void(zmq::multipart_t&)>;
        using internal_listener = std::function<zmq::multipart_t(zmq::multipart_t&)>;

        virtual ~xserver() = default;

        xserver(const xserver&) = delete;
        xserver& operator=(const xserver&) = delete;

        xserver(xserver&&) = delete;
        xserver& operator=(xserver&&) = delete;

        xcontrol_messenger& get_control_messenger();

        void send_shell(zmq::multipart_t& message);
        void send_control(zmq::multipart_t& message);
        void send_stdin(zmq::multipart_t& message);
        void publish(zmq::multipart_t& message, channel c);

        void start(zmq::multipart_t& message);
        void abort_queue(const listener& l, long polling_interval);
        void stop();
        void update_config(xconfiguration& config) const;

        void register_shell_listener(const listener& l);
        void register_control_listener(const listener& l);
        void register_stdin_listener(const listener& l);
        void register_internal_listener(const internal_listener& l);

    protected:

        xserver() = default;

        void notify_shell_listener(zmq::multipart_t& message);
        void notify_control_listener(zmq::multipart_t& message);
        void notify_stdin_listener(zmq::multipart_t& message);
        zmq::multipart_t notify_internal_listener(zmq::multipart_t& message);

    private:

        virtual xcontrol_messenger& get_control_messenger_impl() = 0;

        virtual void send_shell_impl(zmq::multipart_t& message) = 0;
        virtual void send_control_impl(zmq::multipart_t& message) = 0;
        virtual void send_stdin_impl(zmq::multipart_t& message) = 0;
        virtual void publish_impl(zmq::multipart_t& message, channel c) = 0;

        virtual void start_impl(zmq::multipart_t& message) = 0;
        virtual void abort_queue_impl(const listener& l, long polling_interval) = 0;
        virtual void stop_impl() = 0;
        virtual void update_config_impl(xconfiguration& config) const = 0;

        listener m_shell_listener;
        listener m_control_listener;
        listener m_stdin_listener;
        internal_listener m_internal_listener;
    };

    XEUS_API
    std::unique_ptr<xserver> make_xserver(zmq::context_t& context, const xconfiguration& config);

    XEUS_API
    std::unique_ptr<xserver> make_xserver_control_main(zmq::context_t& context, const xconfiguration& config);

    XEUS_API
    std::unique_ptr<xserver> make_xserver_shell_main(zmq::context_t& context, const xconfiguration& config);
}

#endif
