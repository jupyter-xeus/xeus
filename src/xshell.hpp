/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_HPP
#define XEUS_SHELL_HPP

#include <string>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "xeus/xmessage.hpp"

namespace xeus
{
    class xserver_zmq_split;

    class xshell
    {
    public:

        using listener = std::function<void(xmessage)>;

        xshell(zmq::context_t& context,
               const std::string& transport,
               const std::string& ip,
               const std::string& shell_port,
               const std::string& sdtin_port,
               xserver_zmq_split* server);

        ~xshell();

        std::string get_shell_port() const;
        std::string get_stdin_port() const;

        void run();

        void send_shell(zmq::multipart_t& message);
        void send_stdin(zmq::multipart_t& message);
        void publish(zmq::multipart_t& message);
        void abort_queue(const listener& l, long polling_interval);

        void reply_to_controller(zmq::multipart_t& message);

    private:

        zmq::socket_t m_shell;
        zmq::socket_t m_stdin;
        zmq::socket_t m_publisher_pub;
        zmq::socket_t m_controller;
        xserver_zmq_split* p_server;
    };
}

#endif

