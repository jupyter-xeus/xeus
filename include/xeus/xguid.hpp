/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_GUID_HPP
#define XEUS_GUID_HPP

#include "xtl/xbasic_fixed_string.hpp"

#include "xeus.hpp"

namespace xeus
{
    // xfixed_string contains 2 members:
    // - a stack-allocated buffer of N (template parameter) + 1 (null termination) char
    // - a size_t for the number of characters actually used
    // So its memory footprint is N + 9 bytes. In order to not waste memory
    // due to padding, it is good to choose N so the size of the structure is a
    // multiple of the stackframe alignment (8 or 16 depending on the architecture
    // and compilers).
    // Here we want a size of 64 bytes, which gives room for 55 characters (64 - 8 - 1).
    using xguid = xtl::xfixed_string<55>;

    XEUS_API xguid new_xguid();
}

#endif
