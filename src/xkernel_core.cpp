/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xkernel_core.hpp"
#include <iostream>

using namespace std::placeholders;

namespace xeus
{

    xkernel_core::xkernel_core(const std::string& kernel_id,
                               const std::string& user_name,
                               const std::string& session_id,
                               authentication_ptr auth,
                               server_ptr server,
                               interpreter_ptr interpreter)
        : m_kernel_id(std::move(kernel_id)),
          m_user_name(std::move(user_name)),
          m_session_id(std::move(session_id)),
          m_execution_counter(0),
          p_auth(std::move(auth)),
          p_server(server),
          p_interpreter(interpreter)
    {
        // Request handlers
        m_handler["execute_request"] = &xkernel_core::execute_request;
        m_handler["complete_request"] = &xkernel_core::complete_request;
        m_handler["inspect_request"] = &xkernel_core::inspect_request;
        m_handler["history_request"] = &xkernel_core::history_request;
        m_handler["is_complete_request"] = &xkernel_core::is_complete_request;
        m_handler["comm_info_request"] = &xkernel_core::comm_info_request;
        m_handler["kernel_info_request"] = &xkernel_core::kernel_info_request;
        m_handler["shutdown_request"] = &xkernel_core::shutdown_request;

        // Server settings
        p_server->register_shell_listener(std::bind(&xkernel_core::dispatch_shell, this, _1));
        p_server->register_control_listener(std::bind(&xkernel_core::dispatch_control, this, _1));

        // TODO : this message can't be sent because
        // server is not started yet
        // Status
        publish_status("starting", xjson());
    }

    void xkernel_core::dispatch_shell(zmq::multipart_t& wire_msg)
    {
        dispatch(wire_msg, channel::SHELL);
    }

    void xkernel_core::dispatch_control(zmq::multipart_t& wire_msg)
    {
        dispatch(wire_msg, channel::CONTROL);
    }

    void xkernel_core::dispatch(zmq::multipart_t& wire_msg, channel c)
    {
        xmessage msg;
        try
        {
            msg.deserialize(wire_msg, *p_auth);
        }
        catch (std::exception& e)
        {
            std::cout << "ERROR: could not deserialize message" << std::endl;
            std::cout << e.what() << std::endl;;
            return;
        }

        const xjson& header = msg.header();
        publish_status("busy", header.copy());

        std::string msg_type = header.get_string("/msg_type", "");
        handler_type handler = get_handler(msg_type);
        if (handler == nullptr)
        {
            std::cout << "ERROR: received unknown message" << std::endl;
        }
        else
        {
            try
            {
                (this->*handler)(msg, c);
            }
            catch (std::exception&)
            {
                std::cout << "ERROR: received bad message" << std::endl;
            }
        }

        publish_status("idle", header.copy());
    }

    auto xkernel_core::get_handler(const std::string& msg_type) -> handler_type
    {
        auto iter = m_handler.find(msg_type);
        handler_type res = (iter == m_handler.end()) ? nullptr : iter->second;
        return res;
    }

    void xkernel_core::execute_request(const xmessage& request, channel c)
    {
        try
        {
            const xjson& content = request.content();
            std::string code = content.get_string("/code", "");
            bool silent = content.get_bool("/silent", false);
            bool store_history = !silent && content.get_bool("/store_history", true);
            const xjson::node_type* user_expression = content.get_node("/user_expressions");
            bool allow_stdin = content.get_bool("/allow_stdin", true);
            bool stop_on_error = content.get_bool("/stop_on_error", false);

            xjson metadata = get_metadata();

            if (!silent)
            {
                ++m_execution_counter;
                publish_execute_input(code, m_execution_counter, request.header().copy());
            }

            xjson reply = p_interpreter->execute_request(code,
                                                         silent,
                                                         store_history,
                                                         user_expression,
                                                         allow_stdin);

            std::string status = reply.get_string("/status", "error");
            reply.set_value("/execution_count", m_execution_counter);
            send_reply(request, "execute_reply", std::move(metadata), std::move(reply), c);

            if (!silent && status == "error" && stop_on_error)
            {
                long polling_interval = 50;
                p_server->abort_queue(std::bind(&xkernel_core::abort_request, this, _1), 50);
            }
        }
        catch (std::exception&)
        {
            // TODO : log received bad message
        }
    }

    void xkernel_core::complete_request(const xmessage& request, channel c)
    {
        const xjson& content = request.content();
        std::string code = content.get_string("/code", "");
        int cursor_pos = content.get_int("/cursor_pos", -1);

        xjson reply = p_interpreter->complete_request(code, cursor_pos);
        send_reply(request, "complete_request", xjson(), std::move(reply), c);
    }

    void xkernel_core::inspect_request(const xmessage& request, channel c)
    {
        const xjson& content = request.content();
        std::string code = content.get_string("/code", "");
        int cursor_pos = content.get_int("/cursor_pos", -1);
        int detail_level = content.get_int("/detail_level", 0);

        xjson reply = p_interpreter->inspect_request(code, cursor_pos, detail_level);
        send_reply(request, "inspect_reply", xjson(), std::move(reply), c);
    }

