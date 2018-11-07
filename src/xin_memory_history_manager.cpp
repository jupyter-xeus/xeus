/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <stdexcept>
#include <string>

#include "xeus/xin_memory_history_manager.hpp"
#include "xeus/xjson.hpp"

namespace xeus
{
    xin_memory_history_manager::xin_memory_history_manager()
    {
    }

    xin_memory_history_manager::~xin_memory_history_manager()
    {
    }

    void xin_memory_history_manager::configure_impl()
    {
    }

    void xin_memory_history_manager::store_inputs_impl(int line_num, const std::string& input)
    {
        m_history.push_back({"0", std::to_string(line_num), input});
    }

    xjson xin_memory_history_manager::get_tail_impl(int n, bool /*raw*/, bool /*output*/) const
    {
        xjson reply;
        history_type history;

        int count = std::min(n, static_cast<int>(m_history.size()));
        std::copy_n(m_history.cbegin(), count, std::back_inserter(history));

        reply["status"] = "ok";
        reply["history"] = history;

        return reply;
    }

    xjson xin_memory_history_manager::get_range_impl(int /*session*/, int start, int stop, bool /*raw*/, bool /*output*/) const
    {
        xjson reply;
        history_type history;

        int hist_size = static_cast<int>(m_history.size());
        if (start > stop || start > hist_size)
        {
            reply["status"] = "error";
            reply["ename"] = "history_request_error";
            reply["ename"] = "get_range: start is too high given stop or current history";

            return reply;
        }

        int count = std::min(stop, hist_size) - start;
        auto it = m_history.cbegin();
        std::advance(it, start);
        std::copy_n(it, count, std::back_inserter(history));

        reply["status"] = "ok";
        reply["history"] = history;

        return reply;
    }

    xjson xin_memory_history_manager::search_impl(const std::string& /*pattern*/, bool /*raw*/, bool /*output*/, int /*n*/, bool /*unique*/) const
    {
        throw std::runtime_error("search not implemented for xin_memory_history_manager");
    }

}
