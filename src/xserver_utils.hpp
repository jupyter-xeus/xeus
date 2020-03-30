/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_UTILS_HPP
#define XEUS_SERVER_UTILS_HPP

#include <string>

#include "zmq.hpp"

#include "xeus/xmiddleware.hpp"

namespace xeus
{

    void console_log(const std::string& message);

    void bind_socket(const std::string& agent_name,
                     const std::string& socket_name,
                     zmq::socket_t& socket,
                     const std::string& transport,
                     const std::string& ip,
                     const std::string& port);

    void bind_socket(const std::string& agent_name,
                     const std::string& socket_name,
                     zmq::socket_t& socket,
                     const std::string& end_point);

    void connect_socket(const std::string& agent_name,
                        const std::string& socket_name,
                        zmq::socket_t& socket,
                        const std::string& end_point);

}

#endif

