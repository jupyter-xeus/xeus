/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef ECHO_INTERPRETER_HPP
#define ECHO_INTERPRETER_HPP

#include "xeus/xinterpreter.hpp"

using xeus::xinterpreter;
using xeus::xjson;
using xeus::xjson_node;

namespace echo_kernel
{
    class echo_interpreter : public xinterpreter
    {

    public:

        echo_interpreter() = default;
        virtual ~echo_interpreter() = default;

    private:

        void configure_impl() override;

        xjson execute_request_impl(int execution_counter,
                                   const std::string& code,
                                   bool silent,
                                   bool store_history,
                                   const xjson_node* user_expressions,
                                   bool allow_stdin) override;

        xjson complete_request_impl(const std::string& code,
                                    int cursor_pos) override;

        xjson inspect_request_impl(const std::string& code,
                                   int cursor_pos,
                                   int detail_level) override;

        xjson is_complete_request_impl(const std::string& code) override;

        xjson kernel_info_request_impl() override;

        void shutdown_request_impl() override;
    };
}

#endif
