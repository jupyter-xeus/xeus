/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <fstream>
#include "rapidjson/istreamwrapper.h"
#include "xconfiguration.hpp"
#include "xjson.hpp"

namespace xeus
{

    xconfiguration load_configuration(const std::string& file_name)
    {
        std::ifstream ifs(file_name);
        rapidjson::IStreamWrapper isw(ifs);

        xjson doc;
        doc.parse(isw);

        xconfiguration res;
        res.m_transport = doc.get_string("/transport");
        res.m_ip = doc.get_string("/ip");
        res.m_control_port = std::to_string(doc.get_int("/control_port"));
        res.m_shell_port = std::to_string(doc.get_int("/shell_port"));
        res.m_stdin_port = std::to_string(doc.get_int("/stdin_port"));
        res.m_iopub_port = std::to_string(doc.get_int("/iopub_port"));
        res.m_hb_port = std::to_string(doc.get_int("/hb_port"));
        res.m_signature_scheme = doc.get_string("/signature_scheme", "");
        if (res.m_signature_scheme != "")
        {
            res.m_key = doc.get_string("/key");
        }
        else
        {
            res.m_key = "";
        }

        return res;
    }

}