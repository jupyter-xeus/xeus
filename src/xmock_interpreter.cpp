/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xmock_interpreter.hpp"

namespace xeus
{
    xinterpreter* get_mock_interpreter()
    {
        static xmock_interpreter interpreter;
        return &interpreter;
    }

    xinterpreter*& get_registered_interpreter()
    {
        static xinterpreter* interpreter = nullptr;
        return interpreter;
    }

    bool register_interpreter(xinterpreter* interpreter)
    {
        xinterpreter*& interp = get_registered_interpreter();
        if (interp != nullptr)
        {
            return false;
        }
        else
        {
            interp = interpreter;
            return true;
        }
    }

    xinterpreter& get_interpreter()
    {
        xinterpreter* interp = get_registered_interpreter();
        if (interp != nullptr)
            return *interp;
        else
            return *get_mock_interpreter();
    }

    xmock_interpreter::xmock_interpreter()
        : base_type()
        , m_comm_manager(nullptr)
    {
        base_type::register_publisher(base_type::publisher_type());
        base_type::register_stdin_sender(base_type::stdin_sender_type());
        base_type::register_comm_manager(&m_comm_manager);
    }
}
