/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XHEARTBEAT_HPP
#define XHEARTBEAT_HPP

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

        void run();

    private:

        zmq::socket_t m_heartbeat;
        zmq::socket_t m_controller;
    };

}

#endif
