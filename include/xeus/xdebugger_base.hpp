/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_DEBUGGER_BASE_HPP
#define XEUS_DEBUGGER_BASE_HPP

#include <cstddef>
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "zmq.hpp"
#include "nlohmann/json.hpp"

#include "xdebugger.hpp"
#include "xeus.hpp"

namespace xeus
{
    struct XEUS_API xdebugger_info
    {
        std::size_t m_hash_seed;
        std::string m_tmp_file_prefix;
        std::string m_tmp_file_suffix;
        bool m_rich_rendering;
        std::vector<std::string> m_exception_paths;

        xdebugger_info(std::size_t hash_seed,
                       const std::string& tmp_file_prefix,
                       const std::string& tmp_file_suffix,
                       bool rich_rendering = false,
                       std::vector<std::string> exception_paths = {});
    };

    class XEUS_API xdebugger_base : public xdebugger
    {
    public:

        virtual ~xdebugger_base() = default;
        
    protected:

        xdebugger_base(zmq::context_t& context);

        bool is_started() const;
        std::function<void (const nl::json&)> get_event_callback();

        /*********************
         * Requests handling *
         *********************/

        using request_handler_t = std::function<nl::json (const nl::json&)>;
        void register_request_handler(const std::string& command,
                                      const request_handler_t& handler,
                                      bool require_started);

        nl::json debug_info_request(const nl::json& message);
        nl::json dump_cell_request(const nl::json& message);
        nl::json set_breakpoints_request(const nl::json& message);
        nl::json source_request(const nl::json& message);
        nl::json stack_trace_request(const nl::json& message);
        nl::json variables_request(const nl::json& message);
        nl::json forward_message(const nl::json& message);

        /*******************
         * Events handling *
         *******************/

        using event_handler_t = std::function<void (const nl::json&)>;
        void register_event_handler(const std::string& event,
                                    const event_handler_t& handler);

        void continued_event(const nl::json& message);
        void stopped_event(const nl::json& message);

        const std::set<int>& get_stopped_threads() const;

    protected:

        virtual nl::json variables_request_impl(const nl::json& message);

    private:

        void handle_event(const nl::json& message);

        nl::json process_request_impl(const nl::json& header,
                                      const nl::json& message) override;

        virtual bool start(zmq::socket_t& header_socket,
                           zmq::socket_t& request_socket) = 0;
        virtual void stop(zmq::socket_t& header_socket,
                          zmq::socket_t& request_socket) = 0;
        virtual xdebugger_info get_debugger_info() const = 0;
        virtual std::string get_cell_temporary_file(const std::string& code) const = 0;

        zmq::socket_t m_header_socket;
        zmq::socket_t m_request_socket;
        
        using request_handler_map_t = std::map<std::string, request_handler_t>;
        request_handler_map_t m_started_handler;
        request_handler_map_t m_request_handler;

        using event_handler_map_t = std::map<std::string, event_handler_t>;
        event_handler_map_t m_event_handler;

        using breakpoint_list_t = std::map<std::string, std::vector<nl::json>>;
        breakpoint_list_t m_breakpoint_list;

        std::set<int> m_stopped_threads;
        std::mutex m_stopped_mutex;

        bool m_is_started;
    };
}

#endif

