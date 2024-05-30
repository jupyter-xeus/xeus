/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include "xeus/xbase64.hpp"

namespace xeus
{
    TEST_SUITE("xbase64")
    {
        TEST_CASE("base64encode")
        {
            std::string input = "hello world";
            std::string expected_output = "aGVsbG8gd29ybGQ=";
            std::string encoded = base64encode(input);

            REQUIRE(encoded == expected_output);
        }

        TEST_CASE("base64decode")
        {
            std::string input = "aGVsbG8gd29ybGQ=";
            std::string expected_output = "hello world";
            std::string decoded = base64decode(input);

            REQUIRE(decoded == expected_output);
        }
    }
}