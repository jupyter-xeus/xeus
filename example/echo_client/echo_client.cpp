/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <thread>
#include <sstream>
#include "echo_client.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xguid.hpp"
#include <mutex>

namespace echo_client
{

std::mutex cout_mutex;

    void print(std::string s)
    {
        std::lock_guard<std::mutex> guard(cout_mutex);
        std::cout << s;
    }

    std::string get_end_point(const std::string& transport,
                              const std::string& ip,
                              const std::string& port)
    {
        char sep = (transport == "tcp") ? ':' : '-';
        return transport + "://" + ip + sep + port;
    }

    xclient::xclient(const xeus::xconfiguration& config,
        const std::string& user_name,
        int nb_msg,
        zmq::context_t& context)
        : m_shell(context, zmq::socket_type::req),
          m_control(context, zmq::socket_type::req),
          m_iosub(context, zmq::socket_type::sub),
          p_authentication(xeus::make_xauthentication(config.m_signature_scheme, config.m_key)),
          p_io_authentication(xeus::make_xauthentication(config.m_signature_scheme, config.m_key)),
          m_user_name(user_name),
          m_session_id(xeus::new_xguid().to_string()),
          m_nb_msg(nb_msg)
    {
        std::string sep = get_end_point(config.m_transport, config.m_ip, config.m_shell_port);
        m_shell.connect(sep);

        std::string cep = get_end_point(config.m_transport, config.m_ip, config.m_control_port);
        m_control.connect(cep);

        std::string ioep = get_end_point(config.m_transport, config.m_ip, config.m_iopub_port);
        m_iosub.connect(ioep);
        m_iosub.setsockopt(ZMQ_SUBSCRIBE, "", 0);

        std::thread iosub(&xclient::run_io_suscriber, this);
        iosub.detach();
    }

    void xclient::send_code(const std::string& code)
    {
        xeus::xjson header = xeus::make_header("execute_request", m_user_name, m_session_id);
        xeus::xjson content;
        content.set_value("/code", code);
        content.set_value("/silent", false);
        xeus::xmessage msg(xeus::xmessage::guid_list(),
                           std::move(header),
                           xeus::xjson(),
                           xeus::xjson(),
                           std::move(content));
        zmq::multipart_t wire_msg;
        msg.serialize(wire_msg, *p_authentication);
        wire_msg.send(m_shell);
        wait_for_reply(m_shell);
    }

    void xclient::send_stop()
    {
        xeus::xjson header = xeus::make_header("shutdown_request", m_user_name, m_session_id);
        xeus::xjson content;
        content.set_value("/restart", false);
        xeus::xmessage msg(xeus::xmessage::guid_list(),
                           std::move(header),
                           xeus::xjson(),
                           xeus::xjson(),
                           std::move(content));
        zmq::multipart_t wire_msg;
        msg.serialize(wire_msg, *p_authentication);
        wire_msg.send(m_control);
        wait_for_reply(m_control);
    }

    void xclient::wait_for_reply(zmq::socket_t& socket)
    {
        zmq::multipart_t wire_msg;
        wire_msg.recv(socket);
        xeus::xmessage msg;
        msg.deserialize(wire_msg, *p_authentication);

        const xeus::xjson& header = msg.header();
        const xeus::xjson::node_type* mt_ptr = header.get_node("/msg_type");
        std::string msg_type = std::string(mt_ptr->GetString(), mt_ptr->GetStringLength());

        const xeus::xjson& content = msg.content();

        if (msg_type == "shutdown_reply")
        {
            const xeus::xjson::node_type* restart_ptr = content.get_node("/restart");
            bool restart = restart_ptr->GetBool();

            std::ostringstream oss;
            oss << "Received reply from kernel:" << std::endl;
            oss << "msg_type: " << msg_type << std::endl;
            oss << "restart: " << restart << std::endl;
            oss << std::endl;
            print(oss.str());
        }
        else
        {
            const xeus::xjson::node_type* status_ptr = content.get_node("/status");
            std::string status = std::string(status_ptr->GetString(), status_ptr->GetStringLength());

            std::ostringstream oss;
            oss << "Received reply from kernel:" << std::endl;
            oss << "msg_type: " << msg_type << std::endl;
            oss << "status: " << status << std::endl;
            oss << std::endl;
            print(oss.str());
        }
    }

    void xclient::run_io_suscriber()
    {
        int nb_rcv = 0;
        while (nb_rcv < m_nb_msg * 2)
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_iosub);

            xeus::xpub_message msg;
            msg.deserialize(wire_msg, *p_io_authentication);
            const std::string& topic = msg.topic();
            size_t topic_size = topic.size();

            if (topic.substr(topic_size - 6, topic_size) == "status")
            {
                const xeus::xjson& content = msg.content();
                const xeus::xjson::node_type* status_ptr = content.get_node("/execution_state");
                std::string status = std::string(status_ptr->GetString(), status_ptr->GetStringLength());
                std::ostringstream oss;
                oss << "Kernel published status" << std::endl;
                oss << "execution_state: " << status << std::endl;
                oss << std::endl;
                print(oss.str());
            }
            else if(topic.substr(topic_size - 13, topic_size) == "execute_input")
            {
                ++nb_rcv;
                const xeus::xjson& content = msg.content();
                const xeus::xjson::node_type* code_ptr = content.get_node("/code");
                std::string code = std::string(code_ptr->GetString(), code_ptr->GetStringLength());
                std::ostringstream oss;
                oss << "Kernel published input" << std::endl;
                oss << "code: " << code << std::endl;
                oss << std::endl;
                print(oss.str());
            }
            else
            {
                ++nb_rcv;
                const xeus::xjson& content = msg.content();
                int execution_count = content.get_int("/execution_count");
                std::ostringstream oss;
                oss << "Kernel published result" << std::endl;
                oss << "execution_count: " << execution_count << std::endl;
                oss << std::endl;
                print(oss.str());
            }
        }
    }

}
