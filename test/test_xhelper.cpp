/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "xeus/xhelper.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace nl = nlohmann;

namespace xeus
{
    TEST_SUITE("xhelper")
    {
        TEST_CASE("get_start_message")
        {
            xkernel_configuration config;
            config.m_transport = "tcp";
            config.m_ip = "127.0.0.1";
            config.m_control_port = "50160";
            config.m_shell_port = "57503";
            config.m_stdin_port = "52597";
            config.m_iopub_port = "43072";
            config.m_hb_port = "50931";
            config.m_signature_scheme = "hmac-sha256";
            config.m_key = "a0436f6c-1916-498b-8eb9-e81ab9368e84";

            std::string msg = get_start_message(config);

            REQUIRE(msg.find("tcp") != std::string::npos);
            REQUIRE(msg.find("127.0.0.1") != std::string::npos);
            REQUIRE(msg.find("50160") != std::string::npos);
            REQUIRE(msg.find("57503") != std::string::npos);
            REQUIRE(msg.find("52597") != std::string::npos);
            REQUIRE(msg.find("43072") != std::string::npos);
            REQUIRE(msg.find("50931") != std::string::npos);
            REQUIRE(msg.find("hmac-sha256") != std::string::npos);
            REQUIRE(msg.find("a0436f6c-1916-498b-8eb9-e81ab9368e84") != std::string::npos);
        }

        TEST_CASE("extract_filename")
        {
            int argc = 3;
            char arg0[] = "program";
            char arg1[] = "-f";
            char arg2[] = "kernel.json";
            char* argv[] = { arg0, arg1, arg2 };

            std::string filename = extract_filename(argc, argv);

            REQUIRE_EQ(filename, "kernel.json");
            REQUIRE_EQ(argc, 1);
        }

        TEST_CASE("should_print_version")
        {
            char arg0[] = "program";
            char arg1[] = "--version";
            char* argv[] = { arg0, arg1 };

            REQUIRE_EQ(should_print_version(2, argv), true);

            char arg2[] = "--help";
            char* argv2[] = { arg0, arg2 };

            REQUIRE_EQ(should_print_version(2, argv2), false);
        }

        TEST_CASE("create_error_reply")
        {
            std::string ename = "RuntimeError";
            std::string evalue = "something went wrong";
            nl::json traceback = nl::json::array({"line 1", "line 2"});

            nl::json res = create_error_reply(ename, evalue, traceback);

            REQUIRE_EQ(res["status"], "error");
            REQUIRE_EQ(res["ename"], ename);
            REQUIRE_EQ(res["evalue"], evalue);
            REQUIRE_EQ(res["traceback"], traceback);
        }

        TEST_CASE("create_successful_reply")
        {
            nl::json payload = nl::json::array();
            payload.push_back({{"source", "page"}, {"start", 0}});
            nl::json user_expressions = {{"x", "42"}};

            nl::json res = create_successful_reply(payload, user_expressions);

            REQUIRE_EQ(res["status"], "ok");
            REQUIRE_EQ(res["payload"], payload);
            REQUIRE_EQ(res["user_expressions"], user_expressions);
        }

        TEST_CASE("create_complete_reply")
        {
            nl::json matches = nl::json::array({"foo", "foobar"});
            int cursor_start = 2;
            int cursor_end = 5;
            nl::json metadata = {{"_jupyter_types_experimental", true}};

            nl::json res = create_complete_reply(matches, cursor_start, cursor_end, metadata);

            REQUIRE_EQ(res["status"], "ok");
            REQUIRE_EQ(res["matches"], matches);
            REQUIRE_EQ(res["cursor_start"], cursor_start);
            REQUIRE_EQ(res["cursor_end"], cursor_end);
            REQUIRE_EQ(res["metadata"], metadata);
        }

        TEST_CASE("create_inspect_reply")
        {
            bool found = true;
            nl::json data = {{"text/plain", "int x = 42;"}};
            nl::json metadata = {{"text/plain", ""}};

            nl::json res = create_inspect_reply(found, data, metadata);

            REQUIRE_EQ(res["status"], "ok");
            REQUIRE_EQ(res["found"], found);
            REQUIRE_EQ(res["data"], data);
            REQUIRE_EQ(res["metadata"], metadata);
        }

        TEST_CASE("create_is_complete_reply")
        {
            std::string status = "incomplete";
            std::string indent = "    ";

            nl::json res = create_is_complete_reply(status, indent);

            REQUIRE_EQ(res["status"], status);
            REQUIRE_EQ(res["indent"], indent);
        }

        TEST_CASE("create_info_reply")
        {
            std::string implementation = "my_kernel";
            std::string implementation_version = "2.0.0";
            std::string language_name = "python";
            std::string language_version = "3.11.0";
            std::string language_mimetype = "text/x-python";
            std::string language_file_extension = ".py";
            std::string pygments_lexer = "python3";
            std::string codemirror_mode = "python";
            std::string nbconvert_exporter = "python";
            std::string banner = "Welcome to my_kernel!";
            nl::json help_links = nl::json::array({{{"text", "Help"}, {"url", "https://example.com"}}});
            std::vector<std::string> supported_features = {"kernel_interrupt"};

            nl::json res = create_info_reply(
                implementation, implementation_version,
                language_name, language_version,
                language_mimetype, language_file_extension,
                pygments_lexer, codemirror_mode,
                nbconvert_exporter, banner,
                help_links, supported_features
            );

            REQUIRE_EQ(res["status"], "ok");
            REQUIRE_EQ(res["implementation"], implementation);
            REQUIRE_EQ(res["implementation_version"], implementation_version);
            REQUIRE_EQ(res["language_info"]["name"], language_name);
            REQUIRE_EQ(res["language_info"]["version"], language_version);
            REQUIRE_EQ(res["language_info"]["mimetype"], language_mimetype);
            REQUIRE_EQ(res["language_info"]["file_extension"], language_file_extension);
            REQUIRE_EQ(res["language_info"]["pygments_lexer"], pygments_lexer);
            REQUIRE_EQ(res["language_info"]["codemirror_mode"], codemirror_mode);
            REQUIRE_EQ(res["language_info"]["nbconvert_exporter"], nbconvert_exporter);
            REQUIRE_EQ(res["banner"], banner);
            REQUIRE_EQ(res["help_links"], help_links);
            REQUIRE_EQ(res["supported_features"], supported_features);
        }

        TEST_CASE("create_shutdown_reply")
        {
            nl::json res_restart = create_shutdown_reply(true);
            REQUIRE_EQ(res_restart["status"], "ok");
            REQUIRE_EQ(res_restart["restart"], true);

            nl::json res_no_restart = create_shutdown_reply(false);
            REQUIRE_EQ(res_no_restart["status"], "ok");
            REQUIRE_EQ(res_no_restart["restart"], false);
        }

        TEST_CASE("create_interrupt_reply")
        {
            nl::json res = create_interrupt_reply();
            REQUIRE_EQ(res["status"], "ok");
        }
    }
}
