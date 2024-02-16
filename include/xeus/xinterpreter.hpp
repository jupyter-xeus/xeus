/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_INTERPRETER_HPP
#define XEUS_INTERPRETER_HPP

#include <functional>
#include <string>
#include <vector>

#include "xcomm.hpp"
#include "xeus.hpp"
#include "xcontrol_messenger.hpp"
#include "xhistory_manager.hpp"
#include "xeus/xainterpreter.hpp"
#include "xeus/xaresponse_sender.hpp"

namespace xeus
{
    class xinterpreter;

    XEUS_API bool register_interpreter(xinterpreter* interpreter);
    XEUS_API xinterpreter& get_interpreter();

    class XEUS_API xinterpreter : public xainterpreter
    {
    public:

        xinterpreter() = default;
        virtual ~xinterpreter() = default;

        xinterpreter(const xinterpreter&) = delete;
        xinterpreter& operator=(const xinterpreter&) = delete;

        xinterpreter(xinterpreter&&) = delete;
        xinterpreter& operator=(xinterpreter&&) = delete;



    private:
        void async_execute_request_impl(
                                   int execution_counter,
                                   const std::string& code,
                                   bool silent,
                                   bool store_history,
                                   nl::json user_expressions,
                                   bool allow_stdin,
                                   xaresponse_sender) override;

        virtual nl::json execute_request_impl(int execution_counter,
                                              const std::string& code,
                                              bool silent,
                                              bool store_history,
                                              nl::json user_expressions,
                                              bool allow_stdin) = 0;

    };
}

#endif
