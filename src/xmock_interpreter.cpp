/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xmock_interpreter.hpp"
#include "xeus/xainterpreter.hpp"

namespace xeus
{
    xinterpreter* get_mock_interpreter()
    {
        static xmock_interpreter interpreter;
        return &interpreter;
    }
    xainterpreter* get_mock_ainterpreter()
    {
        static xmock_interpreter interpreter;
        return &interpreter;
    }

    xinterpreter*& get_registered_interpreter()
    {
        static xinterpreter* interpreter = nullptr;
        return interpreter;
    }
    xainterpreter*& get_registered_ainterpreter()
    {
        static xainterpreter* ainterpreter = nullptr;
        return ainterpreter;
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

    bool register_ainterpreter(xainterpreter* ainterpreter)
    {
        xainterpreter*& ainterp = get_registered_ainterpreter();
        if (ainterp != nullptr)
        {
            return false;
        }
        else
        {
            ainterp = ainterpreter;
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
    xainterpreter& get_ainterpreter()
    {
        xainterpreter* ainterp = get_registered_ainterpreter();
        if (ainterp != nullptr)
            return *ainterp;
        else
            return *get_mock_ainterpreter();
    }
    
    xmock_interpreter::xmock_interpreter()
        : base_type()
        , m_comm_manager(nullptr)
    {
        base_type::register_publishers(base_type::publisher_type(), base_type::publisher_with_parent_type());
        base_type::register_stdin_sender(base_type::stdin_sender_type());
        base_type::register_comm_manager(&m_comm_manager);
    }
}