    void xkernel_core::history_request(const xmessage& request, channel c)
    {
        const xjson& content = request.content();
        xhistory_arguments args;
        args.m_hist_access_type = content.get_string("/hist_access_type", "tail");
        args.m_output = content.get_bool("/output", false);
        args.m_raw = content.get_bool("/raw", false);
        args.m_session = content.get_int("/session", 0);
        args.m_start = content.get_int("/start", 0);
        args.m_stop = content.get_int("/stop", 0);
        args.m_n = content.get_int("/n", 0);
        args.m_pattern = content.get_string("/pattern", "");
        args.m_unique = content.get_bool("/unique", false);

        xjson reply = p_interpreter->history_request(args);
        send_reply(request, "history_reply", xjson(), std::move(reply), c);
    }

    void xkernel_core::is_complete_request(const xmessage& request, channel c)
    {
        const xjson& content = request.content();
        std::string code = content.get_string("/code", "");

        xjson reply = p_interpreter->is_complete_request(code);
        send_reply(request, "is_complete_reply", xjson(), std::move(reply), c);
    }

    void xkernel_core::comm_info_request(const xmessage& request, channel c)
    {
        const xjson& content = request.content();
        std::string target_name = content.get_string("/target_name", "");

        xjson reply = p_interpreter->comm_info_request(target_name);
        send_reply(request, "comm_info_reply", xjson(), std::move(reply), c);
    }

    void xkernel_core::kernel_info_request(const xmessage& request, channel c)
    {
        xjson reply = p_interpreter->kernel_info_request();
        reply.set_value("/protocol_version", get_protocol_version());
        send_reply(request, "kernel_info_reply", xjson(), std::move(reply), c);
    }

    void xkernel_core::shutdown_request(const xmessage& request, channel c)
    {
        const xjson& content = request.content();
        bool restart = content.get_bool("/restart", false);
        p_server->stop();
        xjson reply;
        reply.set_value("/restart", restart);
        publish_shutdown(request.parent_header().copy(), reply.copy());
        send_reply(request, "shutdown_reply", xjson(), std::move(reply), c);
    }

    void xkernel_core::publish_status(const std::string& status,
                                      xjson parent)
    {
        xjson content;
        content.set_value("/execution_state", status);
        xpub_message msg(get_topic("status"),
                         make_header("status", m_user_name, m_session_id),
                         std::move(parent),
                         xjson(),
                         std::move(content));
        zmq::multipart_t wire_msg;
        msg.serialize(wire_msg, *p_auth);
        p_server->publish(wire_msg);
    }

    void xkernel_core::publish_execute_input(const std::string& code,
                                             int execution_count,
                                             xjson parent)
    {
        xjson content;
        content.set_value("/code", code);
        content.set_value("/execution_count", execution_count);
        xpub_message msg(get_topic("execute_input"),
                         make_header("execute_input", m_user_name, m_session_id),
                         std::move(parent),
                         xjson(),
                         std::move(content));
        zmq::multipart_t wire_msg;
        msg.serialize(wire_msg, *p_auth);
        p_server->publish(wire_msg);
    }

    void xkernel_core::publish_shutdown(xjson parent, xjson content)
    {
        xpub_message msg(get_topic("shutdown"),
                         make_header("shutdown", m_user_name, m_session_id),
                         std::move(parent),
                         xjson(),
                         std::move(content));
        zmq::multipart_t wire_msg;
        msg.serialize(wire_msg, *p_auth);
        p_server->publish(wire_msg);
    }

    void xkernel_core::send_reply(const xmessage& request,
                                  const std::string& reply_type,
                                  xjson metadata,
                                  xjson reply_content,
                                  channel c)
    {
        xmessage reply(request.identities(),
                       make_header(reply_type, m_user_name, m_session_id),
                       request.header().copy(),
                       std::move(metadata),
                       std::move(reply_content));
        zmq::multipart_t wire_msg;
        reply.serialize(wire_msg, *p_auth);
        if (c == channel::SHELL)
        {
            p_server->send_shell(wire_msg);
        }
        else
        {
            p_server->send_control(wire_msg);
        }
    }

    void xkernel_core::abort_request(zmq::multipart_t& wire_msg)
    {
        xmessage msg;
        try
        {
            msg.deserialize(wire_msg, *p_auth);
        }
        catch (std::exception&)
        {
            // TODO : log error
            return;
        }
        const xjson& header = msg.header();
        std::string msg_type = header.get_string("/msg_type", "");
        // replace "_request" part of message type by "_reply"
        msg_type.replace(msg_type.find_last_of('_'), 8, "_reply");
        xjson content;
        content.set_value("/status", "error");
        send_reply(msg, msg_type, xjson(), std::move(content), channel::SHELL);
    }

    std::string xkernel_core::get_topic(const std::string& status) const
    {
        return "kernel_core." + m_kernel_id + "." + status;
    }

    auto xkernel_core::get_metadata() const -> xjson
    {
        xjson metadata;
        metadata.set_value("/started", iso8601_now());
        return metadata;
    }

}
