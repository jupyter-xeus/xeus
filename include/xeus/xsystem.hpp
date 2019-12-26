/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SYSTEM_HPP
#define XEUS_SYSTEM_HPP

#include <string>

#include "xeus.hpp"

namespace xeus
{
    XEUS_API
    std::string get_temp_directory_path();

    XEUS_API
    bool create_directory(const std::string& path);

    XEUS_API
    int get_current_pid();

    XEUS_API
    std::string get_cell_tmp_file(const std::string& prefix,
                                  int execution_count,
                                  const std::string& extension);
}

#endif

