/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "echo_interpreter.hpp"
#include "xeus/xguid.hpp"

namespace echo_kernel
{

    void echo_interpreter::configure_impl()
    {
        auto handle_comm_opened = [](xeus::xcomm&& comm, const xeus::xmessage&) {
            std::cout << "Comm opened for target: " << comm.target().name() << std::endl;
        };
        comm_manager().register_comm_target("echo_target", handle_comm_opened);
        using function_type = std::function<void(xeus::xcomm&&, const xeus::xmessage&)>;
    }

    xjson echo_interpreter::execute_request_impl(int execution_counter,
                                                 const std::string& code,
                                                 bool /* silent */,
                                                 bool /* store_history */,
                                                 const xjson_node* /* user_expressions */,
                                                 bool /* allow_stdin */)
    {
        xjson kernel_res;

        if (code.compare("hello, world") == 0)
        {
            publish_stream("stdout", code);
        }

        if (code.compare("error") == 0)
        {
            publish_stream("stderr", code);
        }

        if (code.compare("?") == 0)
        {
            std::string html_content = R"(<iframe class="xpyt-iframe-pager" src="
                https://xeus.readthedocs.io"></iframe>)";

            kernel_res["status"] = "ok";
            kernel_res["payload"] = xeus::xjson::array();
            kernel_res["payload"][0] = xeus::xjson::object({
                {"data", {
                    {"text/plain", "https://xeus.readthedocs.io"},
                    {"text/html", html_content}}
                },
                {"source", "page"},
                {"start", 0}
            });
            kernel_res["user_expressions"] = xeus::xjson::object();

            return kernel_res;
        }

        xjson pub_data;
        pub_data["text/plain"] = code;
        publish_execution_result(execution_counter, std::move(pub_data), xjson());

        kernel_res["status"] = "ok";
        kernel_res["payload"] = xeus::xjson::array();
        kernel_res["user_expressions"] = xeus::xjson::object();

        return kernel_res;
    }

    xjson echo_interpreter::complete_request_impl(const std::string& /* code */,
                                                  int /* cursor_pos */)
    {
        xjson result;
        result["status"] = "ok";
        result["matches"] = {"a.echo1", "a.echo2"};
        result["cursor_start"] = 2;
        result["cursor_end"] = 6;

        return result;
    }

    xjson echo_interpreter::inspect_request_impl(const std::string& /* code */,
                                                 int /* cursor_pos */,
                                                 int /* detail_level */)
    {
        xjson result;
        result["status"] = "ok";
        result["found"] = true;
        result["data"] = {{"text/plain", ""}};
        result["metadata"] = {{"text/plain", ""}};
        return result;
    }

    xjson echo_interpreter::is_complete_request_impl(const std::string& code)
    {
        xjson result;
        result["status"] = code;
        if (code.compare("incomplete") == 0)
        {
            result["indent"] = "   ";
        }
        return result;
    }

    xjson echo_interpreter::kernel_info_request_impl()
    {
        xjson result;
        result["implementation"] = "cpp_echo";
        result["implementation_version"] = "1.0.0";
        result["banner"] = "echo_kernel";
        result["language_info"]["name"] = "cpp";
        result["language_info"]["version"] = "14.0.0";
        result["language_info"]["mimetype"] = "text/x-c++src";
        result["language_info"]["file_extension"] = ".cpp";
        return result;
    }

    void echo_interpreter::shutdown_request_impl() {}
}
