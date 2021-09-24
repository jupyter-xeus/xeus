/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>

#include "nlohmann/json.hpp"
#include "xeus/xhelper.hpp"
#include "custom_interpreter.hpp"

namespace nl = nlohmann;


namespace custom
{

    nl::json custom_interpreter::execute_request_impl(int execution_counter, // Typically the cell number
                                                      const std::string& /*code*/, // Code to execute
                                                      bool /*silent*/,
                                                      bool /*store_history*/,
                                                      nl::json /*user_expressions*/,
                                                      bool /*allow_stdin*/)
    {
        // You can use the C-API of your target language for executing the code,
        // e.g. `PyRun_String` for the Python C-API
        //      `luaL_dostring` for the Lua C-API

        // Use this method for publishing the execution result to the client,
        // this method takes the ``execution_counter`` as first argument,
        // the data to publish (mime type data) as second argument and metadata
        // as third argument.
        // Replace "Hello World !!" by what you want to be displayed under the execution cell
        nl::json pub_data;
        pub_data["text/plain"] = "Hello World !!";
        publish_execution_result(execution_counter, std::move(pub_data), nl::json::object());

        // You can also use this method for publishing errors to the client, if the code
        // failed to execute
        // publish_execution_error(error_name, error_value, error_traceback);
        publish_execution_error("TypeError", "123", {"!@#$", "*(*"});

        return xeus::create_successful_reply();
    }

    void custom_interpreter::configure_impl()
    {
        // Perform some operations
    }

    nl::json custom_interpreter::complete_request_impl(const std::string& code,
                                                       int cursor_pos)
    {
        // Code starts with 'H', it could be the following completion
        if (code[0] == 'H')
        {
            return xeus::create_complete_reply({"Hello", "Hey", "Howdy"}, 5, cursor_pos);
        }
        // No completion result
        else
        {
            return xeus::create_complete_reply({}, cursor_pos, cursor_pos);
        }
    }

    nl::json custom_interpreter::inspect_request_impl(const std::string& code,
                                                      int /*cursor_pos*/,
                                                      int /*detail_level*/)
    {
        nl::json result;

        if (code.compare("print") == 0)
        {
            return xeus::create_inspect_reply(true,
                                              {"text/plain", "Print objects to the text stream file, [...]"});
        }
        else
        {
            return xeus::create_inspect_reply();
        }
    }

    nl::json custom_interpreter::is_complete_request_impl(const std::string& /*code*/)
    {
        return xeus::create_is_complete_reply("complete");
    }

    nl::json custom_interpreter::kernel_info_request_impl()
    {
        return xeus::create_info_reply("",
                                       "my_kernel",
                                       "0.1.0",
                                       "python",
                                       "3.7",
                                       "text/x-python",
                                       ".py");
    }

    void custom_interpreter::shutdown_request_impl()
    {
        std::cout << "Bye!!" << std::endl;
    }

}
