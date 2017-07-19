/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XKERNEL_CORE_HPP
#define XKERNEL_CORE_HPP

#include <map>

#include "xserver.hpp"
#include "xinterpreter.hpp"
#include "xauthentication.hpp"
#include "xmessage.hpp"

namespace xeus
{

    class xkernel_core
    {
    public:

        using authentication_ptr = std::unique_ptr<xauthentication>;
        using server_ptr = xserver*;
        using interpreter_ptr = xinterpreter*;

        xkernel_core(const std::string& kernel_id,
                     const std::string& user_name,
                     const std::string& session_id,
                     authentication_ptr auth,
                     server_ptr server,
                     interpreter_ptr p_interpreter);

        void dispatch_shell(zmq::multipart_t& wire_msg);
        void dispatch_control(zmq::multipart_t& wire_msg);
        void dispatch_stdin(zmq::multipart_t& wire_msg);

        void publish_message(const std::string& msg_type,
                             xjson metadata,
                             xjson content);

        void send_stdin(const std::string& msg_type,
                        xjson metadata,
                        xjson content);

    private:

        enum class channel
        {
            SHELL,
            CONTROL
        };

        using handler_type = void (xkernel_core::*)(const xmessage&, xkernel_core::channel);
        using guid_list = xmessage::guid_list;

        void dispatch(zmq::multipart_t& wire_msg, channel c);

        handler_type get_handler(const std::string& msg_type);

        void execute_request(const xmessage& request, channel c);
        void complete_request(const xmessage& request, channel c);
        void inspect_request(const xmessage& request, channel c);
        void history_request(const xmessage& request, channel c);
        void is_complete_request(const xmessage& request, channel c);
        void comm_info_request(const xmessage& request, channel c);

        void kernel_info_request(const xmessage& request, channel c);
        void shutdown_request(const xmessage& request, channel c);

        void publish_status(const std::string& status);

        void publish_execute_input(const std::string& code,
                                   int execution_count);

        void send_reply(const std::string& reply_type,
                        xjson metadata,
                        xjson reply_content,
                        channel c);

        void send_reply(const guid_list& id_list,
                        const std::string& reply_type,
                        xjson parent_header,
                        xjson metadata,
                        xjson reply_content,
                        channel c);

        void abort_request(zmq::multipart_t& wire_msg);

        std::string get_topic(const std::string& msg_type) const;
        xjson get_metadata() const;

        void set_parent(const guid_list& list,
                        const xjson& parent);
        const guid_list& get_parent_id() const;
        xjson get_parent_header() const;

        std::string m_kernel_id;
        std::string m_user_name;
        std::string m_session_id;
        authentication_ptr p_auth;

        std::map<std::string, handler_type> m_handler;
        server_ptr p_server;
        interpreter_ptr p_interpreter;

        guid_list m_parent_id;
        xjson m_parent_header;
    };

}

#endif
