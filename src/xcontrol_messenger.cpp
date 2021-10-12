/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"

#include "xeus/xcontrol_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xcontrol_messenger::~xcontrol_messenger()
    {
    }
    
    nl::json xcontrol_messenger::send_to_shell(const nl::json& message)
    {
        return send_to_shell_impl(message);
    }
}

