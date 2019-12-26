/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONTROL_HPP
#define XEUS_CONTROL_HPP

#include <string>

#include "zmq.hpp"
#include "zmq_addon.hpp"
#include "xzmq_messenger.hpp"

namespace xeus
{
    class xserver_zmq_split;

    class xcontrol
    {
    public:

        using listener = std::function<void(zmq::multipart_t&)>;

        xcontrol(zmq::context_t& context,
                 const std::string& transport,
                 const std::string& ip,
                 const std::string& control_port,
                 xserver_zmq_split* server);

        ~xcontrol();

        std::string get_port() const;

        void connect_messenger();
        xcontrol_messenger& get_messenger();

        void run();
        void stop();

        void send_control(zmq::multipart_t& message);
        void publish(zmq::multipart_t& message);

    private:

        zmq::socket_t m_control;
        zmq::socket_t m_publisher_pub;
        // Internal sockets for controlling other threads
        xzmq_messenger m_messenger;
        xserver_zmq_split* p_server;
        bool m_request_stop;
    };
}

#endif

