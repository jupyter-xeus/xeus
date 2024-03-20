/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_INTERPRETER_HPP
#define XEUS_INTERPRETER_HPP

#include <functional>
#include <string>
#include <vector>

#include "xcomm.hpp"
#include "xeus.hpp"
#include "xcontrol_messenger.hpp"
#include "xhistory_manager.hpp"
#include "xrequest_context.hpp"

namespace xeus
{
    class xinterpreter;

    XEUS_API bool register_interpreter(xinterpreter* interpreter);
    XEUS_API xinterpreter& get_interpreter();

    struct XEUS_API execute_request_config
    {
        bool silent;
        bool store_history;
        bool allow_stdin;
    };

    class XEUS_API xinterpreter
    {
    public:

        xinterpreter();
        virtual ~xinterpreter() = default;

        xinterpreter(const xinterpreter&) = delete;
        xinterpreter& operator=(const xinterpreter&) = delete;

        xinterpreter(xinterpreter&&) = delete;
        xinterpreter& operator=(xinterpreter&&) = delete;

        void configure();

        using send_reply_callback = std::function<void(nl::json)>;
        void execute_request(xrequest_context context,
                             send_reply_callback callback,
                             const std::string& code,
                             execute_request_config config,
                             nl::json user_expressions);

        nl::json complete_request(const std::string& code, int cursor_pos);

        nl::json inspect_request(const std::string& code, int cursor_pos, int detail_level);

        nl::json is_complete_request(const std::string& code);
        nl::json kernel_info_request();

        void shutdown_request();

        nl::json internal_request(const nl::json& message);

        // publish(msg_type, metadata, content)
        using publisher_type = std::function<void(xrequest_context, const std::string&, nl::json, nl::json, buffer_sequence)>;
        void register_publisher(const publisher_type& publisher);

        void publish_stream(xrequest_context, const std::string& name, const std::string& text);
        void display_data(xrequest_context, nl::json data, nl::json metadata, nl::json transient);
        void update_display_data(xrequest_context, nl::json data, nl::json metadata, nl::json transient);
        void publish_execution_input(xrequest_context, const std::string& code, int execution_count);
        void publish_execution_result(xrequest_context, int execution_count, nl::json data, nl::json metadata);
        void publish_execution_error(xrequest_context, 
                                     const std::string& ename,
                                     const std::string& evalue,
                                     const std::vector<std::string>& trace_back);
        void clear_output(xrequest_context, bool wait);

        // send_stdin(msg_type, metadata, content)
        using stdin_sender_type = std::function<void(xrequest_context, const std::string&, nl::json, nl::json)>;
        void register_stdin_sender(const stdin_sender_type& sender);
        using input_reply_handler_type = std::function<void(const std::string&)>;
        void register_input_handler(const input_reply_handler_type& handler);

        void input_request(xrequest_context context, const std::string& prompt, bool pwd);
        void input_reply(const std::string& value);

        void register_comm_manager(xcomm_manager* manager);
        xcomm_manager& comm_manager() noexcept;
        const xcomm_manager& comm_manager() const noexcept;

        void register_control_messenger(xcontrol_messenger& messenger);

        void register_history_manager(const xhistory_manager& history);
        const xhistory_manager& get_history_manager() const noexcept;

    protected:

        xcontrol_messenger& get_control_messenger();

    private:

        virtual void configure_impl() = 0;

        virtual void execute_request_impl(xrequest_context request_context,
                                          send_reply_callback cb,
                                          int execution_counter,
                                          const std::string& code,
                                          execute_request_config config,
                                          nl::json user_expressions) = 0;

        virtual nl::json complete_request_impl(const std::string& code,
                                               int cursor_pos) = 0;

        virtual nl::json inspect_request_impl(const std::string& code,
                                              int cursor_pos,
                                              int detail_level) = 0;

        virtual nl::json is_complete_request_impl(const std::string& code) = 0;

        virtual nl::json kernel_info_request_impl() = 0;

        virtual void shutdown_request_impl() = 0;

        virtual nl::json internal_request_impl(const nl::json& message);

        nl::json build_display_content(nl::json data, nl::json metadata, nl::json transient);

        publisher_type m_publisher;
        stdin_sender_type m_stdin;
        int m_execution_count;
        xcomm_manager* p_comm_manager;
        input_reply_handler_type m_input_reply_handler;
        xcontrol_messenger* p_messenger;
        const xhistory_manager* p_history;
    };

    inline xcomm_manager& xinterpreter::comm_manager() noexcept
    {
        return *p_comm_manager;
    }

    inline const xcomm_manager& xinterpreter::comm_manager() const noexcept
    {
        return *p_comm_manager;
    }
}

#endif
