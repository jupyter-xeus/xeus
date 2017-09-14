/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XJSON_HPP
#define XJSON_HPP

#include "xtl/xbasic_fixed_string.hpp"

#include "nl_json.hpp"

namespace xtl
{
    template <class CT, std::size_t N, template <std::size_t> class EP, class TR>
    void to_json(::nlohmann::json& j, const xbasic_fixed_string<CT, N, EP, TR>& str)
    {
        j = str.c_str();
    }

    template <class CT, std::size_t N, template <std::size_t> class EP, class TR>
    void from_json(const ::nlohmann::json& j, xbasic_fixed_string<CT, N, EP, TR>& str)
    {
        str = j.get<std::string>();
    }
}

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
