/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xauthentication.hpp"
#include <array>
#include "dll.h"
#include "sha.h"
#include "hmac.h"

namespace xeus
{

    template <class T>
    class xauthentication_impl : public xauthentication
    {

    public:

        using hmac_type = CryptoPP::HMAC<T>;
        using signature_type = std::array<unsigned char, hmac_type::DIGESTSIZE>;

        explicit xauthentication_impl(const std::string& key);
        virtual ~xauthentication_impl() = default;

    private:

        zmq::message_t sign_impl(const zmq::message_t& header,
                                 const zmq::message_t& parent_header,
                                 const zmq::message_t& meta_data,
                                 const zmq::message_t& content) const override;

        bool verify_impl(const zmq::message_t& signature,
                         const zmq::message_t& header,
                         const zmq::message_t& parent_header,
                         const zmq::message_t& meta_data,
                         const zmq::message_t& content) const override;

    private:

        std::string m_key;
        mutable hmac_type m_hmac;
    };

    class no_xauthentication : public xauthentication
    {

    public:

        no_xauthentication() = default;
        virtual ~no_xauthentication() = default;

    private:

        zmq::message_t sign_impl(const zmq::message_t& header,
                                 const zmq::message_t& parent_header,
                                 const zmq::message_t& meta_data,
                                 const zmq::message_t& content) const override;

        bool verify_impl(const zmq::message_t& signature,
                         const zmq::message_t& header,
                         const zmq::message_t& parent_header,
                         const zmq::message_t& meta_data,
                         const zmq::message_t& content) const override;
    };

    zmq::message_t xauthentication::sign(const zmq::message_t& header,
                                         const zmq::message_t& parent_header,
                                         const zmq::message_t& meta_data,
                                         const zmq::message_t& content) const
    {
        return sign_impl(header, parent_header, meta_data, content);
    }

    bool xauthentication::verify(const zmq::message_t& signature,
                                 const zmq::message_t& header,
                                 const zmq::message_t& parent_header,
                                 const zmq::message_t& meta_data,
                                 const zmq::message_t& content) const
    {
        return verify_impl(signature, header, parent_header, meta_data, content);
    }

    const std::string sha256_scheme = "hmac-sha256";

    std::unique_ptr<xauthentication> make_xauthentication(const std::string& scheme,
                                                          const std::string& key)
    {
        if (scheme == sha256_scheme)
        {
            return std::make_unique<xauthentication_impl<CryptoPP::SHA256>>(key);
        }
        return std::make_unique<no_xauthentication>();
    }

    template <class T>
    xauthentication_impl<T>::xauthentication_impl(const std::string& key)
        : m_key(key)
    {
        m_hmac = hmac_type(reinterpret_cast<const unsigned char*>(m_key.c_str()), m_key.size());
    }

    template <class T>
    zmq::message_t xauthentication_impl<T>::sign_impl(const zmq::message_t& header,
                                                      const zmq::message_t& parent_header,
                                                      const zmq::message_t& meta_data,
                                                      const zmq::message_t& content) const
    {
        m_hmac.Update(header.data<const unsigned char>(), header.size());
        m_hmac.Update(parent_header.data<const unsigned char>(), parent_header.size());
        m_hmac.Update(meta_data.data<const unsigned char>(), meta_data.size());
        m_hmac.Update(content.data<const unsigned char>(), content.size());
        signature_type res;
        m_hmac.Final(res.data());
        return zmq::message_t(res.begin(), res.end());
    }

    template <class T>
    bool xauthentication_impl<T>::verify_impl(const zmq::message_t& signature,
                                              const zmq::message_t& header,
                                              const zmq::message_t& parent_header,
                                              const zmq::message_t& meta_data,
                                              const zmq::message_t& content) const
    {
        m_hmac.Update(header.data<const unsigned char>(), header.size());
        m_hmac.Update(parent_header.data<const unsigned char>(), parent_header.size());
        m_hmac.Update(meta_data.data<const unsigned char>(), meta_data.size());
        m_hmac.Update(content.data<const unsigned char>(), content.size());
        return m_hmac.Verify(signature.data<const unsigned char>());
    }

    zmq::message_t no_xauthentication::sign_impl(const zmq::message_t& /*header*/,
                                                 const zmq::message_t& /*parent_header*/,
                                                 const zmq::message_t& /*meta_data*/,
                                                 const zmq::message_t& /*content*/) const
    {
        return zmq::message_t(0);
    }

    bool no_xauthentication::verify_impl(const zmq::message_t& /*signature*/,
                                         const zmq::message_t& /*header*/,
                                         const zmq::message_t& /*parent_header*/,
                                         const zmq::message_t& /*meta_data*/,
                                         const zmq::message_t& /*content*/) const
    {
        return true;
    }

}
