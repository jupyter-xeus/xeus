/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <fstream>
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xjson.hpp"

namespace xeus
{
    xconfiguration load_configuration(const std::string& file_name)
    {
        std::ifstream ifs(file_name);

        xjson doc;
        ifs >> doc;

        xconfiguration res;
        res.m_transport = doc["transport"];
        res.m_ip = doc["ip"];
        res.m_control_port = std::to_string(doc["control_port"].get<int>());
        res.m_shell_port = std::to_string(doc["shell_port"].get<int>());
        res.m_stdin_port = std::to_string(doc["stdin_port"].get<int>());
        res.m_iopub_port = std::to_string(doc["iopub_port"].get<int>());
        res.m_hb_port = std::to_string(doc["hb_port"].get<int>());
        res.m_signature_scheme = doc.value("signature_scheme", "");
        if (res.m_signature_scheme != "")
        {
            res.m_key = doc["key"];
        }
        else
        {
            res.m_key = "";
        }

        return res;
    }
}
