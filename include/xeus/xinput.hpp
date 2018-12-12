/***************************************************************************
 * Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
 *                                                                          *
 * Distributed under the terms of the BSD 3-Clause License.                 *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

#ifndef XINPUT_HPP
#define XINPUT_HPP

#include <string>

namespace xeus
{
    std::string blocking_input_request(const std::string& prompt, bool password);
}

#endif
