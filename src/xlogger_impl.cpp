/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <fstream>
#include <iostream>

#include "xeus/xjson.hpp"
#include "xeus/xmessage.hpp"
#include "xlogger_impl.hpp"

namespace nl = nlohmann;

namespace xeus
{

    /********************************
     * xlogger_nolog implementation *
     ********************************/

    void xlogger_nolog::log_received_message_impl(const xmessage&, xlogger::channel) const
    {
    }

    void xlogger_nolog::log_sent_message_impl(const xmessage&, xlogger::channel) const
    {
    }

    void xlogger_nolog::log_iopub_message_impl(const xpub_message&) const
    {
    }
    
    void xlogger_nolog::log_message_impl(const std::string&,
                                         const nl::json&,
                                         const nl::json&,
                                         const nl::json&,
                                         const nl::json&) const
    {
    }

    /*********************************
     * xlogger_common implementation *
     *********************************/

    namespace
    {
        const std::array<std::string, xlogger::CHANNEL_SIZE> channel_str = { "shell", "control", "stdin", "heartbeat" };


    
        /**
         * @copyright Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
         * @sa http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
         */
        std::uint8_t decode_utf8(std::uint8_t& state, std::uint32_t& codep, const std::uint8_t byte) noexcept
        {
            static const uint8_t UTF8_ACCEPT = 0;
            static const std::array<std::uint8_t, 400> utf8d =
            {
                {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00..1F
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20..3F
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 40..5F
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 60..7F
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 80..9F
                    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // A0..BF
                    8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C0..DF
                    0xA, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3, // E0..EF
                    0xB, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, // F0..FF
                    0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1, // s0..s0
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, // s1..s2
                    1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, // s3..s4
                    1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, // s5..s6
                    1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // s7..s8
                }
            };

            const std::uint8_t type = utf8d[byte];

            codep = (state != UTF8_ACCEPT)
                    ? (byte & 0x3fu) | (codep << 6u)
                    : (0xFFu >> type) & (byte);

            state = utf8d[256u + state * 16u + type];
            return state;
        }

        bool is_utf8_valid(const std::string& str)
        {
            std::uint32_t codepoint(0);
            std::uint8_t state(0);

            for (std::size_t i = 0; i < str.size(); ++i)
            {
                auto byte = static_cast<uint8_t>(str[i]);
                decode_utf8(state, codepoint, byte);
            }
            return !state;
        }
    }

    xlogger_common::xlogger_common(xlogger::level l, xlogger_ptr next_logger)
        : p_next_logger(next_logger != nullptr ? std::move(next_logger) : std::make_unique<xlogger_nolog>())
        , m_level(l)
    {
    }

    xlogger_common::~xlogger_common()
    {
    }
    
    void xlogger_common::log_received_message_impl(const xmessage& message, xlogger::channel c) const
    {
        std::string id = message.identities()[0];
        std::string socket_info = "XEUS: received message on "
                                + channel_str[c] + " - "
                                + (is_utf8_valid(id) ? id : "invalid UTF8");
        xlogger::log_message(socket_info,
                             message.header(),
                             message.parent_header(),
                             message.metadata(),
                             message.content());
    }

    void xlogger_common::log_sent_message_impl(const xmessage& message, xlogger::channel c) const
    {
        std::string id = message.identities()[0];
        std::string socket_info = "XEUS: sent message on "
                                + channel_str[c] + " - "
                                + (is_utf8_valid(id) ? id : "invalid UTF8");
        xlogger::log_message(socket_info,
                             message.header(),
                             message.parent_header(),
                             message.metadata(),
                             message.content());
    }

    void xlogger_common::log_iopub_message_impl(const xpub_message& message) const
    {
        std::string socket_info = "XEUS: sent message on iopub - "
                                + message.topic();
        xlogger::log_message(socket_info,
                             message.header(),
                             message.parent_header(),
                             message.metadata(),
                             message.content());
    }
    
    void xlogger_common::log_message_impl(const std::string& socket_info,
                                          const nl::json& header,
                                          const nl::json& parent_header,
                                          const nl::json& metadata,
                                          const nl::json& json_content) const
    {
        nl::json message;
        message["msg_type"] = header.value("msg_type", "");
        switch(m_level)
        {
        case msg_type:
            break;
        case content:
            message["content"] = json_content;
            break;
        case full:
        default:
            {
                message["header"] = header;
                message["parent_header"] = parent_header;
                message["metadata"] = metadata;
                message["content"] = json_content;
            }
            break;
        }

        log_json_message(socket_info, message);
        p_next_logger->log_message(socket_info, header, parent_header, metadata, json_content);
    }

    /**********************************
     * xlogger_console implementation *
     **********************************/

    xlogger_console::xlogger_console(xlogger::level l, xlogger_ptr next_logger)
        : xlogger_common(l, std::move(next_logger))
    {
    }

    void xlogger_console::log_json_message(const std::string& socket_info,
                                           const nl::json& json_message) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << socket_info << '\n' << json_message.dump(4) << std::endl;
    }

    /*******************************
     * xlogger_file implementation *
     *******************************/

    xlogger_file::xlogger_file(xlogger::level l,
                               const std::string& file_name,
                               xlogger_ptr next_logger)
        : xlogger_common(l, std::move(next_logger))
        , m_file_name(file_name)
    {
    }

    void xlogger_file::log_json_message(const std::string& socket_info,
                                        const nl::json& json_message) const
    {
        nl::json log;
        log["info"] = socket_info;
        log["message"] = json_message;
        std::lock_guard<std::mutex> lock(m_mutex);
        std::ofstream out(m_file_name, std::ios_base::app);
        out << log.dump(4) << std::endl;
    }

    /************************************
     * Builder functions implementation *
     ************************************/

    std::unique_ptr<xlogger> make_console_logger(xlogger::level log_level,
                                                 std::unique_ptr<xlogger> next_logger)
    {
        return std::make_unique<xlogger_console>(log_level, std::move(next_logger));
    }

    std::unique_ptr<xlogger> make_file_logger(xlogger::level log_level,
                                              const std::string& file_name,
                                              std::unique_ptr<xlogger> next_logger)
    {
        return std::make_unique<xlogger_file>(log_level, file_name, std::move(next_logger));
    }
}

