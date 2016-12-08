/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xheartbeat.hpp"
#include "zmq_addon.hpp"
#include "xmiddleware.hpp"

namespace xeus
{

    xheartbeat::xheartbeat(zmq::context_t& context,
                           const std::string& transport,
                           const std::string& ip,
                           const std::string& port)
        : m_heartbeat(context, zmq::socket_type::router),
          m_controller(context, zmq::socket_type::sub)
    {
        m_heartbeat.setsockopt(ZMQ_LINGER, get_socket_linger());
        m_heartbeat.bind(get_end_point(transport, ip, port));
        m_controller.connect(get_controller_end_point());
        m_controller.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    }

    void xheartbeat::run()
    {
        zmq::pollitem_t items[] = {
            { m_heartbeat, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 }
        };

        while (true)
        {
            zmq::poll(&items[0], std::size(items), -1);

            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::multipart_t wire_msg;
                wire_msg.recv(m_heartbeat);
                wire_msg.send(m_heartbeat);
            }

            if (items[1].revents & ZMQ_POLLIN)
            {
                // stop or restart message
                break;
            }
        }
    }

}
