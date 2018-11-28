/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUX_MOCK_INTERPRETER_HPP
#define XEUX_MOCK_INTERPRETER_HPP

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

        xjson execute_request_impl(int /*execution_counter*/,
                                   const std::string& /*code*/,
                                   bool /*silent*/,
                                   bool /*store_history*/,
                                   const xjson_node* /*user_expressions*/,
                                   bool /*allow_stdin*/) override
        {
            return xjson();
        }

        xjson complete_request_impl(const std::string& /*code*/, int /*cursor_pos*/) override
        {
            return xjson();
        }

        xjson inspect_request_impl(const std::string& /*code*/,
                                   int /*cursor_pos*/,
                                   int /*detail_level*/) override
        {
            return xjson();
        }

        xjson is_complete_request_impl(const std::string& /*code*/) override
        {
            return xjson();
        }

        xjson kernel_info_request_impl() override
        {
            return xjson();
        }

        void shutdown_request_impl() override
        {
        }

        void input_reply_impl(const std::string& /*value*/) override
        {
        }

        xcomm_manager m_comm_manager;
    };
}

#endif
