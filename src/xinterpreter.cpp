/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"

#include "xeus/xinterpreter.hpp"

namespace nl = nlohmann;

namespace xeus
{
    xinterpreter::xinterpreter()
        : m_execution_count(0)
    {
    }

    void xinterpreter::configure()
    {
        configure_impl();
    }

    void xinterpreter::execute_request(xrequest_context context,
                                       send_reply_callback callback,
                                       const std::string& code,
                                       execute_request_config config,
                                       nl::json user_expressions)
    {
        set_request_context(std::move(context));
        if (!config.silent)
        {
            ++m_execution_count;
            publish_execution_input(code, m_execution_count);
        }
        // copy m_execution_count in a local variable to capture it in the lambda
        auto execution_count = m_execution_count;

        auto callback_impl = [execution_count, callback = std::move(callback)](nl::json reply)
        {
            reply["execution_count"] = execution_count;
            callback(std::move(reply));
        };

        execute_request_impl(
            std::move(callback_impl),
            m_execution_count,
            code,
            std::move(config),
            user_expressions
        );
    }

    nl::json xinterpreter::complete_request(const std::string& code, int cursor_pos)
    {
        return complete_request_impl(code, cursor_pos);
    }

    nl::json xinterpreter::inspect_request(const std::string& code, int cursor_pos, int detail_level)
    {
        return inspect_request_impl(code, cursor_pos, detail_level);
    }

    nl::json xinterpreter::is_complete_request(const std::string& code)
    {
        return is_complete_request_impl(code);
    }

    nl::json xinterpreter::kernel_info_request()
    {
        return kernel_info_request_impl();
    }

    void xinterpreter::shutdown_request()
    {
        shutdown_request_impl();
    }

    nl::json xinterpreter::internal_request(const nl::json& message)
    {
        return internal_request_impl(message);
    }

    void xinterpreter::register_publisher(const publisher_type& publisher)
    {
        m_publisher = publisher;
    }

    void xinterpreter::publish_stream(const std::string& name, const std::string& text)
    {
        if (m_publisher)
        {
            nl::json content;
            content["name"] = name;
            content["text"] = text;
            m_publisher(
                get_request_context(),
                "stream",
                nl::json::object(),
                std::move(content),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::display_data(nl::json data, nl::json metadata, nl::json transient)
    {
        if (m_publisher)
        {
            m_publisher(
                get_request_context(),
                "display_data",
                nl::json::object(),
                build_display_content(std::move(data), std::move(metadata), std::move(transient)),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::update_display_data(nl::json data, nl::json metadata, nl::json transient)
    {
        if (m_publisher)
        {
            m_publisher(
                get_request_context(),
                "update_display_data",
                nl::json::object(),
                build_display_content(std::move(data), std::move(metadata), std::move(transient)),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::publish_execution_input(const std::string& code, int execution_count)
    {
        if (m_publisher)
        {
            nl::json content;
            content["code"] = code;
            content["execution_count"] = execution_count;
            m_publisher(
                get_request_context(),
                "execute_input",
                nl::json::object(),
                std::move(content),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::publish_execution_result(int execution_count, nl::json data, nl::json metadata)
    {
        if (m_publisher)
        {
            nl::json content;
            content["execution_count"] = execution_count;
            content["data"] = std::move(data);
            content["metadata"] = std::move(metadata);
            m_publisher(
                get_request_context(),
                "execute_result",
                nl::json::object(),
                std::move(content),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::publish_execution_error(const std::string& ename,
                                               const std::string& evalue,
                                               const std::vector<std::string>& trace_back)
    {
        if (m_publisher)
        {
            nl::json content;
            content["ename"] = ename;
            content["evalue"] = evalue;
            content["traceback"] = trace_back;
            m_publisher(
                get_request_context(),
                "error",
                nl::json::object(),
                std::move(content),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::clear_output(bool wait)
    {
        if (m_publisher)
        {
            nl::json content;
            content["wait"] = wait;
            m_publisher(
                get_request_context(),
                "clear_output",
                nl::json::object(),
                std::move(content),
                buffer_sequence()
            );
        }
    }

    void xinterpreter::register_stdin_sender(const stdin_sender_type& sender)
    {
        m_stdin = sender;
    }

    void xinterpreter::register_input_handler(const input_reply_handler_type& handler)
    {
        m_input_reply_handler = handler;
    }

    void xinterpreter::register_comm_manager(xcomm_manager* manager)
    {
        p_comm_manager = manager;
    }

    const nl::json& xinterpreter::parent_header() const noexcept
    {

        return get_request_context().header();
    }

    void xinterpreter::register_control_messenger(xcontrol_messenger& messenger)
    {
        p_messenger = &messenger;
    }

    void xinterpreter::register_history_manager(const xhistory_manager& history)
    {
        p_history = &history;
    }

    const xhistory_manager& xinterpreter::get_history_manager() const noexcept
    {
        return *p_history;
    }

    xcontrol_messenger& xinterpreter::get_control_messenger()
    {
        return *p_messenger;
    }

    void xinterpreter::input_request(const std::string& prompt, bool pwd)
    {
        if (m_stdin)
        {
            nl::json content;
            content["prompt"] = prompt;
            content["password"] = pwd;
            m_stdin(
                get_request_context(),
                "input_request",
                nl::json::object(),
                std::move(content)
            );
        }
    }

    void xinterpreter::input_reply(const std::string& value)
    {
        if (m_input_reply_handler)
        {
            m_input_reply_handler(value);
        }
    }

    nl::json xinterpreter::internal_request_impl( const nl::json&)
    {
        nl::json res;
        res["status"] = "error";
        res["what"] = "internal request not supported";
        return res;
    }

    nl::json xinterpreter::build_display_content(nl::json data, nl::json metadata, nl::json transient)
    {
        nl::json res;
        res["data"] = std::move(data);
        res["metadata"] = std::move(metadata);
        res["transient"] = std::move(transient);
        return res;
    }

    void xinterpreter::set_request_context(xrequest_context context)
    {
        m_request_context = std::move(context);
    }

    const xrequest_context& xinterpreter::get_request_context() const noexcept
    {
        return m_request_context;
    }
}
