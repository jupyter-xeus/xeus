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
        auto handle_comm_opened = [](const xeus::xcomm& comm, const xeus::xmessage&) {
            std::cout << "Comm opened for target: " << comm.target().name() << std::endl;
        };
        comm_manager().register_comm_target("echo_target", handle_comm_opened);
        using function_type = std::function<void(const xeus::xcomm&, const xeus::xmessage&)>;
    }

    xjson echo_interpreter::execute_request_impl(int execution_counter,
                                                 const std::string& code,
                                                 bool silent,
                                                 bool store_history,
                                                 const xjson_node* /* user_expressions */,
                                                 bool allow_stdin)
    {
        std::cout << "Received execute_request" << std::endl;
        std::cout << "execution_counter: " << execution_counter << std::endl;
        std::cout << "code: " << code << std::endl;
        std::cout << "silent: " << silent << std::endl;
        std::cout << "store_history: " << store_history << std::endl;
        std::cout << "allow_stdin: " << allow_stdin << std::endl;
        std::cout << std::endl;

        xjson pub_data;
        pub_data["text/plain"] = code;
        publish_execution_result(execution_counter, std::move(pub_data), xjson());

        xjson result;
        result["status"] = "ok";
        return result;
    }

    xjson echo_interpreter::complete_request_impl(const std::string& code,
                                                  int cursor_pos)
    {
        std::cout << "Received complete_request" << std::endl;
        std::cout << "code: " << code << std::endl;
        std::cout << "cursor_pos: " << cursor_pos << std::endl;
        std::cout << std::endl;
        xjson result;
        result["status"] = "ok";
        result["matches"] = {"a.echo1"};
        result["cursor_start"] = 2;
        result["cursor_end"] = 6;
        return result;
    }

    xjson echo_interpreter::inspect_request_impl(const std::string& code,
                                                 int cursor_pos,
                                                 int detail_level)
    {
        std::cout << "Received inspect_request" << std::endl;
        std::cout << "code: " << code << std::endl;
        std::cout << "cursor_pos: " << cursor_pos << std::endl;
        std::cout << "detail_level: " << detail_level << std::endl;
        std::cout << std::endl;
        xjson result;
        result["status"] = "ok";
        result["found"] = false;
        return result;
    }

    xjson echo_interpreter::history_request_impl(const xhistory_arguments& args)
    {
        std::cout << "Received history_request" << std::endl;
        std::cout << "output: " << args.m_output << std::endl;
        std::cout << "raw: " << args.m_raw << std::endl;
        std::cout << "hist_access_type: " << args.m_hist_access_type << std::endl;
        std::cout << "session: " << args.m_session << std::endl;
        std::cout << "start: " << args.m_start << std::endl;
        std::cout << "stop: " << args.m_stop << std::endl;
        std::cout << "n: " << args.m_n << std::endl;
        std::cout << "pattern: " << args.m_pattern << std::endl;
        std::cout << "unique: " << args.m_unique << std::endl;
        std::cout << std::endl;
        xjson result;
        result["history"] = {{args.m_session, 0, ""}};
        return result;
    }

    xjson echo_interpreter::is_complete_request_impl(const std::string& code)
    {
        std::cout << "Received is_complete_request" << std::endl;
        std::cout << "code: " << code << std::endl;
        std::cout << std::endl;
        xjson result;
        result["status"] = "complete";
        return result;
    }

    xjson echo_interpreter::kernel_info_request_impl()
    {
        xjson result;
        result["implementation"] = "cpp_echo";
        result["implementation_version"] = "1.0.0";
        result["language_info"]["name"] = "cpp";
        result["language_info"]["version"] = "14.0.0";
        result["language_info"]["mimetype"] = "text/x-c++src";
        result["language_info"]["file_extension"] = ".cpp";
        return result;
    }

    void echo_interpreter::input_reply_impl(const std::string& value)
    {
        std::cout << "Received input_reply" << std::endl;
        std::cout << "value: " << value << std::endl;
    }
}
