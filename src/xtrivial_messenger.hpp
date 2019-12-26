/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_TRIVIAL_MESSENGER_HPP
#define XEUS_TRIVIAL_MESSENGER_HPP

#include "nlohmann/json.hpp"

#include "xeus/xeus.hpp"
#include "xeus/xcontrol_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class xserver_zmq;

    class XEUS_API xtrivial_messenger : public xcontrol_messenger
    {
    public:
        
        explicit xtrivial_messenger(xserver_zmq* server);
        virtual ~xtrivial_messenger ();

    private:

        nl::json send_to_shell_impl(const nl::json& message) override;

        xserver_zmq* p_server;
    };
}

#endif
