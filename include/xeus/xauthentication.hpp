/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_AUTHENTICATION_HPP
#define XEUS_AUTHENTICATION_HPP

#include <memory>
#include <string>

#include "zmq.hpp"
#include "xeus.hpp"

namespace xeus
{
    class XEUS_API xauthentication
    {
    public:

        virtual ~xauthentication() = default;

        xauthentication(const xauthentication&) = delete;
        xauthentication& operator=(const xauthentication&) = delete;

        xauthentication(xauthentication&&) = delete;
        xauthentication& operator=(xauthentication&&) = delete;

        zmq::message_t sign(const zmq::message_t& header,
                            const zmq::message_t& parent_header,
                            const zmq::message_t& meta_data,
                            const zmq::message_t& content) const;

        bool verify(const zmq::message_t& signature,
                    const zmq::message_t& header,
                    const zmq::message_t& parent_header,
                    const zmq::message_t& meta_data,
                    const zmq::message_t& content) const;

    protected:

        xauthentication() = default;

    private:

        virtual zmq::message_t sign_impl(const zmq::message_t& header,
                                         const zmq::message_t& parent_header,
                                         const zmq::message_t& meta_data,
                                         const zmq::message_t& content) const = 0;

        virtual bool verify_impl(const zmq::message_t& signature,
                                 const zmq::message_t& header,
                                 const zmq::message_t& parent_header,
                                 const zmq::message_t& meta_data,
                                 const zmq::message_t& content) const = 0;
    };

    XEUS_API std::unique_ptr<xauthentication> make_xauthentication(const std::string& scheme,
                                                                   const std::string& key);
}

#endif
