/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XSERVER_HPP
#define XSERVER_HPP

#include <functional>
#include <memory>

#include "xeus.hpp"
#include "xkernel_configuration.hpp"
#include "zmq.hpp"
#include "zmq_addon.hpp"

namespace xeus
{

    class XEUS_API xserver
    {
    public:

        using listener = std::function<void(zmq::multipart_t&)>;

        virtual ~xserver() = default;

        xserver(const xserver&) = delete;
        xserver& operator=(const xserver&) = delete;

        xserver(xserver&&) = delete;
        xserver& operator=(xserver&&) = delete;

        void send_shell(zmq::multipart_t& message);
        void send_control(zmq::multipart_t& message);
        void send_stdin(zmq::multipart_t& message);
        void publish(zmq::multipart_t& message);

        void start(zmq::multipart_t& message);
        void abort_queue(const listener& l, long polling_interval);
        void stop();

        void register_shell_listener(const listener& l);
        void register_control_listener(const listener& l);
        void register_stdin_listener(const listener& l);

    protected:

        xserver() = default;

        void notify_shell_listener(zmq::multipart_t& message);
        void notify_control_listener(zmq::multipart_t& message);
        void notify_stdin_listener(zmq::multipart_t& message);

    private:

        virtual void send_shell_impl(zmq::multipart_t& message) = 0;
        virtual void send_control_impl(zmq::multipart_t& message) = 0;
        virtual void send_stdin_impl(zmq::multipart_t& message) = 0;
        virtual void publish_impl(zmq::multipart_t& message) = 0;

        virtual void start_impl(zmq::multipart_t& message) = 0;
        virtual void abort_queue_impl(const listener& l, long polling_interval) = 0;
        virtual void stop_impl() = 0;

        listener m_shell_listener;
        listener m_control_listener;
        listener m_stdin_listener;
    };

    XEUS_API
    std::unique_ptr<xserver> make_xserver(zmq::context_t& context,
                                          const xconfiguration& config);
}

#endif
