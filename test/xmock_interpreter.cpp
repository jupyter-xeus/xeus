/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "nlohmann/json.hpp"

#include "xmock_interpreter.hpp"

#include "xeus/xhelper.hpp"
#include "xeus/xguid.hpp"

namespace nl = nlohmann;

namespace xeus
{

    void xmock_interpreter::configure_impl()
    {
        auto handle_comm_opened = [](xeus::xcomm&& comm, const xeus::xmessage&) {
            std::cout << "Comm opened for target: " << comm.target().name() << std::endl;
        };
        comm_manager().register_comm_target("test_target", handle_comm_opened);
        using function_type = std::function<void(xeus::xcomm&&, const xeus::xmessage&)>;
    }

    void xmock_interpreter::execute_request_impl(send_reply_callback cb,
                                                 int execution_counter,
                                                 const std::string& code,
                                                 execute_request_config /*config*/,
                                                 nl::json /*user_expressions*/)
    {
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

            auto payload = nl::json::array();
            payload[0] = nl::json::object({
                            {"data", {
                                {"text/plain", "https://xeus.readthedocs.io"},
                                {"text/html", html_content}}
                            },
                            {"source", "page"},
                            {"start", 0}
                        });

            cb(xeus::create_successful_reply(payload));
        }

        nl::json pub_data;
        pub_data["text/plain"] = code;
        publish_execution_result(execution_counter, std::move(pub_data), nl::json::object());

        cb(xeus::create_successful_reply());
    }

    nl::json xmock_interpreter::complete_request_impl(const std::string& /* code */,
                                                      int /* cursor_pos */)
    {
        return xeus::create_complete_reply({"a.test1", "a.test2"}, 2, 6);
    }

    nl::json xmock_interpreter::inspect_request_impl(const std::string& /* code */,
                                                     int /* cursor_pos */,
                                                     int /* detail_level */)
    {
        return xeus::create_inspect_reply(true, {{"text/plain", ""}}, {{"text/plain", ""}});
    }

    nl::json xmock_interpreter::is_complete_request_impl(const std::string& code)
    {
        nl::json result = xeus::create_is_complete_reply(code);
        if (code.compare("incomplete") == 0)
        {
            result["indent"] = "   ";
        }
        return result;
    }

    nl::json xmock_interpreter::kernel_info_request_impl()
    {
        return xeus::create_info_reply("",
                                       "cpp_test",
                                       "1.0.0",
                                       "cpp",
                                       "14.0.0",
                                       "text/x-c++src",
                                       ".cpp",
                                       "",
                                       "",
                                       "",
                                       "test_kernel");
    }

    void xmock_interpreter::shutdown_request_impl()
    {
    }
}
