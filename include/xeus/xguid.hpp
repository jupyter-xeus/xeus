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
    /*********************
     * xguid declaration *
     *********************/

    class XEUS_API xguid
    {
    public:

        static constexpr std::size_t GUID_SIZE = 16;
        using buffer_type = std::array<unsigned char, GUID_SIZE>;

        xguid();
        xguid(const char* buffer);
        const buffer_type& buffer() const noexcept;

        std::string to_string() const noexcept;

        bool equal(const xguid& other) const noexcept;

    private:

        buffer_type m_buffer;
    };

    bool operator==(const xguid&, const xguid&);
    bool operator!=(const xguid&, const xguid&);

    /************************
     * xguid implementation *
     ************************/

    inline xguid::xguid(const char* buffer)
    {
        std::copy(buffer, buffer + GUID_SIZE, m_buffer.begin());
    }

    inline const typename xguid::buffer_type& xguid::buffer() const noexcept
    {
        return m_buffer;
    }

    inline bool xguid::equal(const xguid& other) const noexcept
    {
        return m_buffer == other.m_buffer;
    }

    inline bool operator==(const xguid& lhs, const xguid& rhs)
    {
        return lhs.equal(rhs);
    }

    inline bool operator!=(const xguid& lhs, const xguid& rhs)
    {
        return !lhs.equal(rhs);
    }
}

#endif
