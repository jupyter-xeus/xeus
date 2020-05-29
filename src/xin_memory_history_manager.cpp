/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iterator>
#include <stdexcept>
#include <string>
#include <regex>

#include "nlohmann/json.hpp"

#include "xin_memory_history_manager.hpp"

namespace nl = nlohmann;

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
        m_history.push_back({ "0", std::to_string(line_num), input });
    }

    nl::json xin_memory_history_manager::get_tail_impl(int n, bool /*raw*/, bool /*output*/) const
    {
        nl::json reply;
        history_type history;

        int count = std::min(n, static_cast<int>(m_history.size()));

        if (n > count) 
            history = m_history;
        else
            std::copy(
                m_history.rbegin(), 
                std::next(m_history.rbegin(), n),
                std::front_inserter(history));
        
        reply["status"] = "ok";
        reply["history"] = history;

        return reply;
    }

    nl::json xin_memory_history_manager::get_range_impl(
        int /*session*/, int start, int stop, bool /*raw*/, bool /*output*/) const
    {
        nl::json reply;
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

    nl::json xin_memory_history_manager::search_impl(
        const std::string& pattern, bool /*raw*/, bool /*output*/, int n, bool unique) const
    {
        nl::json reply;
        history_type history;

        // Sanitize the pattern from special regex characters
        std::regex special_chars(R"([-[\]{}()+.,\^$|#\s])");
        std::string sanitized = std::regex_replace(pattern, special_chars, R"(\$&)");

        // Turn the glob pattern into regex (simple version)
        std::string regex_pattern = std::regex_replace(std::regex_replace(sanitized, std::regex("\\?"), "."), std::regex("\\*"), ".*");

        std::regex regex(regex_pattern);
        std::cmatch m;

        std::copy_if(m_history.cbegin(), m_history.cend(), std::back_inserter(history), [&] (const std::array<std::string, 3>& item) {
            return std::regex_search(item[2].c_str(), m, regex);
        });

        if (unique)
        {
            auto last = std::unique(history.begin(), history.end());
            history.erase(last, history.end());
        }

        int nb_erase = static_cast<int>(history.size()) - n;
        if (nb_erase > 0)
        {
            auto erase_end = history.begin();
            std::advance(erase_end, nb_erase);
            history.erase(history.begin(), erase_end);
        }

        reply["status"] = "ok";
        reply["history"] = history;

        return reply;
    }
}
