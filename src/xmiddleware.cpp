/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <random>

#include "zmq_addon.hpp"
#include "xeus/xmiddleware.hpp"

namespace xeus
{
    std::string get_controller_end_point(const std::string& channel)
    {
        return "inproc://" + channel + "_controller";
    }

    std::string get_publisher_end_point()
    {
        return "inproc://publisher";
    }

    std::string get_end_point(const std::string& transport,
                              const std::string& ip,
                              const std::string& port)
    {
        char sep = (transport == "tcp") ? ':' : '-';
        return transport + "://" + ip + sep + port;
    }

    int get_socket_linger()
    {
        return 1000;
    }

    void init_socket(zmq::socket_t& socket,
                     const std::string& transport,
                     const std::string& ip,
                     const std::string& port)
    {
        socket.setsockopt(ZMQ_LINGER, get_socket_linger());

        if (!port.empty())
        {
            socket.bind(get_end_point(transport, ip, port));
        }
        else
        {
            std::random_device r;
            std::default_random_engine generator(r());
            std::uniform_int_distribution<int> distribution(49152, 65536);
            std::size_t max_tries(100);
            std::size_t tries(0);
            std::string rd_port;

            do
            {
                rd_port = std::to_string(distribution(generator));
            }
            while (++tries <= max_tries && zmq_bind(socket, get_end_point(transport, ip, rd_port).c_str()) != 0);
        }
    }

    void init_socket(zmq::socket_t& socket, const std::string& end_point)
    {
        socket.setsockopt(ZMQ_LINGER, get_socket_linger());
        socket.bind(end_point);
    }

    std::string get_socket_port(const zmq::socket_t& socket)
    {
        char opt[32];
        std::size_t len = sizeof(opt);
        socket.getsockopt(ZMQ_LAST_ENDPOINT, &opt, &len);
        std::string end_point(opt, len);
        return end_point.substr(end_point.find_last_of(":") + 1);
    }
}
