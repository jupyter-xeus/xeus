/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONFIGURATION_HPP
#define XEUS_CONFIGURATION_HPP

#include <string>

#include "xeus.hpp"

namespace xeus
{
    struct XEUS_API xconfiguration
    {
        std::string m_transport = "tcp";
        std::string m_ip = "127.0.0.1";
        std::string m_control_port;
        std::string m_shell_port;
        std::string m_stdin_port;
        std::string m_iopub_port;
        std::string m_hb_port;
        std::string m_signature_scheme = "hmac-sha256";
        std::string m_key;
    };

    XEUS_API
    xconfiguration load_configuration(const std::string& file_name);
}

#endif
