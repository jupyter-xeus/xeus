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
#include "xeus/xmiddleware.hpp"
#include "xeus/xsystem.hpp"

namespace nl = nlohmann;

namespace xeus
{
    TEST(kernel, get_username)
    {
        std::string username;
        username = get_user_name();
        EXPECT_NE(username, "unspecified user");
    }

    TEST(kernel, find_free_port)
    {
        std::string port = find_free_port();
        EXPECT_NE(port, "");
        EXPECT_EQ(port.length(), std::size_t(5));
    }

    TEST(kernel, temp_directory_path)
    {
        std::string path = get_temp_directory_path();
        std::cout << "Temporary directory path: " << path << std::endl;
        EXPECT_NE(path, "");
    }

    TEST(kernel, create_directory)
    {
        std::string temp_path = get_temp_directory_path();
        std::string path = temp_path + "/intermediate/logs";
        bool res = create_directory(path);
        EXPECT_TRUE(res);
    }

    TEST(kernel, get_current_pid)
    {
        int pid = get_current_pid();
        EXPECT_NE(pid, -1);
    }
}

