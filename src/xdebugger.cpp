/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus/xdebugger.hpp"

namespace xeus
{
    nl::json xdebugger::process_request(const nl::json& message)
    {
        return process_request_impl(message);
    }

    std::unique_ptr<xdebugger> make_null_debugger(zmq::context_t&, const xconfiguration&)
    {
        return nullptr;
    }
}

