/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_MESSAGE_BUILDER_HPP
#define XEUS_MESSAGE_BUILDER_HPP

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "xtl/xvariant.hpp"

namespace nl = nlohmann;

namespace xeus
{
    nl::json create_error_reply(const std::string& evalue = std::string(),
                                const std::string& ename = std::string(),
                                const std::vector<std::string>& trace_back = nl::json::array());

    nl::json create_successful_reply(const nl::json& data = nl::json::object(),
                                     const nl::json& payload = nl::json::array(),
                                     const nl::json& user_expressions = nl::json::object());

    nl::json create_complete_reply(const std::vector<std::string>& matches,
                                   const int& cursor_start,
                                   const int& cursor_end,
                                   const std::string& code = std::string(),
                                   const nl::json& metadata = nl::json::object());

    nl::json create_inspect_reply(const nl::json& data = nl::json::object(),
                                  const nl::json& metadata = nl::json::object(),
                                  const bool& found = false);

    nl::json create_is_complete_reply(const std::string& status = std::string(),
                                      const std::string& indent = std::string(""));

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
                               const bool& debugger = false,
                               const nl::json& help_links = nl::json::object());
}

#endif
