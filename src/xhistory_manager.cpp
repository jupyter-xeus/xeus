/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <vector>

#include "xeus/xhistory_manager.hpp"

namespace xeus
{

    xhistory_manager::xhistory_manager()
    {
    }

    void xhistory_manager::configure()
    {
        configure_impl();
    }

    void xhistory_manager::store_inputs(int line_num, const std::string& code)
    {
        store_inputs_impl(line_num, code);
    }

    auto xhistory_manager::get_tail(int n, bool raw, bool output) -> history_type
    {
        return get_tail_impl(n, raw, output);
    }

    auto xhistory_manager::get_range(int session, int start, int stop, bool raw, bool output) -> history_type
    {
        return get_range_impl(session, start, stop, raw, output);
    }

    auto xhistory_manager::search(const std::string& pattern, bool raw, bool output, int n, bool unique) -> history_type
    {
        return search_impl(pattern, raw, output, n , unique);
    }

}
