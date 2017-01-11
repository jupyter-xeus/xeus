/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XGUID_HPP
#define XGUID_HPP

#include <cstddef>
#include <array>
#include <string>
#include "xeus.hpp"

namespace xeus
{
    class XEUS_API xguid
    {

    public:

        static constexpr std::size_t GUID_SIZE = 16;
        using buffer_type = std::array<unsigned char, GUID_SIZE>;

        xguid(const char* buffer);
        xguid(const buffer_type& buffer);

        const unsigned char* buffer() const;
        static constexpr std::size_t buffer_size()
        {
            return GUID_SIZE;
        }

        std::string to_string() const;

    private:

        buffer_type m_buffer;
    };

    XEUS_API
    xguid new_xguid();

}

#endif
