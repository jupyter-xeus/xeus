/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XGUID_HPP
#define XGUID_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>

#include "xeus.hpp"

namespace xeus
{
    /*********************
     * xguid declaration *
     *********************/

    /**
     * @class xguid
     * @brief Globally unique identifier.
     *
     * Instances of xguid are stack allocated and implement a value semantics.
     * Instances of xguid can be used as keys in std::map and std::unordered maps.
     * The unique identifier is computed upon creation of the object, rebinding on
     * the implementation of the operating system.
     *
     */
    class XEUS_API xguid
    {
    public:

        static constexpr std::size_t GUID_SIZE = 16;
        using buffer_type = std::array<unsigned char, GUID_SIZE>;

        xguid();

        bool equal(const xguid& other) const noexcept;
        bool lesser(const xguid& other) const noexcept;

    private:

        xguid(const char*);

        buffer_type m_buffer;

        friend XEUS_API std::string guid_to_hex(xguid uuid);
        friend XEUS_API xguid hex_to_guid(const char* hex);
    };

    bool operator==(const xguid&, const xguid&);
    bool operator!=(const xguid&, const xguid&);
    bool operator<(const xguid&, const xguid&);

    XEUS_API std::string guid_to_hex(xguid uuid);
    XEUS_API xguid hex_to_guid(const char* hex);

    /************************
     * xguid implementation *
     ************************/

    inline bool xguid::equal(const xguid& other) const noexcept
    {
        return m_buffer == other.m_buffer;
    }

    inline bool xguid::lesser(const xguid& other) const noexcept
    {
        return std::lexicographical_compare(m_buffer.begin(), m_buffer.end(), other.m_buffer.begin(), other.m_buffer.end());
    }

    inline bool operator==(const xguid& lhs, const xguid& rhs)
    {
        return lhs.equal(rhs);
    }

    inline bool operator!=(const xguid& lhs, const xguid& rhs)
    {
        return !lhs.equal(rhs);
    }

    inline bool operator<(const xguid& lhs, const xguid& rhs)
    {
        return lhs.lesser(rhs);
    }
}

#endif
