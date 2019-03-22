/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <map>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "xeus/xauthentication.hpp"
#include "xeus/make_unique.hpp"

#include "xstring_utils.hpp"

namespace xeus
{
    // Specialization of xauthentication using OpenSSL.
    class openssl_xauthentication : public xauthentication
    {
    public:

        openssl_xauthentication(const std::string& scheme,
                                const std::string& key);
        virtual ~openssl_xauthentication();

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

        const EVP_MD* m_evp;
        std::string m_key;
        HMAC_CTX* m_hmac;
    };

    // Specialization of xauthentication without any signature checking.
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

    std::unique_ptr<xauthentication> make_xauthentication(const std::string& scheme,
                                                          const std::string& key)
    {
        if (scheme == "none")
        {
            return ::xeus::make_unique<no_xauthentication>();
        }
        else
        {
            return ::xeus::make_unique<openssl_xauthentication>(scheme, key);
        }
    }

    inline const EVP_MD* asevp(const std::string& scheme)
    {
        static const std::map<std::string, const EVP_MD*(*)()> schemes = {
            {"hmac-md5", EVP_md5},
            {"hmac-sha1", EVP_sha1},
            {"hmac-mdc2", EVP_mdc2},
            {"hmac-ripemd160", EVP_ripemd160},
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
            {"hmac-blake2b512", EVP_blake2b512},
            {"hmac-blake2s256", EVP_blake2s256},
#endif
            {"hmac-sha224", EVP_sha224},
            {"hmac-sha256", EVP_sha256},
            {"hmac-sha384", EVP_sha384},
            {"hmac-sha512", EVP_sha512}
        };
        return schemes.at(scheme)();
    }

    openssl_xauthentication::openssl_xauthentication(const std::string& scheme, const std::string& key)
        : m_evp(asevp(scheme)), m_key(key)
    {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        // OpenSSL 1.0.x
        m_hmac = new HMAC_CTX();
        HMAC_CTX_init(m_hmac);
#else
        m_hmac = HMAC_CTX_new();
#endif
    }

    openssl_xauthentication::~openssl_xauthentication()
    {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        // OpenSSL 1.0.x
        HMAC_CTX_cleanup(m_hmac);
#else
        HMAC_CTX_free(m_hmac);
#endif
    }

    zmq::message_t openssl_xauthentication::sign_impl(const zmq::message_t& header,
                                                      const zmq::message_t& parent_header,
                                                      const zmq::message_t& meta_data,
                                                      const zmq::message_t& content) const
    {
        HMAC_Init_ex(m_hmac, m_key.c_str(), m_key.size(), m_evp, nullptr);

        HMAC_Update(m_hmac, header.data<const unsigned char>(), header.size());
        HMAC_Update(m_hmac, parent_header.data<const unsigned char>(), parent_header.size());
        HMAC_Update(m_hmac, meta_data.data<const unsigned char>(), meta_data.size());
        HMAC_Update(m_hmac, content.data<const unsigned char>(), content.size());

        auto sig = std::vector<unsigned char>(EVP_MD_size(m_evp));
        HMAC_Final(m_hmac, sig.data(), nullptr);

        std::string hex_sig = hex_string(sig);
        return zmq::message_t(hex_sig.begin(), hex_sig.end());
    }

    bool openssl_xauthentication::verify_impl(const zmq::message_t& signature,
                                              const zmq::message_t& header,
                                              const zmq::message_t& parent_header,
                                              const zmq::message_t& meta_data,
                                              const zmq::message_t& content) const
    {
        HMAC_Init_ex(m_hmac, m_key.c_str(), m_key.size(), m_evp, nullptr);

        HMAC_Update(m_hmac, header.data<const unsigned char>(), header.size());
        HMAC_Update(m_hmac, parent_header.data<const unsigned char>(), parent_header.size());
        HMAC_Update(m_hmac, meta_data.data<const unsigned char>(), meta_data.size());
        HMAC_Update(m_hmac, content.data<const unsigned char>(), content.size());

        auto sig = std::vector<unsigned char>(EVP_MD_size(m_evp));
        HMAC_Final(m_hmac, sig.data(), nullptr);

        std::string hex_sig = hex_string(sig);
        auto cmp = CRYPTO_memcmp(reinterpret_cast<const void*>(hex_sig.c_str()), signature.data(), hex_sig.size());
        return cmp == 0;
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
