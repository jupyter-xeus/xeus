/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XINPUT_HPP
#define XINPUT_HPP

#include <string>

#include "xeus/xeus.hpp"
#include "xeus/xrequest_context.hpp"

namespace xeus
{
    XEUS_API 
    std::string blocking_input_request(
        xrequest_context context,
        const std::string& prompt,
        bool password
    );
}

#endif
