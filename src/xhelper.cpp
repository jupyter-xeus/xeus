#include <string>
#include <vector>

#include "nlohmann/json.hpp"

#include "xeus/xhelper.hpp"

namespace nl = nlohmann;

namespace xeus
{
    std::string get_start_message(const xconfiguration& config)
    {
        std::string kernel_info;
        kernel_info = "Starting kernel...\n\n"
            "If you want to connect to this kernel from an other client, just copy"
            " and paste the following content inside of a `kernel.json` file. And then run for example:\n\n"
            "# jupyter console --existing kernel.json\n\n"
            "kernel.json\n```\n{\n"
            "    \"transport\": \"" + config.m_transport + "\",\n"
            "    \"ip\": \"" + config.m_ip + "\",\n"
            "    \"control_port\": " + config.m_control_port + ",\n"
            "    \"shell_port\": " + config.m_shell_port + ",\n"
            "    \"stdin_port\": " + config.m_stdin_port + ",\n"
            "    \"iopub_port\": " + config.m_iopub_port + ",\n"
            "    \"hb_port\": " + config.m_hb_port + ",\n"
            "    \"signature_scheme\": \"" + config.m_signature_scheme + "\",\n"
            "    \"key\": \"" + config.m_key + "\"\n"
            "}\n```";
        return kernel_info;
    }

    std::string extract_filename(int &argc, char* argv[])
    {
        std::string res = "";
        for (int i = 0; i < argc; ++i)
        {
            if ((std::string(argv[i]) == "-f") && (i + 1 < argc))
            {
                res = argv[i + 1];
                for(int j = i; j < argc - 2; ++j)
                {
                    argv[j]  = argv[j + 2];
                }
                argc -= 2;
                break;
            }
        }
        return res;
    }

    bool should_print_version(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
        {
            if (std::string(argv[i]) == "--version")
            {
                return true;
            }
        }
        return false;
    }

    // Helpers that create replies to the server
    nl::json create_error_reply(const std::string& evalue,
                                const std::string& ename,
                                const nl::json& trace_back)
    {
        nl::json kernel_res;
        kernel_res["status"] = "error";
        kernel_res["ename"] = ename;
        kernel_res["evalue"] = evalue;
        kernel_res["traceback"] = trace_back;
        return kernel_res;
    }

    nl::json create_successful_reply(const nl::json& payload,
                                     const nl::json& user_expressions)
    {
        nl::json kernel_res;
        kernel_res["status"] = "ok";
        kernel_res["payload"] = payload;
        kernel_res["user_expressions"] = user_expressions;
        return kernel_res;
    }

    nl::json create_complete_reply(const nl::json& matches,
                                   const int& cursor_start,
                                   const int& cursor_end,
                                   const nl::json& metadata)
    {
        nl::json kernel_res;
        kernel_res["status"] = "ok";
        kernel_res["matches"] = matches;
        kernel_res["cursor_start"] = cursor_start;
        kernel_res["cursor_end"] = cursor_end;
        kernel_res["metadata"] = metadata;
        return kernel_res;
    }

    nl::json create_inspect_reply(const bool found,
                                  const nl::json& data,
                                  const nl::json& metadata)
    {
        nl::json kernel_res;
        kernel_res["status"] = "ok";
        kernel_res["found"] = found;
        kernel_res["data"] = data;
        kernel_res["metadata"] = metadata;
        return kernel_res;
    }

    nl::json create_is_complete_reply(const std::string& status,
                                      const std::string& indent)
    {
        nl::json kernel_res;
        kernel_res["status"] = status;
        kernel_res["indent"] = indent;
        return kernel_res;
    }

    nl::json create_info_reply(const std::string& protocol_version,
                               const std::string& implementation,
                               const std::string& implementation_version,
                               const std::string& language_name,
                               const std::string& language_version,
                               const std::string& language_mimetype,
                               const std::string& language_file_extension,
                               const std::string& language_pygments_lexer,
                               const std::string& language_codemirror_mode,
                               const std::string& language_nbconvert_exporter,
                               const std::string& banner,
                               const bool debugger,
                               const nl::json& help_links)
    {
        nl::json kernel_res;
        kernel_res["status"] = "ok";
        kernel_res["protocol_version"] = protocol_version;
        kernel_res["implementation"] = implementation;
        kernel_res["implementation_version"] = implementation_version;
        kernel_res["language_info"]["name"] = language_name;
        kernel_res["language_info"]["version"] = language_version;
        kernel_res["language_info"]["mimetype"] = language_mimetype;
        kernel_res["language_info"]["file_extension"] = language_file_extension;
        kernel_res["language_info"]["pygments_lexer"] = language_pygments_lexer;
        kernel_res["language_info"]["codemirror_mode"] = language_codemirror_mode;
        kernel_res["language_info"]["nbconvert_exporter"] = language_nbconvert_exporter;
        kernel_res["banner"] = banner;
        kernel_res["debugger"] = debugger;
        kernel_res["help_links"] = help_links;
        return kernel_res;
    }

}

