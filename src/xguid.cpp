/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <array>
#include <cstddef>
#include <string>
#include <sstream>
#include <iomanip>

#include "xeus/xguid.hpp"
#include "xeus/xstring_utils.hpp"

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
    xguid new_xguid()
    {
        static constexpr std::size_t GUID_SIZE = 16;
        std::array<unsigned char, GUID_SIZE> buffer;
#ifdef GUID_LIBUUID
        uuid_t id;
        uuid_generate(id);
        std::copy(id, id + GUID_SIZE, buffer.begin());
#endif

#ifdef GUID_CFUUID
        auto id = CFUUIDCreate(NULL);
        auto bytes = CFUUIDGetUUIDBytes(id);
        CFRelease(id);

        buffer =
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
#endif

#ifdef GUID_WINDOWS
        GUID id;
        CoCreateGuid(&id);

        using uchar = unsigned char;

        buffer =
        {
            uchar(id.Data1 >> 24 & 0xFF),
            uchar(id.Data1 >> 16 & 0xFF),
            uchar(id.Data1 >> 8 & 0xFF),
            uchar(id.Data1 & 0xFF),
            uchar(id.Data2 >> 8 & 0xFF),
            uchar(id.Data2 & 0xFF),
            uchar(id.Data3 >> 8 & 0xFF),
            uchar(id.Data3 & 0xFF),
            id.Data4[0],
            id.Data4[1],
            id.Data4[2],
            id.Data4[3],
            id.Data4[4],
            id.Data4[5],
            id.Data4[6],
            id.Data4[7]
        };
#endif

        return xguid(hex_string(buffer));
    }
}
