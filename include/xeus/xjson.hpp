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

// Remove annoying false positive warning on GCC: json.hpp:1394:23: warning: potential null pointer dereference
// See https://github.com/nlohmann/json/issues/3525
#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wnull-dereference"
#   include "nlohmann/json.hpp"
#   pragma GCC diagnostic pop
#else
#   include "nlohmann/json.hpp"
#endif

#endif

