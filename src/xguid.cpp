/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xguid.hpp"
#include "xstring_utils.hpp"

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

#ifdef GUID_CFUUID
#include <CoreFoundation/CFUUID.h>
#endif

#ifdef GUID_WINDOWS
#include <objbase.h>
#endif

namespace xeus
{

    xguid::xguid(const char* buffer)
    {
        std::copy(buffer, buffer + GUID_SIZE, m_buffer.begin());
    }

    xguid::xguid(const buffer_type& buffer)
        : m_buffer(buffer)
    {
    }

    const unsigned char* xguid::buffer() const
    {
        return m_buffer.data();
    }

    std::string xguid::to_string() const
    {
        std::string res = hex_string(m_buffer);
        return res;
    }

#ifdef GUID_LIBUUID

    xguid new_xguid()
    {
        uuid_t id;
        uuid_generate(id);
        return id;
    }
#endif

#ifdef GUID_CFUUID

    xguid new_xguid()
    {
        auto id = CFUUIDCreate(NULL);
        auto bytes = CFUUIDGetUUIDBytes(id);
        CFRelease(id);

        std::array<unsigned char, 16> buffer =
        {
            bytes.byte0,
            bytes.byte1,
            bytes.byte2,
            bytes.byte3,
            bytes.byte4,
            bytes.byte5,
            bytes.byte6,
            bytes.byte7,
            bytes.byte8,
            bytes.byte9,
            bytes.byte10,
            bytes.byte11,
            bytes.byte12,
            bytes.byte13,
            bytes.byte14,
            bytes.byte15
        };
        return buffer;
    }

#endif

#ifdef GUID_WINDOWS

    xguid new_xguid()
    {
        GUID id;
        CoCreateGuid(&id);

        std::array<unsigned char, 16> buffer =
        {
            (id.Data1 >> 24) & 0xFF,
            (id.Data1 >> 16) & 0xFF,
            (id.Data1 >> 8) & 0xFF,
            (id.Data1) & 0xFF,
            (id.Data2 >> 8) & 0xFF,
            (id.Data2) & 0xFF,
            (id.Data3 >> 8) & 0xFF,
            (id.Data3) & 0xFF,
            id.Data4[0],
            id.Data4[1],
            id.Data4[2],
            id.Data4[3],
            id.Data4[4],
            id.Data4[5],
            id.Data4[6],
            id.Data4[7]
        };

        return buffer;
    }
#endif

}
