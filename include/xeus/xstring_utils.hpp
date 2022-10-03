/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_STRING_UTILS_HPP
#define XEUS_STRING_UTILS_HPP

#include <cstddef>
#include <string>
#include <sstream>
#include <iomanip>

namespace xeus
{
    template <class B>
    inline std::string hex_string(const B& buffer)
    {
        std::ostringstream oss;
        oss << std::hex;
        for (std::size_t i = 0; i < buffer.size(); ++i)
        {
            oss << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]);
        }
        return oss.str();
    }
}

#endif
