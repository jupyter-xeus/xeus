/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_DEBUGGER_HPP
#define XEUS_DEBUGGER_HPP

#include "nlohmann/json.hpp"

#include "xeus.hpp"
#include "xeus_context.hpp"
#include "xkernel_configuration.hpp"
#include "xcontrol_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class XEUS_API xdebugger
    {
    public:

        virtual ~xdebugger();

        xdebugger(const xdebugger&) = delete;
        xdebugger& operator=(const xdebugger&) = delete;

        xdebugger(xdebugger&&) = delete;
        xdebugger& operator=(xdebugger&&) = delete;

        void register_control_messenger(xcontrol_messenger& messenger);

        nl::json process_request(const nl::json& header,
                                 const nl::json& message);

    protected:

        xdebugger();

        xcontrol_messenger& get_control_messenger();

    private:

        virtual nl::json process_request_impl(const nl::json& header,
                                              const nl::json& message) = 0;

        xcontrol_messenger* p_messenger;
    };

    XEUS_API
    std::unique_ptr<xdebugger> make_null_debugger(xcontext& context,
                                                  const xconfiguration& kernel_configuration,
                                                  const std::string& user_name,
                                                  const std::string& session_id,
                                                  const nl::json& debugger_config);
            
}

#endif

