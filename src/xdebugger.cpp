/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "nlohmann/json.hpp"

#include "xeus/xdebugger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xdebugger::xdebugger()
        : p_messenger(nullptr)
    {
    }

    xdebugger::~xdebugger()
    {
        p_messenger = nullptr;
    }

    void xdebugger::register_control_messenger(xcontrol_messenger& messenger)
    {
        p_messenger = &messenger;
    }

    nl::json xdebugger::process_request(const nl::json& header,
                                        const nl::json& message)
    {
        return process_request_impl(header, message);
    }

    xcontrol_messenger& xdebugger::get_control_messenger()
    {
        return *p_messenger;
    }

    std::unique_ptr<xdebugger> make_null_debugger(xcontext&,
                                                  const xconfiguration&,
                                                  const std::string&,
                                                  const std::string&,
                                                  const nl::json&)
    {
        return nullptr;
    }
}

