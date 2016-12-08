/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xinterpreter.hpp"

namespace xeus
{

    xjson xinterpreter::execute_request(const std::string& code,
                                        bool silent,
                                        bool store_history,
                                        const xjson::node_type* user_expressions,
                                        bool allow_stdin)
    {
        return execute_request_impl(code, silent, store_history, user_expressions, allow_stdin);
    }

    xjson xinterpreter::complete_request(const std::string& code,
                                         int cursor_pos)
    {
        return complete_request_impl(code, cursor_pos);
    }

    xjson xinterpreter::inspect_request(const std::string& code,
                                        int cursor_pos,
                                        int detail_level)
    {
        return inspect_request_impl(code, cursor_pos, detail_level);
    }

    xjson xinterpreter::history_request(const xhistory_arguments& args)
    {
        return history_request_impl(args);
    }

    xjson xinterpreter::is_complete_request(const std::string& code)
    {
        return is_complete_request_impl(code);
    }

    xjson xinterpreter::comm_info_request(const std::string& target_name)
    {
        return comm_info_request_impl(target_name);
    }

    xjson xinterpreter::kernel_info_request()
    {
        return kernel_info_request_impl();
    }

}
