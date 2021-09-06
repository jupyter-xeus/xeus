/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_ZMQ_SERIALIZER_HPP
#define XEUS_ZMQ_SERIALIZER_HPP

#include "zmq_addon.hpp"

#include "xeus.hpp"
#include "xauthentication.hpp"
#include "xmessage.hpp"

namespace xeus
{
    class XEUS_API xzmq_serializer
    {
    public:

        static zmq::multipart_t serialize(xmessage&& msg,
                                          const xauthentication& auth,
                                          nl::json::error_handler_t error_handler = nl::json::error_handler_t::strict);

        static xmessage deserialize(zmq::multipart_t& wire_msg,
                                    const xauthentication& auth);

        static zmq::multipart_t serialize_iopub(xpub_message&& msg,
                                                const xauthentication& auth,
                                                nl::json::error_handler_t error_handler = nl::json::error_handler_t::strict);

        static xpub_message deserialize_iopub(zmq::multipart_t& wire_msg,
                                              const xauthentication& auth);
    };

}

#endif

