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
#include <mutex>
#include <cstddef>

#include "echo_client.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xguid.hpp"

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
        zmq::context_t& context)
        : m_shell(context, zmq::socket_type::dealer),
          m_control(context, zmq::socket_type::req),
          m_iosub(context, zmq::socket_type::sub),
          p_authentication(xeus::make_xauthentication(config.m_signature_scheme, config.m_key)),
          p_io_authentication(xeus::make_xauthentication(config.m_signature_scheme, config.m_key)),
          m_user_name(user_name),
          m_session_id(guid_to_hex(xeus::xguid()))
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
        content["code"] = code;
        content["silent"] = false;
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
        content["restart"] = false;
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
        std::string msg_type = header.at("msg_type");

        const xeus::xjson& content = msg.content();

        if (msg_type == "shutdown_reply")
        {
            bool restart = content.at("restart");

            std::ostringstream oss;
            oss << "Received reply from kernel:" << std::endl;
            oss << "msg_type: " << msg_type << std::endl;
            oss << "restart: " << restart << std::endl;
            oss << std::endl;
            print(oss.str());
        }
        else
        {
            std::string status = content.at("status");

            std::ostringstream oss;
            oss << "Received reply from kernel:" << std::endl;
            oss << "msg_type: " << msg_type << std::endl;
            oss << "status: " << status << std::endl;
            oss << "message: " << content << std::endl;
            oss << std::endl;
            print(oss.str());
        }
    }

    void xclient::run_io_suscriber()
    {
        while (true)
        {
            zmq::multipart_t wire_msg;
            wire_msg.recv(m_iosub);

            xeus::xpub_message msg;
            msg.deserialize(wire_msg, *p_io_authentication);
            const std::string& topic = msg.topic();
            std::size_t topic_size = topic.size();

            if (topic.substr(topic_size - 6, topic_size) == "status")
            {
                const xeus::xjson& content = msg.content();
                std::string status = content.at("execution_state");
                std::ostringstream oss;
                oss << "Kernel published status" << std::endl;
                oss << "execution_state: " << status << std::endl;
                oss << std::endl;
                print(oss.str());
            }
            else if(topic.substr(topic_size - 13, topic_size) == "execute_input")
            {
                const xeus::xjson& content = msg.content();
                std::string code = content.at("code");
                std::ostringstream oss;
                oss << "Kernel published input" << std::endl;
                oss << "code: " << code << std::endl;
                oss << std::endl;
                print(oss.str());
            }
            else if(topic.substr(topic_size - 8, topic_size) == "shutdown")
            {
                const xeus::xjson& content = msg.content();
                std::ostringstream oss;
                oss << "Kernel shut down" << std::endl;
                oss << std::endl;
                print(oss.str());
                break;
            }
            else
            {
                const xeus::xjson& content = msg.content();
                std::ostringstream oss;
                oss << "Kernel published" << std::endl;
                oss << content << std::endl;
                oss << std::endl;
                print(oss.str());
            }
        }
    }

    void xclient::send_comm_open(const std::string& target_name)
    {
        xeus::xjson header = xeus::make_header("comm_open", m_user_name, m_session_id);
        xeus::xjson content;

        content["target_name"] = target_name;
        content["comm_id"] = guid_to_hex(xeus::xguid());
        content["data"] = xeus::xjson();

        xeus::xmessage msg(xeus::xmessage::guid_list(),
                           std::move(header),
                           xeus::xjson(),
                           xeus::xjson(),
                           std::move(content));
        zmq::multipart_t wire_msg;
        msg.serialize(wire_msg, *p_authentication);
        wire_msg.send(m_shell);
    }

    void xclient::send_comm_info()
    {
        xeus::xjson header = xeus::make_header("comm_info_request", m_user_name, m_session_id);
        xeus::xjson content;
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
}
