/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"

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
        return p_server->notify_internal_listener(message);
    }
}

