/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <chrono>
#include <cstddef>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>

#include "nlohmann/json.hpp"

#include "xeus/xguid.hpp"
#include "xeus/xmessage.hpp"

namespace nl = nlohmann;

namespace xeus
{
    const std::string xmessage_base::DELIMITER = "<IDS|MSG>";

    void parse_zmq_message(const zmq::message_t& msg, nl::json& json)
    {
        const char* buf = msg.data<const char>();
        json = nl::json::parse(buf, buf + msg.size());
    }

    zmq::message_t write_zmq_message(const nl::json& json, nl::json::error_handler_t error_handler)
    {
        std::string buffer = json.dump(-1, ' ', false, error_handler);
        return zmq::message_t(buffer.c_str(), buffer.size());
    }

    xmessage_base::xmessage_base(
        nl::json header, nl::json parent_header, nl::json metadata, nl::json content, buffer_sequence buffers)
        : m_header(std::move(header))
        , m_parent_header(std::move(parent_header))
        , m_metadata(std::move(metadata))
        , m_content(std::move(content))
        , m_buffers(std::move(buffers))
    {
    }

    bool xmessage_base::is_delimiter(zmq::message_t& frame) const
    {
        std::size_t frame_size = frame.size();
        if (frame_size != xmessage_base::DELIMITER.size())
        {
            return false;
        }

        std::string check(frame.data<const char>(), frame_size);
        return check == xmessage_base::DELIMITER;
    }

    void xmessage_base::deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth)
    {
        zmq::message_t signature = wire_msg.pop();
        zmq::message_t header = wire_msg.pop();
        zmq::message_t parent_header = wire_msg.pop();
        zmq::message_t metadata = wire_msg.pop();
        zmq::message_t content = wire_msg.pop();

        parse_zmq_message(header, m_header);
        parse_zmq_message(parent_header, m_parent_header);
        parse_zmq_message(metadata, m_metadata);
        parse_zmq_message(content, m_content);

        while (!wire_msg.empty())
        {
            m_buffers.push_back(wire_msg.pop());
        }

        // TODO: should we verify with buffers
        if (!auth.verify(signature, header, parent_header, metadata, content))
        {
            throw std::runtime_error("Signatures don't match");
        }
    }

    void xmessage_base::serialize(zmq::multipart_t& wire_msg,
                                  const xauthentication& auth,
                                  nl::json::error_handler_t error_handler) &&
    {
        // DELIMITER is written in the inheriting class so serialize/ deserialize
        // are symmetric

        zmq::message_t header = write_zmq_message(m_header, error_handler);
        zmq::message_t parent_header = write_zmq_message(m_parent_header, error_handler);
        zmq::message_t metadata = write_zmq_message(m_metadata, error_handler);
        zmq::message_t content = write_zmq_message(m_content, error_handler);
        zmq::message_t signature = auth.sign(header, parent_header, metadata, content);

        wire_msg.add(std::move(signature));
        wire_msg.add(std::move(header));
        wire_msg.add(std::move(parent_header));
        wire_msg.add(std::move(metadata));
        wire_msg.add(std::move(content));

        // was not const and  could only be called on rvalues.
        for (zmq::message_t& buffer : m_buffers)
        {
            wire_msg.add(std::move(buffer));
        }
    }

    const nl::json& xmessage_base::header() const
    {
        return m_header;
    }

    const nl::json& xmessage_base::parent_header() const
    {
        return m_parent_header;
    }

    const nl::json& xmessage_base::metadata() const
    {
        return m_metadata;
    }

    const nl::json& xmessage_base::content() const
    {
        return m_content;
    }

    const buffer_sequence& xmessage_base::buffers() const
    {
        return m_buffers;
    }

    xmessage::xmessage(const guid_list& zmq_id,
                       nl::json header,
                       nl::json parent_header,
                       nl::json metadata,
                       nl::json content,
                       buffer_sequence buffers)
        : xmessage_base(std::move(header),
                        std::move(parent_header),
                        std::move(metadata),
                        std::move(content),
                        std::move(buffers))
        , m_zmq_id(zmq_id)
    {
    }

    void xmessage::deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth)
    {
        zmq::message_t frame = wire_msg.pop();

        // ZMQ identites
        while (!base_type::is_delimiter(frame) && wire_msg.size() != 0)
        {
            m_zmq_id.emplace_back(frame.data<const char>(), frame.size());
            frame = wire_msg.pop();
        }

        // if wire_msg is empty, that means frame doesn't contain <IDS|MSG>
        if (wire_msg.size() == 0)
        {
            throw std::runtime_error("Delimiter not present in message");
        }

        base_type::deserialize(wire_msg, auth);
    }

    void xmessage::serialize(zmq::multipart_t& wire_msg,
                             const xauthentication& auth,
                             nl::json::error_handler_t error_handler) &&
    {
        auto app = [&wire_msg](const std::string& uid) {
            wire_msg.add(zmq::message_t(uid.begin(), uid.end()));
        };
        std::for_each(m_zmq_id.begin(), m_zmq_id.end(), app);
        wire_msg.add(zmq::message_t(DELIMITER.begin(), DELIMITER.end()));
        std::move(*this).base_type::serialize(wire_msg, auth, error_handler);
    }

    auto xmessage::identities() const -> const guid_list&
    {
        return m_zmq_id;
    }

    xpub_message::xpub_message(const std::string& topic,
                               nl::json header,
                               nl::json parent_header,
                               nl::json metadata,
                               nl::json content,
                               buffer_sequence buffers)
        : xmessage_base(std::move(header),
                        std::move(parent_header),
                        std::move(metadata),
                        std::move(content),
                        std::move(buffers))
        , m_topic(topic)
    {
    }

    void xpub_message::deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth)
    {
        zmq::message_t topic_msg = wire_msg.pop();
        m_topic = std::string(topic_msg.data<const char>(), topic_msg.size());
        wire_msg.pop();
        base_type::deserialize(wire_msg, auth);
    }

    void xpub_message::serialize(zmq::multipart_t& wire_msg,
                                 const xauthentication& auth,
                                 nl::json::error_handler_t error_handler) &&
    {
        wire_msg.add(zmq::message_t(m_topic.begin(), m_topic.end()));
        wire_msg.add(zmq::message_t(DELIMITER.begin(), DELIMITER.end()));
        std::move(*this).base_type::serialize(wire_msg, auth, error_handler);
    }

    const std::string& xpub_message::topic() const
    {
        return m_topic;
    }

    std::string iso8601_now()
    {
        std::ostringstream ss;

        // now
        auto now = std::chrono::system_clock::now();

        // down to seconds
        auto itt = std::chrono::system_clock::to_time_t(now);
        ss << std::put_time(std::gmtime(&itt), "%FT%T");

        // down to microseconds
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
        auto fractionals = micros.count() % 1000000;
        ss << "." << fractionals << "Z";

        return ss.str();
    }

    std::string get_protocol_version()
    {
        return XEUS_KERNEL_PROTOCOL_VERSION;
    }

    nl::json make_header(const std::string& msg_type,
                      const std::string& user_name,
                      const std::string& session_id)
    {
        nl::json header;
        header["msg_id"] = new_xguid();
        header["username"] = user_name;
        header["session"] = session_id;
        header["date"] = iso8601_now();
        header["msg_type"] = msg_type;
        header["version"] = get_protocol_version();
        return header;
    }
}
