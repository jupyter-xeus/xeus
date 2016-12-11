/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <memory>
#include <iostream>
#include "xeus/xconfiguration.hpp"
#include "xeus/xkernel.hpp"
#include "echo_interpreter.hpp"

int main(int argc, char* argv[])
{
    std::string file_name = (argc == 1) ? "connection.json" : argv[2];
    xeus::xconfiguration config = xeus::load_configuration(file_name);

    using interpreter_ptr = std::unique_ptr<echo_kernel::echo_interpreter>;
    interpreter_ptr interpreter = interpreter_ptr(new echo_kernel::echo_interpreter());
    xeus::xkernel kernel(config, "jmabille", std::move(interpreter));
    std::cout << "starting kernel" << std::endl;
    kernel.start();

    return 0;
}
