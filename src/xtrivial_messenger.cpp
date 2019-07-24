/***************************************************************************
* Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Martin Renou       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus/xserver_zmq.hpp"
#include "xtrivial_messenger.hpp"

namespace xeus
{
    xtrivial_messenger::xtrivial_messenger(xserver_zmq* server)
        : p_server(server)
    {
    }

    xtrivial_messenger::~xtrivial_messenger()
    {
        p_server = nullptr;
    }

    nl::json xtrivial_messenger::send_to_shell_impl(const nl::json& message)
    {
        zmq::multipart_t wire_msg(message.dump());
        zmq::multipart_t wire_rep = p_server->notify_internal_listener(wire_msg);
        return nl::json::parse(wire_rep.popstr());
    }
}

