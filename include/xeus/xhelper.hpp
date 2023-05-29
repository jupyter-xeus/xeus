/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_HELPER_HPP
#define XEUS_HELPER_HPP

#include <iostream>
#include <string>
#include <vector>

#include "xeus/xeus.hpp"
#include "xeus/xkernel_configuration.hpp"

#include "nlohmann/json.hpp"

namespace nl = nlohmann;

namespace xeus
{
    XEUS_API std::string get_started_message(const xconfiguration& config);

    [[deprecated("Use get_started_message instead")]]
    XEUS_API std::string print_starting_message(const xconfiguration& config);

    XEUS_API std::string extract_filename(int argc, char* argv[]);

    XEUS_API bool should_print_version(int argc, char* argv[]);

    XEUS_API
    nl::json create_error_reply(const std::string& ename = std::string(),
                                const std::string& evalue = std::string(),
                                const nl::json& trace_back = nl::json::array());

    XEUS_API
    nl::json create_successful_reply(const nl::json& payload = nl::json::array(),
                                     const nl::json& user_expressions = nl::json::object());

    XEUS_API
    nl::json create_complete_reply(const nl::json& matches,
                                   const int& cursor_start,
                                   const int& cursor_end,
                                   const nl::json& metadata = nl::json::object());

    XEUS_API
    nl::json create_inspect_reply(const bool found = false,
                                  const nl::json& data = nl::json::object(),
                                  const nl::json& metadata = nl::json::object());

    XEUS_API
    nl::json create_is_complete_reply(const std::string& status = std::string(),
                                      const std::string& indent = std::string(""));

    XEUS_API
    nl::json create_info_reply(const std::string& protocol_version = std::string(),
                               const std::string& implementation = std::string(),
                               const std::string& implementation_version = std::string(),
                               const std::string& language_name = std::string(),
                               const std::string& language_version = std::string(),
                               const std::string& language_mimetype = std::string(),
                               const std::string& language_file_extension = std::string(),
                               const std::string& pygments_lexer = std::string(),
                               const std::string& language_codemirror_mode = std::string(),
                               const std::string& language_nbconvert_exporter = std::string(),
                               const std::string& banner = std::string(),
                               const bool debugger = false,
                               const nl::json& help_links = nl::json::array());
}

#endif
