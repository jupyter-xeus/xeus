/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_IN_MEMORY_HISTORY_MANAGER_HPP
#define XEUS_IN_MEMORY_HISTORY_MANAGER_HPP

#include <list>
#include <array>
#include <string>

#include "nlohmann/json.hpp"

#include "xeus/xeus.hpp"
#include "xeus/xhistory_manager.hpp"

namespace nl = nlohmann;

namespace xeus
{
    class xin_memory_history_manager : public xhistory_manager
    {
    public:

        using entry = std::array<std::string, 4>;
        using history_type = std::list<entry>;
        using short_entry = std::array<std::string, 3>;
        using short_history_type = std::list<short_entry>;

        xin_memory_history_manager();
        virtual ~xin_memory_history_manager();

    private:

        void configure_impl() override;
        void store_inputs_impl(int session,
                               int line_num,
                               const std::string& input,
                               const std::string& output) override;

        nl::json get_tail_impl(int n, bool raw, bool output) const override;
        nl::json get_range_impl(int session, int start, int stop, bool raw, bool output) const override;
        nl::json search_impl(const std::string& pattern, bool raw, bool output, int n, bool unique) const override;

        history_type m_history;
    };
}

#endif
