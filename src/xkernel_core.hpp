/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_KERNEL_CORE_HPP
#define XEUS_KERNEL_CORE_HPP

#include <map>
#include <string>

#include "nlohmann/json.hpp"

#include "xeus/xcomm.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xhistory_manager.hpp"
#include "xeus/xdebugger.hpp"
#include "xeus/xauthentication.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xlogger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class xkernel_core
    {
    public:

        using authentication_ptr = std::unique_ptr<xauthentication>;
        using logger_ptr = xlogger*;
        using server_ptr = xserver*;
        using interpreter_ptr = xinterpreter*;
        using history_manager_ptr = xhistory_manager*;
        using debugger_ptr = xdebugger*;

        xkernel_core(const std::string& kernel_id,
                     const std::string& user_name,
                     const std::string& session_id,
                     authentication_ptr auth,
                     logger_ptr logger,
                     server_ptr server,
                     interpreter_ptr p_interpreter,
                     history_manager_ptr p_history_manager,
                     debugger_ptr debugger,
                     nl::json::error_handler_t eh);

        ~xkernel_core();

        zmq::multipart_t build_start_msg() const;

        void dispatch_shell(zmq::multipart_t& wire_msg);
        void dispatch_control(zmq::multipart_t& wire_msg);
        void dispatch_stdin(zmq::multipart_t& wire_msg);
        zmq::multipart_t dispatch_internal(zmq::multipart_t& wire_msg);

        void publish_message(const std::string& msg_type,
                             nl::json metadata,
                             nl::json content,
                             buffer_sequence buffers,
                             channel origin);

        void send_stdin(const std::string& msg_type, nl::json metadata, nl::json content);

        xcomm_manager& comm_manager() & noexcept;
        const xcomm_manager& comm_manager() const & noexcept;
        xcomm_manager comm_manager() const && noexcept;
        const nl::json& parent_header(channel c) const noexcept;

    private:

        using handler_type = void (xkernel_core::*)(const xmessage&, channel);
        using guid_list = xmessage::guid_list;

        void dispatch(zmq::multipart_t& wire_msg, channel c);

        handler_type get_handler(const std::string& msg_type);

        void execute_request(const xmessage& request, channel c);
        void complete_request(const xmessage& request, channel c);
        void inspect_request(const xmessage& request, channel c);
        void history_request(const xmessage& request, channel c);
        void is_complete_request(const xmessage& request, channel c);
        void comm_info_request(const xmessage& request, channel c);
        void comm_open(const xmessage& request, channel c);
        void comm_close(const xmessage& request, channel c);
        void comm_msg(const xmessage& request, channel c);

        void kernel_info_request(const xmessage& request, channel c);
        void shutdown_request(const xmessage& request, channel c);
        void interrupt_request(const xmessage& request, channel c);
        void debug_request(const xmessage& request, channel c);

        void publish_status(const std::string& status, channel c);

        void publish_execute_input(const std::string& code, int execution_count);

        void send_reply(const std::string& reply_type,
                        nl::json metadata,
                        nl::json reply_content,
                        channel c);

        void send_reply(const guid_list& id_list,
                        const std::string& reply_type,
                        nl::json parent_header,
                        nl::json metadata,
                        nl::json reply_content,
                        channel c);

        void abort_request(zmq::multipart_t& wire_msg);

        std::string get_topic(const std::string& msg_type) const;
        nl::json get_metadata() const;

        void set_parent(const guid_list& list, const nl::json& parent, channel c);
        const guid_list& get_parent_id(channel c) const;
        nl::json get_parent_header(channel c) const;

        std::string m_kernel_id;
        std::string m_user_name;
        std::string m_session_id;
        authentication_ptr p_auth;

        std::map<std::string, handler_type> m_handler;
        xcomm_manager m_comm_manager;
        logger_ptr p_logger;
        server_ptr p_server;
        interpreter_ptr p_interpreter;
        history_manager_ptr p_history_manager;
        debugger_ptr p_debugger;

        std::array<guid_list, 2> m_parent_id;
        std::array<nl::json, 2> m_parent_header;

        nl::json::error_handler_t m_error_handler;
    };
}

#endif
