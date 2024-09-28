/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include <string>
#include <memory>

#include "nlohmann/json.hpp"

#include "xmock_interpreter.hpp"
#include "xmock_server.hpp"

#include "xeus/xkernel.hpp"
#include "xeus/xsystem.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xhelper.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace nl = nlohmann;

namespace xeus
{
    TEST_SUITE("kernel")
    {
        TEST_CASE("get_start_message")
        {
            auto context = make_mock_context();

            using interpreter_ptr = std::unique_ptr<xmock_interpreter>;
            interpreter_ptr interpreter = interpreter_ptr(new xmock_interpreter());
            xkernel kernel(get_user_name(),
                         std::move(context),
                         std::move(interpreter),
                         make_mock_server);
            std::string kernel_config = get_start_message(kernel.get_config());
            std::cout << kernel_config << std::endl;
            size_t pos = kernel_config.find("Starting kernel...\n"
                                            "\n"
                                            "If you want to connect to this kernel from an other client, just copy and paste the following content inside of a `kernel.json` file. And then run for example:\n"
                                            "\n"
                                            "# jupyter console --existing kernel.json\n"
                                            "\n"
                                            "kernel.json\n"
                                            "```\n"
                                            "{\n"
                                            "    \"transport\": \"tcp\",\n");
            REQUIRE_NE(pos, std::string::npos);
        }

        TEST_CASE("extract_filename")
        {
            int argc = 3;
            char* argv[2];
            argv[0] = (char*)"-f";
            argv[1] = (char*)"connection.json";
            std::string file_name = extract_filename(argc, argv);
            REQUIRE_EQ(file_name, "connection.json");
            REQUIRE_EQ(argc, 1);
        }

        TEST_CASE("should_print_version")
        {
            char* argv[2];
            argv[0] = (char*)"--version";
            REQUIRE_EQ(should_print_version(1, argv), true);
        }

        TEST_CASE("get_username")
        {
            std::string username;
            username = get_user_name();
            REQUIRE_NE(username, "unspecified user");
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
            REQUIRE_EQ(res, true);
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

