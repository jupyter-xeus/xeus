/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xmiddleware.hpp"

namespace xeus
{
    std::string get_publisher_controller_end_point()
    {
        return "inproc://publisher_controller";
    }

    std::string get_heartbeat_controller_end_point()
    {
        return "inproc://heartbeat_controller";
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
}
