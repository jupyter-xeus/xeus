/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>

#include "nlohmann/json.hpp"

#include "xkernel_core.hpp"
#include "xeus/xhistory_manager.hpp"

using namespace std::placeholders;

namespace xeus
{
    xkernel_core::xkernel_core(const std::string& kernel_id,
                               const std::string& user_name,
                               const std::string& session_id,
                               authentication_ptr auth,
                               logger_ptr logger,
                               server_ptr server,
                               interpreter_ptr interpreter,
                               history_manager_ptr history_manager,
                               debugger_ptr debugger,
                               nl::json::error_handler_t eh)
        : m_kernel_id(std::move(kernel_id))
        , m_user_name(std::move(user_name))
        , m_session_id(std::move(session_id))
        , p_auth(std::move(auth))
        , m_comm_manager(this)
        , p_logger(logger)
        , p_server(server)
        , p_interpreter(interpreter)
        , p_history_manager(history_manager)
        , p_debugger(debugger)
        , m_parent_id({guid_list(0), guid_list(0)})
        , m_parent_header({nl::json::object(), nl::json::object()})
        , m_error_handler(eh)
    {
        // Request handlers
        m_handler["execute_request"] = &xkernel_core::execute_request;
        m_handler["complete_request"] = &xkernel_core::complete_request;
        m_handler["inspect_request"] = &xkernel_core::inspect_request;
        m_handler["history_request"] = &xkernel_core::history_request;
        m_handler["is_complete_request"] = &xkernel_core::is_complete_request;
        m_handler["comm_info_request"] = &xkernel_core::comm_info_request;
        m_handler["comm_open"] = &xkernel_core::comm_open;
        m_handler["comm_close"] = &xkernel_core::comm_close;
        m_handler["comm_msg"] = &xkernel_core::comm_msg;
        m_handler["kernel_info_request"] = &xkernel_core::kernel_info_request;
        m_handler["shutdown_request"] = &xkernel_core::shutdown_request;
        m_handler["interrupt_request"] = &xkernel_core::interrupt_request;
        m_handler["debug_request"] = &xkernel_core::debug_request;

        // Server bindings
        p_server->register_shell_listener(std::bind(&xkernel_core::dispatch_shell, this, _1));
        p_server->register_control_listener(std::bind(&xkernel_core::dispatch_control, this, _1));
        p_server->register_stdin_listener(std::bind(&xkernel_core::dispatch_stdin, this, _1));
        p_server->register_internal_listener(std::bind(&xkernel_core::dispatch_internal, this, _1));

        // Interpreter bindings
        p_interpreter->register_publisher([this](const std::string& msg_type,
                                                 nl::json metadata,
                                                 nl::json content,
                                                 buffer_sequence buffers)
        {
            this->publish_message(msg_type, std::move(metadata), std::move(content), std::move(buffers),
                                  channel::SHELL);
        });
        p_interpreter->register_stdin_sender(std::bind(&xkernel_core::send_stdin, this, _1, _2, _3));
        p_interpreter->register_comm_manager(&m_comm_manager);
        p_interpreter->register_parent_header([this]() -> const nl::json& {
            return this->parent_header(channel::SHELL);
        });
    }

    xkernel_core::~xkernel_core()
    {
    }

    zmq::multipart_t xkernel_core::build_start_msg() const
    {
        zmq::multipart_t start_msg;

        std::string topic = "kernel_core." + m_kernel_id + ".status";
        nl::json content;
        content["execution_state"] = "starting";

        xpub_message msg(topic,
                         make_header("status", m_user_name, m_session_id),
                         nl::json::object(),
                         nl::json::object(),
                         std::move(content),
                         buffer_sequence());
        std::move(msg).serialize(start_msg, *p_auth, m_error_handler);

        return start_msg;
    }

    void xkernel_core::dispatch_shell(zmq::multipart_t& wire_msg)
    {
        dispatch(wire_msg, channel::SHELL);
    }

    void xkernel_core::dispatch_control(zmq::multipart_t& wire_msg)
    {
        dispatch(wire_msg, channel::CONTROL);
    }

    void xkernel_core::dispatch_stdin(zmq::multipart_t& wire_msg)
    {
        xmessage msg;
        try
        {
            msg.deserialize(wire_msg, *p_auth);
            p_logger->log_received_message(msg, xlogger::stdinput);
            const nl::json& content = msg.content();
            std::string value = content.value("value", "");
            p_interpreter->input_reply(value);
        }
        catch (std::exception& e)
        {
            std::cerr << "ERROR: could not handle stdin message" << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }
    }

    zmq::multipart_t xkernel_core::dispatch_internal(zmq::multipart_t& wire_msg)
    {
        nl::json msg = nl::json::parse(wire_msg.popstr());
        nl::json rep = p_interpreter->internal_request(msg);
        return zmq::multipart_t(rep.dump(-1, ' ', false, m_error_handler));
    }

