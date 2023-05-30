/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus/xlogger.hpp"

#include "nlohmann/json.hpp"

namespace nl = nlohmann;

namespace xeus
{
    void xlogger::log_received_message(const xmessage& message, channel c) const
    {
        log_received_message_impl(message, c);
    }

    void xlogger::log_sent_message(const xmessage& message, channel c) const
    {
        log_sent_message_impl(message, c);
    }

    void xlogger::log_iopub_message(const xpub_message& message) const
    {
        log_iopub_message_impl(message);
    }

    void xlogger::log_message(const std::string& socket_info,
                              const nl::json& header,
                              const nl::json& parent_header,
                              const nl::json& metadata,
                              const nl::json& content_json) const
    {
        log_message_impl(socket_info,
                         header,
                         parent_header,
                         metadata,
                         content_json);
    }
}

