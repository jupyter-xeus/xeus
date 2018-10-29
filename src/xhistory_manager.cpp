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
#include "xeus/xjson.hpp"

namespace xeus
{

    xhistory_manager::xhistory_manager()
    {
    }

    void xhistory_manager::configure()
    {
        configure_impl();
    }

    void xhistory_manager::store_inputs(int line_num, const std::string& input)
    {
        store_inputs_impl(line_num, input);
    }

    xjson xhistory_manager::process_request(const xjson& content) const
    {
        xjson history;

        std::string hist_access_type = content.value("hist_access_type", "tail");

        if (hist_access_type.compare("tail") == 0)
        {
            int n = content.value("n", 10);
            bool raw = content.value("raw", true);
            bool output = content.value("output", false);

            history = get_tail(n, raw, output);
        }

        if (hist_access_type.compare("search") == 0)
        {
            std::string pattern = content.value("pattern", "*");
            bool raw = content.value("raw", true);
            bool output = content.value("output", false);
            int n = content.value("n", 10);
            bool unique = content.value("unique", false);

            history = search(pattern, raw, output, n, unique);
        }

        if (hist_access_type.compare("range") == 0)
        {
            int session = content.value("session", 0);
            int start = content.value("start", 1);
            int stop = content.value("stop", 10);
            bool raw = content.value("raw", true);
            bool output = content.value("output", false);

            history = get_range(session, start, stop, raw, output);
        }

        return history;
    }

    xjson xhistory_manager::get_tail(int n, bool raw, bool output) const
    {
        return get_tail_impl(n, raw, output);
    }

    xjson xhistory_manager::get_range(int session, int start, int stop, bool raw, bool output) const
    {
        return get_range_impl(session, start, stop, raw, output);
    }

    xjson xhistory_manager::search(const std::string& pattern, bool raw, bool output, int n, bool unique) const
    {
        return search_impl(pattern, raw, output, n , unique);
    }

}
