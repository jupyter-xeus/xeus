/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XMESSAGE_HPP
#define XMESSAGE_HPP

#include "xauthentication.hpp"
#include "xeus.hpp"
#include "xjson.hpp"
#include "zmq_addon.hpp"
#include <vector>

namespace xeus
{

    class XEUS_API xmessage_base
    {
    public:

        xmessage_base(const xmessage_base&) = delete;
        xmessage_base& operator=(const xmessage_base&) = delete;

        const xjson& header() const;
        const xjson& parent_header() const;
        const xjson& metadata() const;
        const xjson& content() const;

    protected:

        xmessage_base() = default;
        xmessage_base(xjson header,
                      xjson parent_header,
                      xjson metadata,
                      xjson content);

        ~xmessage_base() = default;

        xmessage_base(xmessage_base&&) = default;
        xmessage_base& operator=(xmessage_base&&) = default;

        bool is_delimiter(zmq::message_t& frame) const;
        void deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth);
        void serialize(zmq::multipart_t& wire_msg, const xauthentication& auth) const;

        static const std::string DELIMITER;

    private:

        xjson m_header;
        xjson m_parent_header;
        xjson m_metadata;
        xjson m_content;
    };

    class XEUS_API xmessage : public xmessage_base
    {

    public:

        using guid_list = std::vector<std::string>;

        xmessage() = default;
        xmessage(const guid_list& zmq_id,
                 xjson header,
                 xjson parent_header,
                 xjson metadata,
                 xjson content);

        ~xmessage() = default;

        xmessage(xmessage&&) = default;
        xmessage& operator=(xmessage&&) = default;

        xmessage(const xmessage&) = delete;
        xmessage& operator=(const xmessage&) = delete;

        void deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth);
        void serialize(zmq::multipart_t& wire_msg, const xauthentication& auth) const;

        const guid_list& identities() const;

    private:

        guid_list m_zmq_id;
    };

    class XEUS_API xpub_message : public xmessage_base
    {

    public:

        xpub_message() = default;
        xpub_message(const std::string& topic,
                     xjson header,
                     xjson parent_header,
                     xjson metadata,
                     xjson content);

        ~xpub_message() = default;

        xpub_message(xpub_message&&) = default;
        xpub_message& operator=(xpub_message&&) = default;

        xpub_message(const xpub_message&) = delete;
        xpub_message& operator=(const xpub_message&) = delete;

        void deserialize(zmq::multipart_t& wire_msg, const xauthentication& auth);
        void serialize(zmq::multipart_t& wire_msg, const xauthentication& auth) const;

        const std::string& topic() const;

    private:

        std::string m_topic;
    };

    XEUS_API
    std::string iso8601_now();

    XEUS_API
    std::string get_protocol_version();

    XEUS_API
    xjson make_header(const std::string& msg_type,
                      const std::string& user_name,
                      const std::string& session_id);
}

#endif
