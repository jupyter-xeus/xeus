/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <array>
#include <cstddef>
#include "xeus/xauthentication.hpp"
#include "xeus/make_unique.hpp"
#include "xstring_utils.hpp"
#include "cryptopp/sha.h"
#include "cryptopp/hmac.h"

namespace xeus
{

    template <class T>
    class xauthentication_impl : public xauthentication
    {

    public:

        using hmac_type = CryptoPP::HMAC<T>;
        using signature_type = std::array<byte, hmac_type::DIGESTSIZE>;

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
            return ::xeus::make_unique<xauthentication_impl<CryptoPP::SHA256>>(key);
        }
        return ::xeus::make_unique<no_xauthentication>();
    }

    template <class T>
    xauthentication_impl<T>::xauthentication_impl(const std::string& key)
    {
        m_hmac = hmac_type(reinterpret_cast<const byte*>(key.c_str()), key.size());
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
        signature_type sig;
        m_hmac.Final(sig.data());
        // Signature message must be the hexdigest, not the digest
        std::string hex_sig = hex_string(sig);
        return zmq::message_t(hex_sig.begin(), hex_sig.end());
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
        signature_type sig;
        m_hmac.Final(sig.data());
        std::string hex_sig = hex_string(sig);

        // Reduces the vulnerability to timing attacks.
        bool res = CryptoPP::VerifyBufsEqual(reinterpret_cast<const byte*>(hex_sig.c_str()),
                                             signature.data<const unsigned char>(),
                                             hex_sig.size());
        return res;
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
