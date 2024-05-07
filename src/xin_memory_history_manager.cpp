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

#include "xeus/xjson.hpp"

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

    void xin_memory_history_manager::store_inputs_impl(int session,
                                                       int line_num,
                                                       const std::string& input,
                                                       const std::string& output)
    {
        m_history.push_back({ std::to_string(session), std::to_string(line_num), input, output });
    }

    std::array<std::string, 3> make_short_entry(const std::array<std::string, 4>& in)
    {
        std::array<std::string, 3> res = { in[0], in[1], in[2] };
        return res;
    }

    nl::json xin_memory_history_manager::get_tail_impl(int n, bool /*raw*/, bool output) const
    {
        nl::json reply;

        int count = std::min(n, static_cast<int>(m_history.size()));

        if (output)
        {
            history_type history;
            std::copy(m_history.rbegin(), 
                      std::next(m_history.rbegin(), count),
                      std::front_inserter(history));
        
            reply["history"] = history;
        }
        else
        {
            short_history_type history;
            std::transform(m_history.rbegin(),
                           std::next(m_history.rbegin(), count),
                           std::front_inserter(history),
                           make_short_entry);
            reply["history"] = history;
        }

        reply["status"] = "ok";
        return reply;
    }

    nl::json xin_memory_history_manager::get_range_impl(int /*session*/,
                                                        int start, 
                                                        int stop,
                                                        bool /*raw*/,
                                                        bool output) const
    {
        nl::json reply;

        int hist_size = static_cast<int>(m_history.size());
        if (start > stop || start > hist_size)
        {
            reply["status"] = "error";
            reply["ename"]  = "history_request_error";
            reply["evalue"] = "get_range: start is too high given stop or current history";

            return reply;
        }

        int count = std::min(stop, hist_size) - start;

        if (output)
        {
            history_type history;
            std::copy_n(std::next(m_history.cbegin(), start), count, std::back_inserter(history));
            reply["history"] = history;
        }
        else
        {
            short_history_type history;
            std::transform(std::next(m_history.cbegin(), start),
                           std::next(m_history.cbegin(), start + count),
                           std::back_inserter(history),
                           make_short_entry);
            reply["history"] = history;
        }


        reply["status"] = "ok";

        return reply;
    }

    template <class InputIt, class OutputIt, class Predicate, class Operation>
    OutputIt transform_if(InputIt first, InputIt last, OutputIt d_first,
                          Predicate pred, Operation op)
    {
        while (first != last)
        {
            if( pred(*first))
            {
                *d_first++ = op(*first);
            }
            ++first;
        }
        return d_first;
    }

    template <class H>
    void clean_history(H& history, int n, bool unique)
    {
        if (unique)
        {
            auto last = std::unique(history.begin(), history.end());
            history.erase(last, history.end());
        }

        int nb_erase = static_cast<int>(history.size()) - n;
        if (nb_erase > 0)
        {
            history.erase(history.begin(), std::next(history.begin(), nb_erase));
        }
    }

    nl::json xin_memory_history_manager::search_impl(const std::string& pattern,
                                                     bool /*raw*/,
                                                     bool output,
                                                     int n,
                                                     bool unique) const
    {
        nl::json reply;

        // Sanitize the pattern from special regex characters
        std::regex special_chars(R"([-[\]{}()+.,\^$|#\s])");
        std::string sanitized = std::regex_replace(pattern, special_chars, R"(\$&)");

        // Turn the glob pattern into regex (simple version)
        std::string regex_pattern = std::regex_replace(std::regex_replace(sanitized, std::regex("\\?"), "."), std::regex("\\*"), ".*");

        std::regex regex(regex_pattern);
        std::cmatch m;

        auto regex_lambda = [&m, &regex](const auto& item) { return std::regex_search(item[2].c_str(), m, regex); };
        if (output)
        {
            history_type history;
            std::copy_if(m_history.cbegin(), m_history.cend(), std::back_inserter(history), regex_lambda);
            clean_history(history, n, unique);
            reply["history"] = history;
        }
        else
        {
            short_history_type history;
            transform_if(m_history.cbegin(),
                         m_history.cend(), 
                         std::back_inserter(history),
                         regex_lambda,
                         make_short_entry);
            clean_history(history, n, unique);
            reply["history"] = history;
        }

        reply["status"] = "ok";

        return reply;
    }
}
