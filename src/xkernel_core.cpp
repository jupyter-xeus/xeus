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
#include "xeus/xrequest_context.hpp"

using namespace std::placeholders;

namespace xeus
{
    xkernel_core::xkernel_core(const std::string& kernel_id,
                               const std::string& user_name,
                               const std::string& session_id,
                               logger_ptr logger,
                               server_ptr server,
                               interpreter_ptr interpreter,
                               history_manager_ptr history_manager,
                               debugger_ptr debugger)
        : m_kernel_id(std::move(kernel_id))
        , m_user_name(std::move(user_name))
        , m_session_id(std::move(session_id))
        , m_comm_manager(this)
        , p_logger(logger)
        , p_server(server)
        , p_interpreter(interpreter)
        , p_history_manager(history_manager)
        , p_debugger(debugger)
    {
        // Request handlers (all but execute_request are blocking)
        m_handler["execute_request"] = handler_type{&xkernel_core::execute_request, /*blocking*/ false};
        m_handler["complete_request"] = handler_type{&xkernel_core::complete_request, true};
        m_handler["inspect_request"] = handler_type{&xkernel_core::inspect_request, true};
        m_handler["history_request"] = handler_type{&xkernel_core::history_request, true};
        m_handler["is_complete_request"] = handler_type{&xkernel_core::is_complete_request, true};
        m_handler["comm_info_request"] = handler_type{&xkernel_core::comm_info_request, true};
        m_handler["comm_open"] = handler_type{&xkernel_core::comm_open, true};
        m_handler["comm_close"] = handler_type{&xkernel_core::comm_close, true};
        m_handler["comm_msg"] = handler_type{&xkernel_core::comm_msg, true};
        m_handler["kernel_info_request"] = handler_type{&xkernel_core::kernel_info_request, true};
        m_handler["shutdown_request"] = handler_type{&xkernel_core::shutdown_request, true};
        m_handler["interrupt_request"] = handler_type{&xkernel_core::interrupt_request, true};
        m_handler["debug_request"] = handler_type{&xkernel_core::debug_request, true};

        // Server bindings
        p_server->register_shell_listener(std::bind(&xkernel_core::dispatch_shell, this, _1));
        p_server->register_control_listener(std::bind(&xkernel_core::dispatch_control, this, _1));
        p_server->register_stdin_listener(std::bind(&xkernel_core::dispatch_stdin, this, _1));
        p_server->register_internal_listener(std::bind(&xkernel_core::dispatch_internal, this, _1));

        // Interpreter bindings
        p_interpreter->register_publisher([this](xrequest_context request_context,
                                                 const std::string& msg_type,
                                                 nl::json metadata,
                                                 nl::json content,
                                                 buffer_sequence buffers)
        {
            this->publish_message(msg_type, request_context.header(), std::move(metadata), std::move(content), std::move(buffers),
                                  channel::SHELL);
        });

        p_interpreter->register_stdin_sender([this](xrequest_context request_context,
                                                   const std::string& msg_type,
                                                   nl::json metadata,
                                                   nl::json content)
        {
            this->send_stdin(msg_type, request_context.id(), request_context.header(), std::move(metadata), std::move(content));
        });


        p_interpreter->register_comm_manager(&m_comm_manager);

    }

    xkernel_core::~xkernel_core()
    {
    }

    xpub_message xkernel_core::build_start_msg() const
    {
        std::string topic = "kernel_core." + m_kernel_id + ".status";
        nl::json content;
        content["execution_state"] = "starting";

        xpub_message msg(topic,
                         make_header("status", m_user_name, m_session_id),
                         nl::json::object(),
                         nl::json::object(),
                         std::move(content),
                         buffer_sequence());
        return msg;
    }

    void xkernel_core::dispatch_shell(xmessage msg)
    {
        dispatch(std::move(msg), channel::SHELL);
    }

    void xkernel_core::dispatch_control(xmessage msg)
    {
        dispatch(std::move(msg), channel::CONTROL);
    }

