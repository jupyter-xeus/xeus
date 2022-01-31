#include "zmq_addon.hpp"
#include "nlohmann/json.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xzmq_serializer.hpp"
#include "xeus/xdap_tcp_client.hpp"

namespace xeus
{
    /*********************************
     * xdap_tcp_client implemenation *
     *********************************/

    xdap_tcp_configuration::xdap_tcp_configuration(dap_tcp_type adap_tcp_type,
                                                   dap_init_type adap_init_type,
                                                   const std::string& user_name,
                                                   const std::string& session_id)
        : m_dap_tcp_type(adap_tcp_type)
        , m_dap_init_type(adap_init_type)
        , m_user_name(user_name)
        , m_session_id(session_id)
    {
    }

    xdap_tcp_client::xdap_tcp_client(zmq::context_t& context,
                                     const xeus::xconfiguration& config,
                                     int socket_linger,
                                     const xdap_tcp_configuration& dap_config,
                                     const event_callback& cb)
        : m_tcp_socket(context, zmq::socket_type::stream)
        , m_socket_id()
        , m_publisher(context, zmq::socket_type::pub)
        , m_controller(context, zmq::socket_type::rep)
        , m_controller_header(context, zmq::socket_type::rep)
        , m_dap_tcp_type(dap_config.m_dap_tcp_type)
        , m_dap_init_type(dap_config.m_dap_init_type)
        , m_user_name(dap_config.m_user_name)
        , m_session_id(dap_config.m_session_id)
        , m_event_callback(cb)
        , p_auth(xeus::make_xauthentication(config.m_signature_scheme, config.m_key))
        , m_parent_header("")
        , m_request_stop(false)
    {       
        m_tcp_socket.set(zmq::sockopt::linger, socket_linger);
        m_publisher.set(zmq::sockopt::linger, socket_linger);
        m_controller.set(zmq::sockopt::linger, socket_linger);
        m_controller_header.set(zmq::sockopt::linger, socket_linger);
    }

    const std::string& xdap_tcp_client::parent_header() const
    {
        return m_parent_header;
    }

    void xdap_tcp_client::forward_event(nl::json message)
    {
        if (message["type"] == "event" && message["event"] == "stopped" && message["body"]["allThreadsStopped"])
        {
            int seq = message["seq"].get<int>() + 1;
            nl::json req = {
                {"seq", seq},
                {"type", "request"},
                {"command", "threads"}
            };
            send_dap_request(std::move(req));
            auto rep = wait_for_message([](const nl::json& msg)
            {
                return msg["command"] == "threads";
            });
            nl::json new_message = message;
            new_message["body"]["threadList"] = nl::json::array();
            for (auto& th: rep["body"]["threads"])
            {
                new_message["body"]["threadList"].push_back(th["id"]);
            }
            m_event_callback(new_message);
        }
        else
        {
            m_event_callback(message);
        }
        nl::json header = xeus::make_header("debug_event", m_user_name, m_session_id);
        nl::json parent_header = m_parent_header.empty() ? nl::json::object() : nl::json::parse(m_parent_header);
        xeus::xpub_message msg("debug_event",
                                std::move(header),
                                std::move(parent_header),
                                nl::json::object(),
                                std::move(message),
                                xeus::buffer_sequence());
        zmq::multipart_t wire_msg = xzmq_serializer::serialize_iopub(std::move(msg), *p_auth);
        //std::move(msg).serialize(wire_msg, *p_auth);
        wire_msg.send(m_publisher);
    }

    void xdap_tcp_client::send_dap_request(nl::json message)
    {
        std::string content = message.dump();
        size_t content_length = content.length();
        std::string buffer = xdap_tcp_client::HEADER
                           + std::to_string(content_length)
                           + xdap_tcp_client::SEPARATOR
                           + content;
        zmq::message_t raw_message(buffer.c_str(), buffer.length());

        m_tcp_socket.send(get_tcp_id(), zmq::send_flags::sndmore);
        m_tcp_socket.send(raw_message, zmq::send_flags::none);
    }

