/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <cstddef>

#include "xmessage.hpp"
#include "xguid.hpp"

namespace xeus
{

    const std::string xmessage_base::DELIMITER = "<IDS|MSG>";

    void parse_zmq_message(const zmq::message_t& msg,
                           xjson& json)
    {
        const char* buf = msg.data<const char>();
        json = xjson::parse(buf, buf + msg.size());
    }

    zmq::message_t write_zmq_message(const xjson& json)
    {
        std::string buffer = json.dump();
        return zmq::message_t(buffer.c_str(), buffer.size());
    }

    xmessage_base::xmessage_base(xjson header,
                                 xjson parent_header,
                                 xjson metadata,
                                 xjson content)
        : m_header(std::move(header)),
          m_parent_header(std::move(parent_header)),
          m_metadata(std::move(metadata)),
          m_content(std::move(content))
    {
    }

    bool xmessage_base::is_delimiter(zmq::message_t& frame) const
    {
        std::size_t frame_size = frame.size();
        if (frame_size != xmessage_base::DELIMITER.size())
            return false;

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

        if (!auth.verify(signature, header, parent_header, metadata, content))
            throw std::runtime_error("Signatures don't match");

        parse_zmq_message(header, m_header);
        parse_zmq_message(parent_header, m_parent_header);
        parse_zmq_message(metadata, m_metadata);
        parse_zmq_message(content, m_content);
    }

    void xmessage_base::serialize(zmq::multipart_t& wire_msg, const xauthentication& auth) const
    {
        // DELIMITER is written in the inheriting class so serialize/ deserialize
        // are symmetric

        zmq::message_t header = write_zmq_message(m_header);
        zmq::message_t parent_header = write_zmq_message(m_parent_header);
        zmq::message_t metadata = write_zmq_message(m_metadata);
        zmq::message_t content = write_zmq_message(m_content);
        zmq::message_t signature = auth.sign(header, parent_header, metadata, content);

        wire_msg.add(std::move(signature));
        wire_msg.add(std::move(header));
        wire_msg.add(std::move(parent_header));
        wire_msg.add(std::move(metadata));
        wire_msg.add(std::move(content));
    }

    const xjson& xmessage_base::header() const
    {
        return m_header;
    }

    const xjson& xmessage_base::parent_header() const
    {
        return m_parent_header;
    }

    const xjson& xmessage_base::metadata() const
    {
        return m_metadata;
    }

    const xjson& xmessage_base::content() const
    {
        return m_content;
    }

    xmessage::xmessage(const guid_list& zmq_id,
                       xjson header,
                       xjson parent_header,
                       xjson metadata,
                       xjson content)
        : xmessage_base(std::move(header),
            std::move(parent_header),
            std::move(metadata),
            std::move(content)),
        m_zmq_id(zmq_id)
    {
    }

    void xmessage::deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth)
    {
        zmq::message_t frame = wire_msg.pop();
        // ZMQ identites
        while (!xmessage_base::is_delimiter(frame) && wire_msg.size() != 0)
        {
            m_zmq_id.emplace_back(frame.data<const char>(), frame.size());
            frame = wire_msg.pop();
        }
        // if wire_msg is empty, that means frame doesn't contain <IDS|MSG>
        if (wire_msg.size() == 0)
        {
            throw std::runtime_error("Delimiter not present in message");
        }

        xmessage_base::deserialize(wire_msg, auth);
    }

    void xmessage::serialize(zmq::multipart_t& wire_msg, const xauthentication& auth) const
    {
        auto app = [&wire_msg](const std::string& uid)
        {
            wire_msg.add(zmq::message_t(uid.begin(), uid.end()));
        };
        std::for_each(m_zmq_id.begin(), m_zmq_id.end(), app);
        wire_msg.add(zmq::message_t(DELIMITER.begin(), DELIMITER.end()));
        xmessage_base::serialize(wire_msg, auth);
    }

    auto xmessage::identities() const -> const guid_list&
    {
        return m_zmq_id;
    }

    xpub_message::xpub_message(const std::string& topic,
                               xjson header,
                               xjson parent_header,
                               xjson metadata,
                               xjson content)
        : xmessage_base(std::move(header),
                        std::move(parent_header),
                        std::move(metadata),
                        std::move(content)),
        m_topic(topic)
    {
    }

    void xpub_message::deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth)
    {
        zmq::message_t topic_msg = wire_msg.pop();
        m_topic = std::string(topic_msg.data<const char>(), topic_msg.size());
        wire_msg.pop();
        xmessage_base::deserialize(wire_msg, auth);
    }

    void xpub_message::serialize(zmq::multipart_t& wire_msg, const xauthentication& auth) const
    {
        wire_msg.add(zmq::message_t(m_topic.begin(), m_topic.end()));
        wire_msg.add(zmq::message_t(DELIMITER.begin(), DELIMITER.end()));
        xmessage_base::serialize(wire_msg, auth);
    }

    const std::string& xpub_message::topic() const
    {
        return m_topic;
    }

    std::string iso8601_now()
    {
        std::time_t now;
        std::time(&now);
        char buffer[sizeof "2016-12-02T15:10:00Z"];
        std::strftime(buffer, sizeof buffer, "%FT%TZ", std::gmtime(&now));
        return std::string(buffer, buffer + sizeof buffer);
    }

    std::string get_protocol_version()
    {
        return "5.1";
    }

    xjson make_header(const std::string& msg_type,
                      const std::string& user_name,
                      const std::string& session_id)
    {
        xjson header;
        header["msg_id"] = guid_to_hex(xguid());
        header["username"] = user_name;
        header["session"] = session_id;
        header["date"] = iso8601_now();
        header["msg_type"] = msg_type;
        header["version"] = get_protocol_version();
        return header;
    }
}
