/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_MOCK_INTERPRETER_HPP
#define XEUS_MOCK_INTERPRETER_HPP

#include <string>

#include "xeus/xinterpreter.hpp"
#include "xeus/xeus.hpp"

namespace xeus
{
    class xmock_interpreter : public xinterpreter
    {
    public:

        using base_type = xinterpreter;

        xmock_interpreter();
        virtual ~xmock_interpreter() = default;

        xmock_interpreter(const xmock_interpreter&) = delete;
        xmock_interpreter& operator=(const xmock_interpreter&) = delete;

        xmock_interpreter(xmock_interpreter&&) = delete;
        xmock_interpreter& operator=(xmock_interpreter&&) = delete;

    private:

        void configure_impl() override
        {
        }

        void execute_request_impl(send_reply_callback cb,
                                  int /*execution_counter*/,
                                  const std::string& /*code*/,
                                  execute_request_config /*config*/,
                                  nl::json /*user_expressions*/) override
        {
            cb(nl::json());
        }

        nl::json complete_request_impl(const std::string& /*code*/, int /*cursor_pos*/) override
        {
            return nl::json();
        }

        nl::json inspect_request_impl(const std::string& /*code*/,
                                   int /*cursor_pos*/,
                                   int /*detail_level*/) override
        {
            return nl::json();
        }

        nl::json is_complete_request_impl(const std::string& /*code*/) override
        {
            return nl::json();
        }

        nl::json kernel_info_request_impl() override
        {
            return nl::json();
        }

        void shutdown_request_impl() override
        {
        }

        xcomm_manager m_comm_manager;
    };
}

#endif
