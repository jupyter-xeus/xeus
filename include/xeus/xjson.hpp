/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_JSON_HPP
#define XEUS_JSON_HPP

#include "nlohmann/json.hpp"

namespace xeus
{
    // This is deprecated and will be removed
    using xjson = nlohmann::json;
}

#endif
