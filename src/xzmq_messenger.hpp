/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_ZMQ_MESSENGER_HPP
#define XEUS_ZMQ_MESSENGER_HPP

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "nlohmann/json.hpp"

#include "xeus/xeus.hpp"
#include "xeus/xcontrol_messenger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class XEUS_API xzmq_messenger : public xcontrol_messenger
    {
    public:

        explicit xzmq_messenger(zmq::context_t& context);
        virtual ~xzmq_messenger();

        void connect();
        void stop_channels();

    private:

        nl::json send_to_shell_impl(const nl::json& message) override;

        zmq::socket_t m_shell_controller;
        zmq::socket_t m_publisher_controller;
        zmq::socket_t m_heartbeat_controller;
    };
}

#endif
