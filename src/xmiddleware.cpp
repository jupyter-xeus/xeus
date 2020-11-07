/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
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

    std::string find_free_port_impl(zmq::socket_t& socket,
                                    const std::string& transport,
                                    const std::string& ip,
                                    std::size_t max_tries,
                                    int start,
                                    int stop)
    {
            std::random_device r;
            std::default_random_engine generator(r());
            std::uniform_int_distribution<int> distribution(start, stop);
            std::size_t tries(0);
            std::string rd_port;

            do
            {
                rd_port = std::to_string(distribution(generator));
            }
            while (++tries <= max_tries && zmq_bind(socket, get_end_point(transport, ip, rd_port).c_str()) != 0);

            if(tries > max_tries)
            {
                rd_port = "";
            }

            return rd_port;
    }

    void init_socket(zmq::socket_t& socket,
                     const std::string& transport,
                     const std::string& ip,
                     const std::string& port)
    {
        socket.set(zmq::sockopt::linger, get_socket_linger());

        if (!port.empty())
        {
            socket.bind(get_end_point(transport, ip, port));
        }
        else
        {
            find_free_port_impl(socket, transport, ip, 100, 49152, 65536);
        }
    }

    void init_socket(zmq::socket_t& socket, const std::string& end_point)
    {
        socket.set(zmq::sockopt::linger, get_socket_linger());
        socket.bind(end_point);
    }

    std::string get_socket_port(const zmq::socket_t& socket)
    {
        std::string end_point = socket.get(zmq::sockopt::last_endpoint, 32);
        return end_point.substr(end_point.find_last_of(":") + 1);
    }

    std::string find_free_port(std::size_t max_tries, int start, int stop)
    {
        static const std::string transport = "tcp";
        static const std::string ip = "127.0.0.1";
        zmq::context_t ctx;
        zmq::socket_t socket(ctx, zmq::socket_type::req);
        std::string port = find_free_port_impl(socket, transport, ip, max_tries, start, stop);
        socket.unbind(get_end_point(transport, ip, port));
        return port;
    }
}
