/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <string>

#include "nlohmann/json.hpp"

#include "xeus/xkernel.hpp"

namespace nl = nlohmann;

namespace xeus
{
    TEST(kernel, get_username)
    {
        std::string username;
        username = get_user_name();
        EXPECT_NE(username, "unspecified user");
    }
}

