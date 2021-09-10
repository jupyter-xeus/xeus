/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONTROL_MESSENGER_HPP
#define XEUS_CONTROL_MESSENGER_HPP

#include "nlohmann/json.hpp"

#include "xeus/xeus.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class XEUS_API xcontrol_messenger
    {
    public:

        virtual ~xcontrol_messenger();

        xcontrol_messenger(const xcontrol_messenger&) = delete;
        xcontrol_messenger& operator=(const xcontrol_messenger&) = delete;

        xcontrol_messenger(xcontrol_messenger&&) = delete;
        xcontrol_messenger& operator=(xcontrol_messenger&&) = delete;

        nl::json send_to_shell(const nl::json& message);

    protected:

        xcontrol_messenger() = default;

    private:

        virtual nl::json send_to_shell_impl(const nl::json& message) = 0;
    };
}

#endif

