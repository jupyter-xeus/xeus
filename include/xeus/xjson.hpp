/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XJSON_HPP
#define XJSON_HPP

#include "nl_json.hpp"

namespace xeus
{
    using xjson = nlohmann::json;
    using xjson_node = xjson::object_t;

    inline const xjson_node* get_json_node(const xjson& document, const xjson_node::key_type& key)
    {
        const auto it = document.find(key);
        return it != document.end() ? it.value().get_ptr<const xjson_node*>() : nullptr;
    }
}

#endif
