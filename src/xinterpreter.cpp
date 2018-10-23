/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <utility>
#include <vector>

#include "xeus/xinterpreter.hpp"

namespace xeus
{

    xinterpreter::xinterpreter() : m_execution_count(0)
    {
    }

    void xinterpreter::configure()
    {
        configure_impl();
    }

    xjson xinterpreter::execute_request(const std::string& code,
                                        bool silent,
                                        bool store_history,
                                        const xjson_node* user_expressions,
                                        bool allow_stdin)
    {
        if (!silent)
        {
            ++m_execution_count;
            publish_execution_input(code, m_execution_count);
        }

        xjson reply = execute_request_impl(m_execution_count, code, silent,
                                           store_history, user_expressions, allow_stdin);

        reply["execution_count"] = m_execution_count;
        return reply;
    }

    xjson xinterpreter::complete_request(const std::string& code,
                                         int cursor_pos)
    {
        return complete_request_impl(code, cursor_pos);
    }

    xjson xinterpreter::inspect_request(const std::string& code,
                                        int cursor_pos,
                                        int detail_level)
    {
        return inspect_request_impl(code, cursor_pos, detail_level);
    }

    xjson xinterpreter::history_request(const xhistory_arguments& args)
    {
        return history_request_impl(args);
    }

    xjson xinterpreter::is_complete_request(const std::string& code)
    {
        return is_complete_request_impl(code);
    }

    xjson xinterpreter::kernel_info_request()
    {
        return kernel_info_request_impl();
    }

    void xinterpreter::restart_request()
    {
        restart_request_impl();
    }

    void xinterpreter::register_publisher(const publisher_type& publisher)
    {
        m_publisher = publisher;
    }

    void xinterpreter::publish_stream(const std::string& name, const std::string& text)
    {
        if (m_publisher)
        {
            xjson content;
            content["name"] = name;
            content["text"] = text;
            m_publisher("stream", xjson::object(), std::move(content), buffer_sequence());
        }
    }

    void xinterpreter::display_data(xjson data, xjson metadata, xjson transient)
    {
        if (m_publisher)
        {
            m_publisher("display_data", xjson::object(),
                        build_display_content(std::move(data),
                                              std::move(metadata),
                                              std::move(transient)),
                                              buffer_sequence());
        }
    }

    void xinterpreter::update_display_data(xjson data, xjson metadata, xjson transient)
    {
        if (m_publisher)
        {
            m_publisher("update_display_data", xjson::object(),
                        build_display_content(std::move(data),
                                              std::move(metadata),
                                              std::move(transient)),
                                              buffer_sequence());
        }
    }

    void xinterpreter::publish_execution_input(const std::string& code, int execution_count)
    {
        if (m_publisher)
        {
            xjson content;
            content["code"] = code;
            content["execution_count"] = execution_count;
            m_publisher("execute_input", xjson::object(), std::move(content), buffer_sequence());
        }
    }

    void xinterpreter::publish_execution_result(int execution_count, xjson data, xjson metadata)
    {
        if (m_publisher)
        {
            xjson content;
            content["execution_count"] = execution_count;
            content["data"] = std::move(data);
            content["metadata"] = std::move(metadata);
            m_publisher("execute_result", xjson::object(), std::move(content), buffer_sequence());
        }
    }

    void xinterpreter::publish_execution_error(const std::string& ename, const std::string& evalue,
                                               const std::vector<std::string>& trace_back)
    {
        if (m_publisher)
        {
            xjson content;
            content["ename"] = ename;
            content["evalue"] = evalue;
            content["traceback"] = trace_back;
            m_publisher("error", xjson::object(), std::move(content), buffer_sequence());
        }
    }

    void xinterpreter::clear_output(bool wait)
    {
        if (m_publisher)
        {
            xjson content;
            content["wait"] = wait;
            m_publisher("clear_output", xjson::object(), std::move(content), buffer_sequence());
        }
    }

    void xinterpreter::register_stdin_sender(const stdin_sender_type& sender)
    {
        m_stdin = sender;
    }

    void xinterpreter::register_comm_manager(xcomm_manager* manager)
    {
        p_comm_manager = manager;
    }

    void xinterpreter::input_request(const std::string& prompt, bool pwd)
    {
        if (m_stdin)
        {
            xjson content;
            content["prompt"] = prompt;
            content["pwd"] = pwd;
            m_stdin("input_request", xjson::object(), std::move(content));
        }
    }

    void xinterpreter::input_reply(const std::string& value)
    {
        input_reply_impl(value);
    }

    xjson xinterpreter::build_display_content(xjson data, xjson metadata, xjson transient)
    {
        xjson res;
        res["data"] = std::move(data);
        res["metadata"] = std::move(metadata);
        res["transient"] = std::move(transient);
        return res;
    }
}
