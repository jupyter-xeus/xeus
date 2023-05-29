/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <array>
#include <mutex>
#include <string>

#include "nlohmann/json.hpp"

#include "xeus/xlogger.hpp"

namespace nl = nlohmann;

namespace xeus
{

    /*****************
     * xlogger_nolog *
     *****************/

    class xlogger_nolog : public xlogger
    {
    public:

        xlogger_nolog() = default;
        virtual ~xlogger_nolog() = default;

    private:

        void log_received_message_impl(const xmessage& message, xlogger::channel c) const override;
        void log_sent_message_impl(const xmessage& message, xlogger::channel c) const override;
        void log_iopub_message_impl(const xpub_message& message) const override;
        
        void log_message_impl(const std::string& socket_info,
                              const nl::json& header,
                              const nl::json& parent_header,
                              const nl::json& metadata,
                              const nl::json& content) const override;
    };

    /******************
     * xlogger_common *
     ******************/

    class xlogger_common : public xlogger
    {
    public:

        virtual ~xlogger_common();

    protected:

        using xlogger_ptr = std::unique_ptr<xlogger>;
        xlogger_common(xlogger::level l, xlogger_ptr next_logger = nullptr);

    private:
        
        void log_received_message_impl(const xmessage& message, xlogger::channel c) const override;
        void log_sent_message_impl(const xmessage& message, xlogger::channel c) const override;
        void log_iopub_message_impl(const xpub_message& message) const override;
        
        void log_message_impl(const std::string& socket_info,
                              const nl::json& header,
                              const nl::json& parent_header,
                              const nl::json& metadata,
                              const nl::json& content) const override;

        virtual void log_json_message(const std::string& socket_info,
                                      const nl::json& json_message) const = 0;

        xlogger_ptr p_next_logger;
        xlogger::level m_level;
    };

    /*******************
     * xlogger_console *
     *******************/

    class xlogger_console : public xlogger_common
    {
    public:

        using xlogger_ptr = xlogger_common::xlogger_ptr;

        xlogger_console(xlogger::level l, xlogger_ptr next_logger = nullptr);
        virtual ~xlogger_console() = default;

    private:

        void log_json_message(const std::string& socket_info,
                              const nl::json& json_message) const override;

        mutable std::mutex m_mutex;
    };

    /****************
     * xlogger_file *
     ****************/

    class xlogger_file : public xlogger_common
    {
    public:

        using xlogger_ptr = xlogger_common::xlogger_ptr;

        xlogger_file(xlogger::level l,
                     const std::string& file_name,
                     xlogger_ptr next_logger = nullptr);
        virtual ~xlogger_file() = default;

    private:

        void log_json_message(const std::string& socket_info,
                              const nl::json& json_message) const override;

        std::string m_file_name;
        mutable std::mutex m_mutex;
    };
}

