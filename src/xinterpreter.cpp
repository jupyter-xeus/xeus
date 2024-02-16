/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"

#include "xeus/xinterpreter.hpp"

namespace nl = nlohmann;

namespace xeus
{
  
    void xinterpreter::async_execute_request_impl(
                                           int execution_count,
                                           const std::string& code,
                                           bool silent,
                                           bool store_history,
                                           nl::json user_expressions,
                                           bool allow_stdin,
                                           xaresponse_sender response_sender)
    {

        nl::json reply = execute_request_impl(
            execution_count, code, silent,
            store_history, std::move(user_expressions), allow_stdin
        );
        response_sender(reply);
    }

}
