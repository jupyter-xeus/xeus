/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include <vector>
#include <array>
#include <string>

#include "xeus/xhelper.hpp"
#include "nlohmann/json.hpp"



namespace nl = nlohmann;

namespace xeus
{


    // template<class ... ARGS>
    // class 


    TEST_SUITE("xhelper"){
        TEST_CASE("test_json_conversion")
        {
            xeus::xinfo_reply info_reply;
            info_reply.protocol_version = "";
            info_reply.implementation = "cpp_test";
            info_reply.implementation_version = "1.0.0";
            info_reply.language_name = "cpp";
            info_reply.language_version = "14.0.0";
            info_reply.language_mimetype = "text/c-c++src";
            info_reply.language_file_extension = ".cpp";
            info_reply.pygments_lexer = "";
            info_reply.language_codemirror_mode = "";
            info_reply.language_nbconvert_exporter = "";
            info_reply.banner = "test_kernel";
            info_reply.debugger = false;
            nl::json json =  info_reply;
        }
    }
}