    void xkernel_core::dispatch_stdin(xmessage msg)
    {
        try
        {
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

    nl::json xkernel_core::dispatch_internal(nl::json msg)
    {
        nl::json rep = p_interpreter->internal_request(msg);
        return rep;
    }

    void xkernel_core::publish_message(const std::string& msg_type,
                                       nl::json parent_header,
                                       nl::json metadata,
                                       nl::json content,
                                       buffer_sequence buffers,
                                       channel c)
    {
        xpub_message msg(get_topic(msg_type),
                         make_header(msg_type, m_user_name, m_session_id),
                         std::move(parent_header),
                         std::move(metadata),
                         std::move(content),
                         std::move(buffers));
        p_logger->log_iopub_message(msg);
        p_server->publish(std::move(msg), c);
    }

    void xkernel_core::send_stdin(const std::string& msg_type,
                                  const guid_list& id_list,
                                  nl::json parent_header,
                                  nl::json metadata,
                                  nl::json content)
    {
        xmessage msg(id_list,
                     make_header(msg_type, m_user_name, m_session_id),
                     std::move(parent_header),
                     std::move(metadata),
                     std::move(content),
                     buffer_sequence());
        p_logger->log_sent_message(msg, xlogger::stdinput);
        p_server->send_stdin(std::move(msg));
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

    const nl::json& xkernel_core::parent_header() const noexcept
    {
        return p_interpreter->parent_header();
    }

    void xkernel_core::dispatch(xmessage msg, channel c)
    {
        p_logger->log_received_message(msg, c == channel::SHELL ? xlogger::shell : xlogger::control);
        // Copy because the msg is moved after, and we may need the header
        // for publishing the status.
        nl::json header = msg.header();
        publish_status(header, "busy", c);

        std::string msg_type = header.value("msg_type", "");
        handler_type handler = get_handler(msg_type);
        if (handler.fptr == nullptr)
        {
            std::cerr << "ERROR: received unknown message" << std::endl;
            std::cerr << "Message type: " << msg_type << std::endl;
        }
        else
        {
            try
            {
                (this->*(handler.fptr))(std::move(msg), c);
            }
            catch (std::exception& e)
            {
                std::cerr << "ERROR: received bad message: " << e.what() << std::endl;
                std::cerr << "Message type: " << msg_type << std::endl;
            }
        }
        // async handlers need to set the idle status themselves
        if(handler.blocking)
        {
            publish_status(header, "idle", c);
        }
    }

    auto xkernel_core::get_handler(const std::string& msg_type) -> handler_type
    {
        auto iter = m_handler.find(msg_type);
        handler_type res = (iter == m_handler.end()) ? handler_type{nullptr} : iter->second;
        return res;
    }

    void xkernel_core::execute_request(xmessage request, channel)
    {
        // xeus assumes execute_request will be executed on SHELL only
        try
        {
            const nl::json& content = request.content();
            std::string code = content.value("code", "");
            bool silent = content.value("silent", false);
            bool store_history = content.value("store_history", true);
            store_history = store_history && !silent;
            nl::json user_expression = content.value("user_expressions", nl::json::object());
            bool allow_stdin = content.value("allow_stdin", true);
            bool stop_on_error = content.value("stop_on_error", false);

            xrequest_context request_context(request.header(), request.identities());
            execute_request_config config { silent, store_history, allow_stdin };
            
            auto reply_callback = [this, request_context, config, stop_on_error, code](nl::json reply)
            {
                int execution_count = 1;
                execution_count = reply.value("execution_count", 1);
                std::string status;
                status = reply.value("status", "error");
                nl::json metadata = get_metadata();

                send_reply(
                    request_context.id(),
                    "execute_reply", 
                    request_context.header(),
                    std::move(metadata), 
                    std::move(reply), 
                    channel::SHELL
                );

                if (!config.silent && config.store_history)
                {
                    p_history_manager->store_inputs(0, execution_count, code);
                }
                if (!config.silent && status == "error" && stop_on_error)
                {
                    constexpr long polling_interval = 50;
                    p_server->abort_queue(std::bind(&xkernel_core::abort_request, this, _1), polling_interval);
                }

                // idle
                publish_status(request_context.header(), "idle", channel::SHELL);
            };

            p_interpreter->execute_request(
                std::move(request_context),
                std::move(reply_callback),
                code,
                config,
                std::move(user_expression)
            );
        }
        catch (std::exception& e)
        {
            std::cerr << "ERROR: during execute_request" << std::endl;
            std::cerr << e.what() << std::endl;
        }
    }

    void xkernel_core::complete_request(xmessage request, channel c)
    {
        const nl::json& content = request.content();
        std::string code = content.value("code", "");
        int cursor_pos = content.value("cursor_pos", -1);
        nl::json reply = p_interpreter->complete_request(code, cursor_pos);
        send_reply(request.identities(), "complete_reply", request.header(),
            nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::inspect_request(xmessage request, channel c)
    {
        const nl::json& content = request.content();
        std::string code = content.value("code", "");
        int cursor_pos = content.value("cursor_pos", -1);
        int detail_level = content.value("detail_level", 0);
        nl::json reply = p_interpreter->inspect_request(code, cursor_pos, detail_level);
        send_reply(request.identities(), "inspect_reply", request.header(), nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::history_request(xmessage request, channel c)
    {
        const nl::json& content = request.content();

        nl::json history = p_history_manager->process_request(content);

        send_reply(request.identities(), "history_reply",  request.header(), nl::json::object(), std::move(history), c);
    }

    void xkernel_core::is_complete_request(xmessage request, channel c)
    {
        const nl::json& content = request.content();
        std::string code = content.value("code", "");
        nl::json reply = p_interpreter->is_complete_request(code);
        send_reply(request.identities(), "is_complete_reply", request.header(), nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::comm_info_request(xmessage request, channel c)
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
        send_reply(request.identities(), "comm_info_reply",request.header(), nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::kernel_info_request(xmessage request, channel c)
    {
        nl::json reply = p_interpreter->kernel_info_request();
        reply["protocol_version"] = get_protocol_version();
        send_reply(request.identities(), "kernel_info_reply", request.header(), nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::shutdown_request(xmessage request, channel c)
    {
        const nl::json& content = request.content();
        bool restart = content.value("restart", false);
        p_interpreter->shutdown_request();
        p_server->stop();
        nl::json reply;
        reply["restart"] = restart;
        publish_message("shutdown", request.header(), nl::json::object(), std::move(reply), buffer_sequence(), channel::CONTROL);
        send_reply(request.identities(), "shutdown_reply", request.header(), nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::interrupt_request(xmessage request, channel c)
    {
        nl::json reply = nl::json::object();
        publish_message("interrupt", request.header(), nl::json::object(), std::move(reply), buffer_sequence(), channel::CONTROL);
        send_reply(request.identities(), "interrupt_reply", request.header(), nl::json::object(), std::move(reply), c);
    }

    void xkernel_core::debug_request(xmessage request, channel c)
    {
        if(p_debugger)
        {
            nl::json reply = p_debugger->process_request(request.header(), request.content());
            nl::json metadata = get_metadata();
            send_reply(request.identities(), "debug_reply", request.header(), std::move(metadata), std::move(reply), c);
        }
    }

    void xkernel_core::publish_status(nl::json parent_header, const std::string& status, channel c)
    {
        nl::json content;
        content["execution_state"] = status;
        publish_message("status", parent_header, nl::json::object(), std::move(content), buffer_sequence(), c);
    }

    void xkernel_core::publish_execute_input(nl::json parent_header,
                                             const std::string& code,
                                             int execution_count)
    {
        nl::json content;
        content["code"] = code;
        content["execution_count"] = execution_count;
        publish_message("execute_input",
                         std::move(parent_header),
                         nl::json::object(),
                         std::move(content),
                         buffer_sequence(),
                         channel::SHELL);
    }

    void xkernel_core::send_reply(const guid_list& id_list,
                                  const std::string& reply_type,
                                  nl::json parent_header,
                                  nl::json metadata,
                                  nl::json reply_content,
                                  channel c)
    {
        xmessage reply(id_list,
                       make_header(reply_type, m_user_name, m_session_id),
                       std::move(parent_header),
                       std::move(metadata),
                       std::move(reply_content),
                       buffer_sequence());
        p_logger->log_sent_message(reply, c == channel::SHELL ? xlogger::shell : xlogger::control);
        if (c == channel::SHELL)
        {
            p_server->send_shell(std::move(reply));
        }
        else
        {
            p_server->send_control(std::move(reply));
        }
    }

    void xkernel_core::abort_request(xmessage msg)
    {
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

    void xkernel_core::comm_open(xmessage request, channel)
    {
        m_comm_manager.comm_open(std::move(request));
    }

    void xkernel_core::comm_close(xmessage request, channel)
    {
        m_comm_manager.comm_close(std::move(request));
    }

    void xkernel_core::comm_msg(xmessage request, channel)
    {
        m_comm_manager.comm_msg(std::move(request));
    }
}
