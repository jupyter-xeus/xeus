/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>

#include "xeus/xinput.hpp"
#include "xeus/xinterpreter.hpp"

namespace xeus
{
    std::string blocking_input_request(
        const std::string& prompt, 
        bool password
    )
    {
        auto& interpreter = xeus::get_interpreter();

        // Register the input handler
        std::string value;
        interpreter.register_input_handler([&value](const std::string& v) { value = v; });

        // Send the input request
        interpreter.input_request(prompt, password);

        // Remove input handler
        interpreter.register_input_handler(nullptr);

        return value;
    }
}
