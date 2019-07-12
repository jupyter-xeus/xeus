/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <fstream>
#include <iostream>

#include "xeus/xmessage.hpp"
#include "xlogger_impl.hpp"

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
    }

    xlogger_common::xlogger_common(xlogger::level l, xlogger_ptr next_logger)
        : p_next_logger(next_logger != nullptr ? std::move(next_logger) : std::make_unique<xlogger_nolog>())
        , p_authentication(nullptr)
        , m_level(l)
    {
    }

    xlogger_common::~xlogger_common()
    {
        p_authentication = nullptr;
    }
    
    void xlogger_common::log_received_message_impl(const xmessage& message, xlogger::channel c) const
    {
        std::string socket_info = "XEUS: received message on "
                                + channel_str[c] + " - "
                                + message.identities()[0];
        xlogger::log_message(socket_info,
                             message.header(),
                             message.parent_header(),
                             message.metadata(),
                             message.content());
    }

    void xlogger_common::log_sent_message_impl(const xmessage& message, xlogger::channel c) const
    {
        std::string socket_info = "XEUS: sent message on "
                                + channel_str[c] + " - "
                                + message.identities()[0];
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
        std::string message_type = header.value("msg_type", "");
        std::string message = "msg_type: " + message_type;
        switch(m_level)
        {
        case msg_type:
            break;
        case content:
            message += '\n' + json_content.dump(4);
            break;
        case full:
        default:
            {
                nl::json j;
                j["header"] = header;
                j["parent_header"] = parent_header;
                j["metadata"] = metadata;
                j["content"] = json_content;
                message += '\n' + j.dump(4);
            }
            break;
        }

        log_message_impl(socket_info, message);
        p_next_logger->log_message(socket_info, header, parent_header, metadata, json_content);
    }

    /**********************************
     * xlogger_console implementation *
     **********************************/

    xlogger_console::xlogger_console(xlogger::level l, xlogger_ptr next_logger)
        : xlogger_common(l, std::move(next_logger))
    {
    }

    void xlogger_console::log_message_impl(const std::string& socket_info,
                                           const std::string& message) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::clog << socket_info << '\n' << message << std::endl;
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

    void xlogger_file::log_message_impl(const std::string& socket_info,
                                        const std::string& message) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::ofstream out(m_file_name, std::ios_base::app);
        out<< socket_info << '\n' << message << std::endl;
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

