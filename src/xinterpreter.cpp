/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xinterpreter.hpp"

namespace xeus
{

    xjson xinterpreter::execute_request(const std::string& code,
                                        bool silent,
                                        bool store_history,
                                        const xjson::node_type* user_expressions,
                                        bool allow_stdin)
    {
        if (!silent)
        {
            ++m_execution_count;
            publish_execution_input(code, m_execution_count);
        }

        xjson reply = execute_request_impl(m_execution_count, code, silent,
                                           store_history, user_expressions, allow_stdin);

        reply.set_value("/execution_count", m_execution_count);
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

    void xinterpreter::register_publisher(const publisher& pub)
    {
        m_publisher = pub;
    }

    void xinterpreter::publish_stream(const std::string& name, const std::string& text)
    {
        xjson content;
        content.set_value("/name", name);
        content.set_value("/text", text);
        m_publisher("stream", xjson(), std::move(content));
    }

    void xinterpreter::display_data(xjson data, xjson metadata, xjson transient)
    {
        m_publisher("display_data", xjson(),
                    build_display_content(std::move(data),
                                          std::move(metadata), 
                                          std::move(transient)));
    }

    void xinterpreter::update_display_data(xjson data, xjson metadata, xjson transient)
    {
        m_publisher("display_data", xjson(),
                    build_display_content(std::move(data),
                                          std::move(metadata),
                                          std::move(transient)));
    }

    void xinterpreter::publish_execution_input(const std::string& code, int execution_count)
    {
        xjson content;
        content.set_value("/code", code);
        content.set_value("/execution_count", execution_count);
        m_publisher("execute_input", xjson(), std::move(content));
    }

    void xinterpreter::publish_execution_result(int execution_count, xjson data, xjson metadata)
    {
        xjson content;
        content.set_value("/execution_count", execution_count);
        content.add_subtree("data", data);
        content.add_subtree("metadata", metadata);
        m_publisher("execute_result", xjson(), std::move(content));
    }

    void xinterpreter::publish_execution_error(const std::string& ename, const std::string& evalue,
                                               const std::vector<std::string>& trace_back)
    {
        xjson content;
        content.set_value("/ename", ename);
        content.set_value("/evalue", evalue);
        content.set_value("/traceback", trace_back);
        m_publisher("error", xjson(), std::move(content));
    }

    void xinterpreter::clear_output(bool wait)
    {
        xjson content;
        content.set_value("/wait", wait);
        m_publisher("clear_output", xjson(), std::move(content));
    }

    void xinterpreter::register_stdin_sender(const stdin_sender& sender)
    {
        m_stdin = sender;
    }

    void xinterpreter::input_request(const std::string& prompt, bool pwd)
    {
        xjson content;
        content.set_value("/prompt", prompt);
        content.set_value("/pwd", pwd);
        m_stdin("input_request", xjson(), std::move(content));
    }

    void xinterpreter::input_reply(const std::string& value)
    {
        input_reply_impl(value);
    }

    xjson xinterpreter::build_display_content(xjson data, xjson metadata, xjson transient)
    {
        xjson res;
        res.add_subtree("data", data);
        res.add_subtree("metadata", metadata);
        res.add_subtree("transient", transient);
        return res;
    }
}
