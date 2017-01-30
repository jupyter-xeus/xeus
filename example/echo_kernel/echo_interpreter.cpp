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

    xjson echo_interpreter::execute_request_impl(int execution_counter,
                                                 const std::string& code,
                                                 bool silent,
                                                 bool store_history,
                                                 const xjson::node_type* /* user_expressions */,
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
        pub_data.add_member("text/plain", code);
        publish_execution_result(execution_counter, std::move(pub_data), xjson());
        
        xjson result;
        result.set_value("/status", "ok");
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
        result.set_value("/status", "ok");
        result.set_value("/matches/0", "a.echo1");
        result.set_value("/cursor_start", 2);
        result.set_value("/cursor_end", 6);
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
        result.set_value("/status", "ok");
        result.set_value("/found", false);
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
        result.create_value("/history/0");
        result.set_value("/history/0/0", args.m_session);
        result.set_value("/history/0/1", 0);
        result.set_value("/history/0/2", "");
        return result;
    }

    xjson echo_interpreter::is_complete_request_impl(const std::string& code)
    {
        std::cout << "Received is_complete_request" << std::endl;
        std::cout << "code: " << code << std::endl;
        std::cout << std::endl;
        xjson result;
        result.set_value("/status", "complete");
        return result;
    }

    xjson echo_interpreter::comm_info_request_impl(const std::string& target_name)
    {
        std::cout << "Received comm_info_request" << std::endl;
        std::cout << "target_name " << target_name << std::endl;
        std::cout << std::endl;
        xjson result;
        std::string uid = xeus::new_xguid().to_string();
        std::string snode_name = "/comms/" + uid + "/target_name";
        char node_name[35];
        std::copy(snode_name.begin(), snode_name.end(), node_name);
        result.set_value(node_name, target_name);
        return result;
    }

    xjson echo_interpreter::kernel_info_request_impl()
    {
        xjson result;
        result.set_value("/implementation", "cpp_echo");
        result.set_value("/implementation_version", "1.0.0");
        result.set_value("/language_info/name", "cpp");
        result.set_value("/language_info/version", "14.0.0");
        result.set_value("/language_info/mimetype", "text/x-c++src");
        result.set_value("/language_info/file_extension", ".cpp");
        return result;
    }

    void echo_interpreter::input_reply_impl(const std::string& value)
    {
        std::cout << "Received input_reply" << std::endl;
        std::cout << "value: " << value << std::endl;
    }

}