    nl::json xdap_tcp_client::wait_for_message(const message_condition& condition)
    {
        bool wait_cond = true;
        nl::json message;
        
        // Checks if the message is not already in the message queue
        auto iter = std::find_if(m_message_queue.cbegin(),
                                 m_message_queue.cend(),
                                 [&condition, &message](const std::string& raw_message)
                                 {
                                     message = nl::json::parse(raw_message);
                                     return condition(message);
                                 });
        if (iter != m_message_queue.cend())
        {
            wait_cond = false;
            m_message_queue.erase(iter);
        }

        // Waits for additional messages
        while (wait_cond)
        {
            handle_tcp_socket(m_stopped_queue);
            while (!m_stopped_queue.empty())
            {
                const std::string& raw_message = m_stopped_queue.front();
                nl::json tmp_message = nl::json::parse(raw_message);
                if (condition(tmp_message))
                {
                    wait_cond = false;
                    message = tmp_message;
                }
                else
                {
                    m_message_queue.push_back(raw_message);
                }
                m_stopped_queue.pop_front();
            }
        }
        return message;
    }

    void xdap_tcp_client::start_debugger(std::string tcp_end_point,
                                         std::string publisher_end_point,
                                         std::string controller_end_point,
                                         std::string controller_header_end_point)
    {
        m_publisher.connect(publisher_end_point);
        m_controller.connect(controller_end_point);
        m_controller_header.connect(controller_header_end_point);
        init_tcp_socket(tcp_end_point);

        // Tells the controller that the connection with
        // ptvsd has been established
        zmq::message_t req;
        (void)m_controller.recv(req);
        m_controller.send(zmq::message_t("ACK", 3), zmq::send_flags::none);
        
        zmq::pollitem_t items[] =
        {
            { m_controller_header, 0, ZMQ_POLLIN, 0 },
            { m_controller, 0, ZMQ_POLLIN, 0 },
            { m_tcp_socket, 0, ZMQ_POLLIN, 0 }
        };

        m_request_stop = false;
        m_wait_attach = m_dap_init_type == dap_init_type::parallel;
        while(!m_request_stop)
        {
            zmq::poll(&items[0], 3, -1);

            if(items[0].revents & ZMQ_POLLIN)
            {
                handle_header_socket();
            }

            if(items[1].revents & ZMQ_POLLIN)
            {
                handle_control_socket();
            }

            if(items[2].revents & ZMQ_POLLIN)
            {
                handle_tcp_socket(m_message_queue);
            }

            process_message_queue();
        }
        m_request_stop = false;
        
        finalize_tcp_socket(tcp_end_point);
        m_controller.disconnect(controller_end_point);
        m_controller_header.disconnect(controller_header_end_point);
        m_publisher.disconnect(publisher_end_point);
    }

    zmq::message_t xdap_tcp_client::get_tcp_id() const
    {

        return zmq::message_t(m_socket_id.data<const char>(), m_socket_id.size());
    }

    void xdap_tcp_client::init_tcp_socket(const std::string& tcp_end_point)
    {
        if (m_dap_tcp_type == dap_tcp_type::client)
        {
            m_tcp_socket.connect(tcp_end_point);
            size_t id_size = 256;
            std::string id = m_tcp_socket.get(zmq::sockopt::routing_id, id_size);
            m_socket_id = zmq::message_t(id);
        }
        else
        {
            m_tcp_socket.bind(tcp_end_point);
            (void)m_tcp_socket.recv(m_socket_id);
            zmq::message_t msg;
            (void)m_tcp_socket.recv(msg);
        }
    }

    void xdap_tcp_client::finalize_tcp_socket(const std::string& tcp_end_point)
    {
        if (m_dap_tcp_type == dap_tcp_type::client)
        {
            m_tcp_socket.disconnect(tcp_end_point);
        }
        else
        {
            m_tcp_socket.unbind(tcp_end_point);
        }
    }

    void xdap_tcp_client::handle_header_socket()
    {
        zmq::message_t message;
        (void)m_controller_header.recv(message);
        m_parent_header = std::string(message.data<const char>(), message.size());
        m_controller_header.send(zmq::message_t("ACK", 3), zmq::send_flags::none);
    }