    void xkernel_core::publish_message(const std::string& msg_type,
                                       nl::json metadata,
                                       nl::json content,
                                       buffer_sequence buffers,
                                       channel c)
    {

        zmq::multipart_t wire_msg;
        xpub_message msg(get_topic(msg_type),
                         make_header(msg_type, m_user_name, m_session_id),
                         get_parent_header(c),
                         std::move(metadata),
                         std::move(content),
                         std::move(buffers));
        p_logger->log_iopub_message(msg);
        std::move(msg).serialize(wire_msg, *p_auth, m_error_handler);
        p_server->publish(wire_msg, c);
    }

    void xkernel_core::send_stdin(const std::string& msg_type,
                                  nl::json metadata,
                                  nl::json content)
    {
        zmq::multipart_t wire_msg;
        xmessage msg(get_parent_id(channel::SHELL),
                     make_header(msg_type, m_user_name, m_session_id),
                     get_parent_header(channel::SHELL),
                     std::move(metadata),
                     std::move(content),
                     buffer_sequence());
        p_logger->log_sent_message(msg, xlogger::stdinput);
        std::move(msg).serialize(wire_msg, *p_auth, m_error_handler);
        p_server->send_stdin(wire_msg);
    }

    xcomm_manager& xkernel_core::comm_manager() & noexcept
    {
        return m_comm_manager;
    }

    const xcomm_manager& xkernel_core::comm_manager() const & noexcept
    {
        return m_comm_manager;
    }

    xcomm_manager xkernel_core::comm_manager() const && noexcept
    {
        return m_comm_manager;
    }

    const nl::json& xkernel_core::parent_header(channel c) const noexcept
    {
        return m_parent_header[std::size_t(c)];
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
            std::cerr << "ERROR: could not deserialize message" << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }

        p_logger->log_received_message(msg, c == channel::SHELL ? xlogger::shell : xlogger::control);
        const nl::json& header = msg.header();
        set_parent(msg.identities(), header, c);
        publish_status("busy", c);

        std::string msg_type = header.value("msg_type", "");
        handler_type handler = get_handler(msg_type);
        if (handler == nullptr)
        {
            std::cerr << "ERROR: received unknown message" << std::endl;
        }
        else
        {
            try
            {
                (this->*handler)(msg, c);
            }
            catch (std::exception& e)
            {
                std::cerr << "ERROR: received bad message: " << e.what() << std::endl;
                std::cerr << "Message content: " << msg.content() << std::endl;
            }
        }

        publish_status("idle", c);
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
            const nl::json& content = request.content();
            std::string code = content.value("code", "");
            bool silent = content.value("silent", false);
            bool store_history = content.value("store_history", true);
            int execution_count = content.value("execution_count", 1);
            store_history = store_history && !silent;
            nl::json user_expression = content.value("user_expressions", nl::json::object());
            bool allow_stdin = content.value("allow_stdin", true);
            bool stop_on_error = content.value("stop_on_error", false);

            nl::json metadata = get_metadata();

            nl::json reply = p_interpreter->execute_request(
                code, silent, store_history, std::move(user_expression), allow_stdin);

            std::string status = reply.value("status", "error");
            send_reply("execute_reply", std::move(metadata), std::move(reply), c);

            if (!silent && store_history)
            {
                p_history_manager->store_inputs(0, execution_count, code);
            }

