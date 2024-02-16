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

#include "xeus/xainterpreter.hpp"

#include <iostream>

namespace nl = nlohmann;

namespace xeus
{
    xainterpreter::xainterpreter()
        : m_execution_count(0)
    {
    }

    void xainterpreter::configure()
    {
        configure_impl();
    }

    void xainterpreter::async_execute_request(const std::string& code,
                                           bool silent,
                                           bool store_history,
                                           nl::json user_expressions,
                                           bool allow_stdin,
                                           xaresponse_sender response_sender)
    {
        
        if (!silent)
        {
            ++m_execution_count;
        }
        
        auto execution_count = m_execution_count;

        auto post_send = std::move(response_sender.m_post_send);
        response_sender.m_post_send = [this,post_send, execution_count](nl::json parent, nl::json reply, nl::json meta) {

            std::cout<<" in xainterpreter sending reply "<<std::endl;
            reply["execution_count"] = execution_count;
            post_send(std::move(parent), std::move(reply), std::move(meta));
        };

        async_execute_request_impl(
            execution_count, code, silent,
            store_history, user_expressions, allow_stdin,
            response_sender
        );
    }

    nl::json xainterpreter::complete_request(const std::string& code, int cursor_pos)
    {
        return complete_request_impl(code, cursor_pos);
    }

    nl::json xainterpreter::inspect_request(const std::string& code, int cursor_pos, int detail_level)
    {
        return inspect_request_impl(code, cursor_pos, detail_level);
    }

    nl::json xainterpreter::is_complete_request(const std::string& code)
    {
        return is_complete_request_impl(code);
    }

    nl::json xainterpreter::kernel_info_request()
    {
        return kernel_info_request_impl();
    }

    void xainterpreter::shutdown_request()
    {
        shutdown_request_impl();
    }

    nl::json xainterpreter::internal_request(const nl::json& message)
    {
        return internal_request_impl(message);
    }

    void xainterpreter::register_publisher(const publisher_type& publisher)
    {
        m_publisher = publisher;
    }

    void xainterpreter::publish_stream(const std::string& name, const std::string& text)
    {
        if (m_publisher)
        {
            nl::json content;
            content["name"] = name;
            content["text"] = text;
            m_publisher("stream", nl::json::object(), std::move(content), buffer_sequence());
        }
    }

    void xainterpreter::display_data(nl::json data, nl::json metadata, nl::json transient)
    {
        if (m_publisher)
        {
            m_publisher(
                "display_data",
                nl::json::object(),
                build_display_content(std::move(data), std::move(metadata), std::move(transient)),
                buffer_sequence());
        }
    }

    void xainterpreter::update_display_data(nl::json data, nl::json metadata, nl::json transient)
    {
        if (m_publisher)
        {
            m_publisher(
                "update_display_data",
                nl::json::object(),
                build_display_content(std::move(data), std::move(metadata), std::move(transient)),
                buffer_sequence());
        }
    }

    void xainterpreter::publish_execution_input(const std::string& code, int execution_count)
    {
        if (m_publisher)
        {
            nl::json content;
            content["code"] = code;
            content["execution_count"] = execution_count;
            m_publisher("execute_input", nl::json::object(), std::move(content), buffer_sequence());
        }
    }

    void xainterpreter::publish_execution_result(int execution_count, nl::json data, nl::json metadata)
    {
        if (m_publisher)
        {
            nl::json content;
            content["execution_count"] = execution_count;
            content["data"] = std::move(data);
            content["metadata"] = std::move(metadata);
            m_publisher("execute_result", nl::json::object(), std::move(content), buffer_sequence());
        }
    }

    void xainterpreter::publish_execution_error(const std::string& ename,
                                               const std::string& evalue,
                                               const std::vector<std::string>& trace_back)
    {
        if (m_publisher)
        {
            nl::json content;
            content["ename"] = ename;
            content["evalue"] = evalue;
            content["traceback"] = trace_back;
            m_publisher("error", nl::json::object(), std::move(content), buffer_sequence());
        }
    }

    void xainterpreter::clear_output(bool wait)
    {
        if (m_publisher)
        {
            nl::json content;
            content["wait"] = wait;
            m_publisher("clear_output", nl::json::object(), std::move(content), buffer_sequence());
        }
    }

    void xainterpreter::register_stdin_sender(const stdin_sender_type& sender)
    {
        m_stdin = sender;
    }

    void xainterpreter::register_input_handler(const input_reply_handler_type& handler)
    {
        m_input_reply_handler = handler;
    }

    void xainterpreter::register_comm_manager(xcomm_manager* manager)
    {
        p_comm_manager = manager;
    }

    void xainterpreter::register_parent_header(const parent_header_type& parent_header)
    {
        m_parent_header = parent_header;
    }

    const nl::json& xainterpreter::parent_header() const noexcept
    {
        static const auto dummy = nl::json::object();
        if (m_parent_header)
        {
            return m_parent_header();
        }
        else
        {
            return dummy;
        }
    }

    void xainterpreter::register_control_messenger(xcontrol_messenger& messenger)
    {
        p_messenger = &messenger;
    }

    void xainterpreter::register_history_manager(const xhistory_manager& history)
    {
        p_history = &history;
    }

    const xhistory_manager& xainterpreter::get_history_manager() const noexcept
    {
        return *p_history;
    }

    xcontrol_messenger& xainterpreter::get_control_messenger()
    {
        return *p_messenger;
    }

    void xainterpreter::input_request(const std::string& prompt, bool pwd)
    {
        if (m_stdin)
        {
            nl::json content;
            content["prompt"] = prompt;
            content["pwd"] = pwd;
            m_stdin("input_request", nl::json::object(), std::move(content));
        }
    }

    void xainterpreter::input_reply(const std::string& value)
    {
        if (m_input_reply_handler)
        {
            m_input_reply_handler(value);
        }
    }

    nl::json xainterpreter::internal_request_impl(const nl::json&)
    {
        nl::json res;
        res["status"] = "error";
        res["what"] = "internal request not supported";
        return res;
    }

    nl::json xainterpreter::build_display_content(nl::json data, nl::json metadata, nl::json transient)
    {
        nl::json res;
        res["data"] = std::move(data);
        res["metadata"] = std::move(metadata);
        res["transient"] = std::move(transient);
        return res;
    }
}
