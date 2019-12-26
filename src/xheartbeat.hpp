/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_HEARTBEAT_HPP
#define XEUS_HEARTBEAT_HPP

#include <string>

#include "zmq.hpp"

namespace xeus
{
    class xheartbeat
    {
    public:

        xheartbeat(zmq::context_t& context,
                   const std::string& transport,
                   const std::string& ip,
                   const std::string& port);

        ~xheartbeat();

        std::string get_port() const;

        void run();

    private:

        zmq::socket_t m_heartbeat;
        zmq::socket_t m_controller;
    };
}

#endif
