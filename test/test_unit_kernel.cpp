/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include <string>
#include <iostream>

#include "nlohmann/json.hpp"

#include "xeus/xkernel.hpp"
#include "xeus/xmiddleware.hpp"
#include "xeus/xsystem.hpp"

namespace nl = nlohmann;

namespace xeus
{
    TEST_SUITE("kernel") {

    TEST_CASE("get_username")
    {
        std::string username;
        username = get_user_name();
        REQUIRE_NE(username, "unspecified user");
    }

    TEST_CASE("find_free_port")
    {
        std::string port = find_free_port();
        REQUIRE_NE(port, "");
        REQUIRE_EQ(port.length(), std::size_t(5));
    }

    TEST_CASE("temp_directory_path")
    {
        std::string path = get_temp_directory_path();
        std::cout << "Temporary directory path: " << path << std::endl;
        REQUIRE_NE(path, "");
        REQUIRE_NE(path.back(), '/');
        REQUIRE_NE(path.back(), '\\');
    }

    TEST_CASE("create_directory")
    {
        std::string temp_path = get_temp_directory_path();
        std::string path = temp_path + "/intermediate/logs";
        bool res = create_directory(path);
        REQUIRE_UNARY(res);
    }

    TEST_CASE("get_current_pid")
    {
        int pid = get_current_pid();
        REQUIRE_NE(pid, -1);
    }

    TEST_CASE("get_tmp_hash_seed")
    {
        size_t hs = get_tmp_hash_seed();
        size_t expected = static_cast<std::size_t>(0xc70f6907UL);
        REQUIRE_EQ(hs, expected);
    }
    }
}

