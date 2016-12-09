/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <memory>
#include "xeus/xconfiguration.hpp"
#include "xeus/xkernel.hpp"
#include "echo_interpreter.hpp"

int main(int atgc, char* argv[])
{
    xeus::xconfiguration config;
    config.m_transport = "tcp";
    config.m_ip = "127.0.0.1";
    config.m_control_port = "50160";
    config.m_shell_port = "57503";
    config.m_stdin_port = "52597";
    config.m_iopub_port = "40885";
    config.m_hb_port = "42540";
    config.m_signature_scheme = "";
    config.m_signature_scheme = "hmac-sha256";
    config.m_key = "a0436f6c-1916-498b-8eb9-e81ab9368e84";

    using interpreter_ptr = std::unique_ptr<echo_kernel::echo_interpreter>;
    interpreter_ptr interpreter = interpreter_ptr(new echo_kernel::echo_interpreter());
    xeus::xkernel kernel(config, "jmabille", std::move(interpreter));
    kernel.start();

    return 0;
}
