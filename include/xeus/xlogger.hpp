/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_LOGGER_HPP
#define XEUS_LOGGER_HPP

#include <memory>

#include "nlohmann/json.hpp"

#include "xeus.hpp"
#include "xmessage.hpp"

namespace nl = nlohmann;

namespace xeus
{

    class XEUS_API xlogger
    {
    public:

        enum channel
        {
            shell,
            control,
            stdinput,
            heartbeat,
            CHANNEL_SIZE
        };

        enum level
        {
            msg_type,
            content,
            full
        };

        virtual ~xlogger() = default;

        xlogger(const xlogger&) = delete;
        xlogger& operator=(const xlogger&) = delete;

        xlogger(xlogger&&) = delete;
        xlogger& operator=(xlogger&&) = delete;

        void log_received_message(const xmessage& message, channel c) const;
        void log_sent_message(const xmessage& message, channel c) const;
        void log_iopub_message(const xpub_message& message) const;

        void log_message(const std::string& socket_info,
                         const nl::json& header,
                         const nl::json& parent_header,
                         const nl::json& metadata,
                         const nl::json& content) const;

    protected:

        xlogger() = default;

    private:

        virtual void log_received_message_impl(const xmessage& message, channel c) const = 0;
        virtual void log_sent_message_impl(const xmessage& message, channel c) const = 0;
        virtual void log_iopub_message_impl(const xpub_message& message) const = 0;

        virtual void log_message_impl(const std::string& socket_info,
                                      const nl::json& header,
                                      const nl::json& parent_header,
                                      const nl::json& metadata,
                                      const nl::json& content) const = 0;
    };

    XEUS_API
    std::unique_ptr<xlogger> make_console_logger(xlogger::level log_level,
                                                 std::unique_ptr<xlogger> next_logger = nullptr);

    XEUS_API
    std::unique_ptr<xlogger> make_file_logger(xlogger::level log_level,
                                              const std::string& file_name,
                                              std::unique_ptr<xlogger> next_logger = nullptr);
}

#endif

