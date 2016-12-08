/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XINTERPRETER_HPP
#define XINTERPRETER_HPP

#include <string>
#include "xeus_export.hpp"
#include "xjson.hpp"

namespace xeus
{

    struct XEUS_API xhistory_arguments
    {
        std::string m_hist_access_type;
        bool m_output;
        bool m_raw;
        int m_session;
        int m_start;
        int m_stop;
        int m_n;
        std::string m_pattern;
        bool m_unique;
    };

    class XEUS_API xinterpreter
    {

    public:

        xinterpreter() = default;
        virtual ~xinterpreter() = default;

        xinterpreter(const xinterpreter&) = delete;
        xinterpreter& operator=(const xinterpreter&) = delete;

        xinterpreter(xinterpreter&&) = delete;
        xinterpreter& operator=(xinterpreter&&) = delete;

        xjson execute_request(const std::string& code,
                              bool silent,
                              bool store_history,
                              const xjson::node_type* user_expressions,
                              bool allow_stdin);

        xjson complete_request(const std::string& code,
                               int cursor_pos);

        xjson inspect_request(const std::string& code,
                              int cursor_pos,
                              int detail_level);

        xjson history_request(const xhistory_arguments& args);

        xjson is_complete_request(const std::string& code);

        xjson comm_info_request(const std::string& target_name);

        xjson kernel_info_request();

    private:

        virtual xjson execute_request_impl(const std::string& code,
                                           bool silent,
                                           bool store_history,
                                           const xjson::node_type* user_expressions,
                                           bool allow_stdin) = 0;

        virtual xjson complete_request_impl(const std::string& code,
                                            int cursor_pos) = 0;

        virtual xjson inspect_request_impl(const std::string& code,
                                           int cursor_pos,
                                           int detail_level) = 0;

        virtual xjson history_request_impl(const xhistory_arguments& args) = 0;

        virtual xjson is_complete_request_impl(const std::string& code) = 0;

        virtual xjson comm_info_request_impl(const std::string& target_name) = 0;

        virtual xjson kernel_info_request_impl() = 0;
    };

}

#endif
