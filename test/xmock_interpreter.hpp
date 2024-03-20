/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_MOCK_INTERPRETER_HPP
#define XEUS_MOCK_INTERPRETER_HPP

#include "xeus/xinterpreter.hpp"

namespace xeus
{
    class xmock_interpreter : public xinterpreter
    {

    public:

        xmock_interpreter() = default;
        virtual ~xmock_interpreter() = default;

    private:

        void configure_impl() override;

        void execute_request_impl(xrequest_context request_context,
                                  send_reply_callback cb,
                                  int execution_counter,
                                  const std::string& code,
                                  execute_request_config config,
                                  nl::json user_expressions) override;

        nl::json complete_request_impl(const std::string& code,
                                       int cursor_pos) override;

        nl::json inspect_request_impl(const std::string& code,
                                      int cursor_pos,
                                      int detail_level) override;

        nl::json is_complete_request_impl(const std::string& code) override;

        nl::json kernel_info_request_impl() override;

        void shutdown_request_impl() override;
    };
}

#endif
