/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_IN_MEMORY_HISTORY_MANAGER_HPP
#define XEUS_IN_MEMORY_HISTORY_MANAGER_HPP

#include "xhistory_manager.hpp"

namespace xeus
{
    class xin_memory_history_manager : public xhistory_manager
    {

    public:

        using base_type = xeus::xhistory_manager;
        using history_type = typename base_type::history_type;

        xin_memory_history_manager();
        virtual ~xin_memory_history_manager();

    private:

        void configure_impl() override;
        void store_inputs_impl(int line_num, const std::string& code) override;
        history_type get_tail_impl(int n, bool raw, bool output) override;
        history_type get_range_impl(int session, int start, int stop, bool raw, bool output) override;
        history_type search_impl(const std::string& pattern, bool raw, bool output, int n, bool unique) override;

    };
}

#endif
