/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xmiddleware.hpp"

namespace xeus
{

    std::string get_controller_end_point()
    {
        return "inproc://controller";
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
