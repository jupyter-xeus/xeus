/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <stdexcept>

#include "xeus/xin_memory_history_manager.hpp"

namespace xeus
{

    xin_memory_history_manager::xin_memory_history_manager()
    {
    }

    xin_memory_history_manager::~xin_memory_history_manager() {}

    void xin_memory_history_manager::configure_impl()
    {
    }

    void xin_memory_history_manager::store_inputs_impl(int /*line_num*/, const std::string& /*code*/)
    {
        throw std::runtime_error("xin_memory_history_manager::store_inputs_impl not implemented");
    }

    auto xin_memory_history_manager::get_tail_impl(int /*n*/, bool /*raw*/, bool /*output*/) -> history_type
    {
        throw std::runtime_error("xin_memory_history_manager::get_tail_impl not implemented");
    }

    auto xin_memory_history_manager::get_range_impl(int /*session*/, int /*start*/, int /*stop*/, bool /*raw*/, bool /*output*/) -> history_type
    {
        throw std::runtime_error("xin_memory_history_manager::get_range_impl not implemented");
    }

    auto xin_memory_history_manager::search_impl(const std::string& /*pattern*/, bool /*raw*/, bool /*output*/, int /*n*/, bool /*unique*/) -> history_type
    {
        throw std::runtime_error("xin_memory_history_manager::search_impl not implemented");
    }

}
