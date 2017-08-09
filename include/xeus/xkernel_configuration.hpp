/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XCONFIGURATION_HPP
#define XCONFIGURATION_HPP

#include "xeus.hpp"
#include <string>

namespace xeus
{

    struct XEUS_API xconfiguration
    {
        std::string m_transport;
        std::string m_ip;
        std::string m_control_port;
        std::string m_shell_port;
        std::string m_stdin_port;
        std::string m_iopub_port;
        std::string m_hb_port;
        std::string m_signature_scheme;
        std::string m_key;
    };

    XEUS_API
    xconfiguration load_configuration(const std::string& file_name);
}

#endif