    void xdap_tcp_client::handle_control_socket()
    {
        zmq::message_t message;
        (void)m_controller.recv(message);

        if (m_wait_attach)
        {
            std::string raw_message = std::string(message.data<const char>(), message.size());
            auto pos = raw_message.find(SEPARATOR);
            std::string to_parse = raw_message.substr(pos + SEPARATOR_LENGTH);
            nl::json json_message = nl::json::parse(to_parse);

            // Sends a ZMQ header (required for stream socket) and forwards
            // the message
            m_tcp_socket.send(get_tcp_id(), zmq::send_flags::sndmore);
            m_tcp_socket.send(message, zmq::send_flags::none);

            if (json_message["command"] == "attach")
            {
                handle_init_sequence();
                m_wait_attach = false;
            }
        }
        else
        {
            // Sends a ZMQ header (required for stream socket) and forwards
            // the message
            m_tcp_socket.send(get_tcp_id(), zmq::send_flags::sndmore);
            m_tcp_socket.send(message, zmq::send_flags::none);
        }
    }

    void xdap_tcp_client::handle_tcp_socket(queue_type& message_queue)
    {
        using size_type = std::string::size_type;
        
        std::string buffer = "";
        bool messages_received = false;
        size_type header_pos = std::string::npos;
        size_type separator_pos = std::string::npos;
        size_type msg_size = 0;
        size_type msg_pos = std::string::npos;
        size_type hint = 0;

        while(!messages_received)
        {
            while(header_pos == std::string::npos)
            {
                append_tcp_message(buffer);
                header_pos = buffer.find(HEADER, hint);
            }

            hint = header_pos + HEADER_LENGTH;
            separator_pos = buffer.find(SEPARATOR, hint);
            while(separator_pos == std::string::npos)
            {
                append_tcp_message(buffer);
                separator_pos = buffer.find(SEPARATOR, hint);
            }

            msg_size = std::stoull(buffer.substr(header_pos + HEADER_LENGTH, separator_pos));
            msg_pos = separator_pos + SEPARATOR_LENGTH;

            // The end of the buffer does not contain a full message
            while(buffer.size() - msg_pos < msg_size)
            {
                append_tcp_message(buffer);
            }

            // The end of the buffer contains a full message
            if(buffer.size() - msg_pos == msg_size)
            {
                message_queue.push_back(buffer.substr(msg_pos));
                messages_received = true;
            }
            else
            {
                // The end of the buffer contains a full message
                // and the beginning of a new one. We push the first
                // one in the queue, and loop again to get the next
                // one.
                message_queue.push_back(buffer.substr(msg_pos, msg_size));
                hint = msg_pos + msg_size;
                header_pos = buffer.find(HEADER, hint);
                separator_pos = std::string::npos;
            }
        }
    }

    void xdap_tcp_client::append_tcp_message(std::string& buffer)
    {
        // First message is a ZMQ header that we discard
        zmq::message_t header;
        (void)m_tcp_socket.recv(header);

        zmq::message_t content;
        (void)m_tcp_socket.recv(content);

        buffer += std::string(content.data<const char>(), content.size());
    }

    void xdap_tcp_client::process_message_queue()
    {
        while(!m_message_queue.empty())
        {
            const std::string& raw_message = m_message_queue.front();
            nl::json message = nl::json::parse(raw_message);
            // message is either an event or a response
            if(message["type"] == "event")
            {
                handle_event(std::move(message));
            }
            else
            {
                if(message["command"] == "disconnect")
                {
                    m_request_stop = true;
                }
                zmq::message_t reply(raw_message.c_str(), raw_message.size());
                m_controller.send(reply, zmq::send_flags::none);
            }
            m_message_queue.pop_front();
        }
    }

    void xdap_tcp_client::handle_init_sequence()
    {
        // 1] Wait for initialized event
        nl::json initialized = wait_for_message([](const nl::json& message)
        {
            return message["type"] == "event" && message["event"] == "initialized";
        });

        // 2] Sends configuration done
        nl::json configuration_done =
        {
            {"type", "request"},
            {"seq", initialized["seq"].get<int>() + 1},
            {"command", "configurationDone"},
        };
        send_dap_request(configuration_done);

        // 3] Waits for configurationDone response
        nl::json config_response = wait_for_message([](const nl::json& message)
        {
            return message["type"] == "response" && message["command"] == "configurationDone";
        });

        // 4] Waits for attach response
        nl::json attach_response = wait_for_message([](const nl::json& message)
        {
            return message["type"] == "response" && message["command"] == "attach";
        });

        // 5] Forwards initialized event and attach_response
        forward_event(std::move(initialized));
        std::string raw_response = attach_response.dump();
        zmq::message_t reply(raw_response.c_str(), raw_response.size());
        m_controller.send(reply, zmq::send_flags::none);
    }
}

