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
#include "xeus/xmessage.hpp"
#include "xeus/xlogger.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class xkernel_core
    {
    public:

        using logger_ptr = xlogger*;
        using server_ptr = xserver*;
        using interpreter_ptr = xinterpreter*;
        using history_manager_ptr = xhistory_manager*;
        using debugger_ptr = xdebugger*;
        using guid_list = xmessage::guid_list;

        xkernel_core(const std::string& kernel_id,
                     const std::string& user_name,
                     const std::string& session_id,
                     logger_ptr logger,
                     server_ptr server,
                     interpreter_ptr p_interpreter,
                     history_manager_ptr p_history_manager,
                     debugger_ptr debugger);

        ~xkernel_core();

        xpub_message build_start_msg() const;

        void dispatch_shell(xmessage msg);
        void dispatch_control(xmessage msg);
        void dispatch_stdin(xmessage msg);
        nl::json dispatch_internal(nl::json msg);
 
        void publish_message(const std::string& msg_type,
                             nl::json parent_header,
                             nl::json metadata,
                             nl::json content,
                             buffer_sequence buffers,
                             channel origin);

        void send_stdin(const std::string& msg_type, const guid_list& id_list, nl::json parent_header, nl::json metadata, nl::json content);

        xcomm_manager& comm_manager() & noexcept;
        const xcomm_manager& comm_manager() const & noexcept;
        xcomm_manager comm_manager() const && noexcept;

        const nl::json& parent_header() const noexcept;

    private:

        using handler_fptr_type = void (xkernel_core::*)(xmessage, channel);

        struct handler_type{
            handler_fptr_type fptr = nullptr;
            bool blocking = true;
        };
        

        void dispatch(xmessage msg, channel c);

        handler_type get_handler(const std::string& msg_type);

        void execute_request(xmessage request, channel c);
        void complete_request(xmessage request, channel c);
        void inspect_request(xmessage request, channel c);
        void history_request(xmessage request, channel c);
        void is_complete_request(xmessage request, channel c);
        void comm_info_request(xmessage request, channel c);
        void comm_open(xmessage request, channel c);
        void comm_close(xmessage request, channel c);
        void comm_msg(xmessage request, channel c);

        void kernel_info_request(xmessage request, channel c);
        void shutdown_request(xmessage request, channel c);
        void interrupt_request(xmessage request, channel c);
        void debug_request(xmessage request, channel c);

        void publish_status(nl::json parent_header, const std::string& status, channel c);
        void publish_execute_input(nl::json parent_header, const std::string& code, int execution_count);

        void send_reply(const guid_list& id_list,
                        const std::string& reply_type,
                        nl::json parent_header,
                        nl::json metadata,
                        nl::json reply_content,
                        channel c);

        void abort_request(xmessage msg);

        std::string get_topic(const std::string& msg_type) const;
        nl::json get_metadata() const;


        std::string m_kernel_id;
        std::string m_user_name;
        std::string m_session_id;

        std::map<std::string, handler_type> m_handler;
        xcomm_manager m_comm_manager;
        logger_ptr p_logger;
        server_ptr p_server;
        interpreter_ptr p_interpreter;
        history_manager_ptr p_history_manager;
        debugger_ptr p_debugger;
    };
}

#endif
