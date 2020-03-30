/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <mutex>

#include "xserver_utils.hpp"

namespace xeus
{
    std::mutex xserver_utils_io_mutex;

    void console_log(const std::string& message)
    {
        std::lock_guard<std::mutex> lock(xserver_utils_io_mutex);
        std::clog << message << std::endl;
    }

    void bind_socket(const std::string& agent_name,
                     const std::string& socket_name,
                     zmq::socket_t& socket,
                     const std::string& transport,
                     const std::string& ip,
                     const std::string& port)
    {
        try
        {
            init_socket(socket, transport, ip, port);
            std::clog << agent_name << ": socket " << socket_name << "bound to "
                      << get_end_point(transport, ip, port) << std::endl;
        }
        catch(zmq::error_t& e)
        {
            std::clog << agent_name << ": failed to bind socket " << socket_name << '\n';
            std::clog << e.what() << std::endl;
            throw e;
        }
    }

    void bind_socket(const std::string& agent_name,
                     const std::string& socket_name,
                     zmq::socket_t& socket,
                     const std::string& end_point)
    {
        try
        {
            init_socket(socket, end_point);
            std::clog << agent_name << ": socket " << socket_name << "bound to "
                      << end_point << std::endl;
        }
        catch(zmq::error_t& e)
        {
            std::clog << agent_name << ": failed to bind socket " << socket_name << '\n';
            std::clog << e.what() << std::endl;
            throw e;
        }
    }

    void connect_socket(const std::string& agent_name,
                        const std::string& socket_name,
                        zmq::socket_t& socket,
                        const std::string& end_point)
    {
        try
        {
            socket.setsockopt(ZMQ_LINGER, get_socket_linger());
            socket.connect(end_point);
            std::clog << agent_name << ": socket " << socket_name << "connected to "
                      << get_socket_port(socket) << std::endl;
        }
        catch(zmq::error_t& e)
        {
            std::clog << agent_name << ": failed to connect socket " << socket_name << '\n';
            std::clog << e.what() << std::endl;
            throw e;
        }
    }
}