            if (!silent && status == "error" && stop_on_error)
            {
                long polling_interval = 50;
                p_server->abort_queue(std::bind(&xkernel_core::abort_request, this, _1), 50);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "ERROR: during execute_request" << std::endl;
            std::cerr << e.what() << std::endl;
        }
    }

    void xkernel_core::complete_request(const xmessage& request, channel c)
    {
        const nl::json& content = request.content();
        std::string code = content.value("code", "");
        int cursor_pos = content.value("cursor_pos", -1);

        nl::json reply = p_interpreter->complete_request(code, cursor_pos);
        send_reply("complete_reply", nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::inspect_request(const xmessage& request, channel c)
    {
        const nl::json& content = request.content();
        std::string code = content.value("code", "");
        int cursor_pos = content.value("cursor_pos", -1);
        int detail_level = content.value("detail_level", 0);

        nl::json reply = p_interpreter->inspect_request(code, cursor_pos, detail_level);
        send_reply("inspect_reply", nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::history_request(const xmessage& request, channel c)
    {
        const nl::json& content = request.content();

        nl::json history = p_history_manager->process_request(content);

        send_reply("history_reply", nl::json::object(), std::move(history), c);
    }

    void xkernel_core::is_complete_request(const xmessage& request, channel c)
    {
        const nl::json& content = request.content();
        std::string code = content.value("code", "");

        nl::json reply = p_interpreter->is_complete_request(code);
        send_reply("is_complete_reply", nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::comm_info_request(const xmessage& request, channel c)
    {
        const nl::json& content = request.content();
        std::string target_name = content.is_null() ? "" : content.value("target_name", "");
        auto comms = nl::json::object();
        for (auto it = m_comm_manager.comms().cbegin(); it != m_comm_manager.comms().cend(); ++it)
        {
            const std::string& name = it->second->target().name();
            if (target_name.empty() || name == target_name)
            {
                nl::json info;
                info["target_name"] = name;
                comms[it->first] = std::move(info);
            }
        }
        nl::json reply;
        reply["comms"] = comms;
        reply["status"] = "ok";
        send_reply("comm_info_reply", nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::kernel_info_request(const xmessage& /* request */, channel c)
    {
        nl::json reply = p_interpreter->kernel_info_request();
        reply["protocol_version"] = get_protocol_version();
        send_reply("kernel_info_reply", nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::shutdown_request(const xmessage& request, channel c)
    {
        const nl::json& content = request.content();
        bool restart = content.value("restart", false);
        p_interpreter->shutdown_request();
        p_server->stop();
        nl::json reply;
        reply["restart"] = restart;
        publish_message("shutdown", nl::json::object(), nl::json(reply), buffer_sequence(), channel::CONTROL);
        send_reply("shutdown_reply", nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::interrupt_request(const xmessage&, channel c)
    {
        nl::json reply = nl::json::object();
        publish_message("interrupt", nl::json::object(), nl::json(reply), buffer_sequence(), channel::CONTROL);
        send_reply("interrupt_reply", nl::json::object(), std::move(reply), c);
        // \o/
        std::exit(EXIT_FAILURE);
    }

    void xkernel_core::debug_request(const xmessage& request, channel c)
    {
        if(p_debugger)
        {
            nl::json reply = p_debugger->process_request(request.header(), request.content());
            nl::json metadata = get_metadata();
            send_reply("debug_reply", std::move(metadata), std::move(reply), c);
        }
    }

    void xkernel_core::publish_status(const std::string& status, channel c)
    {
        nl::json content;
        content["execution_state"] = status;
        publish_message("status", nl::json::object(), std::move(content), buffer_sequence(), c);
    }

    void xkernel_core::publish_execute_input(const std::string& code,
                                             int execution_count)
    {
        nl::json content;
        content["code"] = code;
        content["execution_count"] = execution_count;
        publish_message("execute_input",
                         nl::json::object(),
                         std::move(content),
                         buffer_sequence(),
                         channel::SHELL);
    }

    void xkernel_core::send_reply(const std::string& reply_type,
                                  nl::json metadata,
                                  nl::json reply_content,
                                  channel c)
    {
        send_reply(get_parent_id(c),
                   reply_type,
                   get_parent_header(c),
                   std::move(metadata),
                   std::move(reply_content),
                   c);
    }

    void xkernel_core::send_reply(const guid_list& id_list,
                                  const std::string& reply_type,
                                  nl::json parent_header,
                                  nl::json metadata,
                                  nl::json reply_content,
                                  channel c)
    {
        zmq::multipart_t wire_msg;
        xmessage reply(id_list,
                       make_header(reply_type, m_user_name, m_session_id),
                       std::move(parent_header),
                       std::move(metadata),
                       std::move(reply_content),
                       buffer_sequence());
        p_logger->log_sent_message(reply, c == channel::SHELL ? xlogger::shell : xlogger::control);
        std::move(reply).serialize(wire_msg, *p_auth, m_error_handler);
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
        catch (std::exception& e)
        {
            std::cerr << "ERROR: during execute_request: " << e.what() << std::endl;
            return;
        }
        const nl::json& header = msg.header();
        std::string msg_type = header.value("msg_type", "");
        // replace "_request" part of message type by "_reply"
        msg_type.replace(msg_type.find_last_of('_'), 8, "_reply");
        nl::json content;
        content["status"] = "error";
        send_reply(msg.identities(),
                   msg_type,
                   nl::json(header),
                   nl::json::object(),
                   std::move(content),
                   channel::SHELL);
    }

    std::string xkernel_core::get_topic(const std::string& msg_type) const
    {
        return "kernel_core." + m_kernel_id + "." + msg_type;
    }

    nl::json xkernel_core::get_metadata() const
    {
        nl::json metadata;
        metadata["started"] = iso8601_now();
        return metadata;
    }

    void xkernel_core::set_parent(const guid_list& parent_id,
                                  const nl::json& parent_header,
                                  channel c)
    {
        auto idx = static_cast<std::size_t>(c);
        m_parent_id[idx] = parent_id;
        m_parent_header[idx] = parent_header;
    }

    const xkernel_core::guid_list& xkernel_core::get_parent_id(channel c) const
    {
        return m_parent_id[std::size_t(c)];
    }

    nl::json xkernel_core::get_parent_header(channel c) const
    {
        return m_parent_header[std::size_t(c)];
    }

    void xkernel_core::comm_open(const xmessage& request, channel)
    {
        return m_comm_manager.comm_open(request);
    }

    void xkernel_core::comm_close(const xmessage& request, channel)
    {
        return m_comm_manager.comm_close(request);
    }

    void xkernel_core::comm_msg(const xmessage& request, channel)
    {
        return m_comm_manager.comm_msg(request);
    }
}
