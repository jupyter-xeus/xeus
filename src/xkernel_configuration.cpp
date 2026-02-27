/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <fstream>
#include <string>

#include "xeus/xjson.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace nl = nlohmann;

namespace xeus
{
    namespace
    {
        void load_common_configuration(const nl::json& doc, xcommon_configuration& res)
        {
            res.m_transport = doc["transport"].get<std::string>();
            res.m_ip = doc["ip"].get<std::string>();
            res.m_signature_scheme = doc.value("signature_scheme", "");
            if (res.m_signature_scheme != "")
            {
                res.m_key = doc["key"].get<std::string>();
            }
            else
            {
                res.m_key = "";
            }
        }

        xkernel_configuration load_kernel_configuration(const nl::json& doc)
        {
            xkernel_configuration res;
            load_common_configuration(doc, res);
            res.m_control_port = std::to_string(doc["control_port"].get<int>());
            res.m_shell_port = std::to_string(doc["shell_port"].get<int>());
            res.m_stdin_port = std::to_string(doc["stdin_port"].get<int>());
            res.m_iopub_port = std::to_string(doc["iopub_port"].get<int>());
            res.m_hb_port = std::to_string(doc["hb_port"].get<int>());
            return res;
        }

        xregistration_configuration load_registration_configuration(const nl::json& doc)
        {
            xregistration_configuration res;
            load_common_configuration(doc, res);
            res.m_kernel_id = doc["kernel_id"].get<std::string>();
            res.m_registration_ip = doc["registration_ip"].get<std::string>();
            res.m_registration_port = doc["registration_port"].get<std::string>();
            return res;
        }
    }

    xconfiguration load_configuration(const std::string& file_name)
    {
        std::ifstream ifs(file_name);

        nl::json doc;
        ifs >> doc;

        std::string registration_ip = doc.value("registration_ip", "");
        if (registration_ip != "")
        {
            return load_registration_configuration(doc);
        }
        else
        {
            return load_kernel_configuration(doc);
        }
    }
}
