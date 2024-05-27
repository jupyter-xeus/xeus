/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include "xeus/xsystem.hpp"

namespace xeus
{
    TEST_SUITE("xsystem")
    {
        TEST_CASE("executable_path")
        {
            std::string exec_path = executable_path();
            REQUIRE(!exec_path.empty());
        }

        TEST_CASE("prefix_path")
        {
            std::string prefix = prefix_path();
            std::string exec_path = executable_path();

            REQUIRE_NE(prefix.size(), exec_path.size());
            REQUIRE(std::equal(prefix.cbegin(), prefix.cend(), exec_path.cbegin()));
            REQUIRE(exec_path.find("test_xsystem") != std::string::npos);
        }
    }
}
