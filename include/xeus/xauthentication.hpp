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

#include "xeus.hpp"

namespace xeus
{
    class XEUS_API xraw_buffer
    {
    public:

        xraw_buffer(const unsigned char* data,
                    size_t size);

        const unsigned char* data() const;
        size_t size() const;

    private:

        const unsigned char* m_data;
        size_t m_size;
    };

    class XEUS_API xauthentication
    {
    public:

        virtual ~xauthentication() = default;

        xauthentication(const xauthentication&) = delete;
        xauthentication& operator=(const xauthentication&) = delete;

        xauthentication(xauthentication&&) = delete;
        xauthentication& operator=(xauthentication&&) = delete;

        std::string sign(const xraw_buffer& header,
                         const xraw_buffer& parent_header,
                         const xraw_buffer& meta_data,
                         const xraw_buffer& content) const;

        bool verify(const xraw_buffer& signature,
                    const xraw_buffer& header,
                    const xraw_buffer& parent_header,
                    const xraw_buffer& meta_data,
                    const xraw_buffer& content) const;

    protected:

        xauthentication() = default;

    private:

        virtual std::string sign_impl(const xraw_buffer& header,
                                      const xraw_buffer& parent_header,
                                      const xraw_buffer& meta_data,
                                      const xraw_buffer& content) const = 0;

        virtual bool verify_impl(const xraw_buffer& signature,
                                 const xraw_buffer& header,
                                 const xraw_buffer& parent_header,
                                 const xraw_buffer& meta_data,
                                 const xraw_buffer& content) const = 0;
    };

    XEUS_API std::unique_ptr<xauthentication> make_xauthentication(const std::string& scheme,
                                                                   const std::string& key);
}

#endif
