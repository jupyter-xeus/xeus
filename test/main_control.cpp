/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <memory>

#include "test_interpreter.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_control_main.hpp"

int main(int argc, char* argv[])
{
    std::string file_name = (argc == 1) ? "connection.json" : argv[2];
    xeus::xconfiguration config = xeus::load_configuration(file_name);

    using history_manager_ptr = std::unique_ptr<xeus::xhistory_manager>;
    history_manager_ptr hist = xeus::make_in_memory_history_manager();
    
    using interpreter_ptr = std::unique_ptr<test_kernel::test_interpreter>;
    interpreter_ptr interpreter = interpreter_ptr(new test_kernel::test_interpreter());
    
    xeus::xkernel kernel(config,
                         xeus::get_user_name(),
                         std::move(interpreter),
                         std::move(hist),
                         nullptr,
                         xeus::make_xserver_control_main);
    std::cout << "starting kernel" << std::endl;
    kernel.start();

    return 0;
}
