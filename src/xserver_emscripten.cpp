
#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_emscripten.hpp"

#include <iostream>
#include <emscripten.h>

namespace nl = nlohmann;

namespace xeus
{

    std::string serialize_msg(const xmessage & message)
    {
        auto json_msg = nl::json::object();
        json_msg["header"] = message.header();
        json_msg["parent_header"] = message.parent_header();
        json_msg["metadata"] = message.metadata();
        json_msg["content"] = message.content();

        auto buffers = nl::json::array();
        for(const auto & bb : message.buffers())
        {   
            std::string buffer_as_str(bb.begin(), bb.end());
            buffers.push_back(buffer_as_str);
        }
        return json_msg.dump();
    }
    
    std::string serialize_msg(const xpub_message & message)
    {
        auto json_msg = nl::json::object();
        json_msg["header"] = message.header();
        json_msg["parent_header"] = message.parent_header();
        json_msg["metadata"] = message.metadata();
        json_msg["content"] = message.content();
        json_msg["topic"] = message.topic();

        auto buffers = nl::json::array();
        for(const auto & bb : message.buffers())
        {   
            std::string buffer_as_str(bb.begin(), bb.end());
            buffers.push_back(buffer_as_str);
        }
        return json_msg.dump();
    }

    xtrivial_emscripten_messenger::xtrivial_emscripten_messenger(xserver_emscripten* server)
    : p_server(server)
    {
    }

    xtrivial_emscripten_messenger::~xtrivial_emscripten_messenger()
    {
    }

    nl::json xtrivial_emscripten_messenger::send_to_shell_impl(const nl::json& message)
    {
        return p_server->notify_internal_listener(message);
    }

    xserver_emscripten::xserver_emscripten(const xconfiguration& /*config*/)
    :    p_messenger(new xtrivial_emscripten_messenger(this)),
    p_js_callback(nullptr)
    {

    }

    xserver_emscripten::~xserver_emscripten()
    {
        if(p_js_callback!=nullptr)
        {
            delete p_js_callback;
        }
    }

    void xserver_emscripten::js_notify_listener(const std::string & json_str, const std::string & channel)
    {
        auto json_msg = nl::json::parse(json_str);
        
        const auto header = json_msg["header"];
        const auto parent_header = json_msg["parent_header"];
        const auto metadata = json_msg["metadata"];
        const auto content = json_msg["content"];
        auto buffer_sequence = xeus::buffer_sequence();
        // convert buffer from json-array of strings
        // to a vector<vector<char>>
        if (json_msg.find("buffer") != json_msg.end())
        {
            const auto buffers = json_msg["buffers"];
            for(std::string bb : buffers)
            {
                buffer_sequence.emplace_back(bb.begin(), bb.end());
            }
        }
        const auto empty_guid_list = xmessage::guid_list();

        xmessage message(empty_guid_list, 
            header,
            parent_header,
            metadata,
            content,
            buffer_sequence
        );

        if(channel == std::string("shell"))
        {   
            this->notify_shell_listener(std::move(message));  
        }
        else if(channel == std::string("control"))
        {
            this->notify_control_listener(std::move(message));  
        }
        else if(channel == std::string("stdin"))
        {
            this->notify_stdin_listener(std::move(message));  
        }
        else
        {
            throw std::runtime_error("unknown channel");
        }
    }

    xcontrol_messenger& xserver_emscripten::get_control_messenger_impl() 
    {
        return *p_messenger;
    }

    void xserver_emscripten::send_shell_impl(xmessage message) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("shell"), 0, serialize_msg(message));
        }
    }

    void xserver_emscripten::send_control_impl(xmessage message) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("control"), 0, serialize_msg(message));
        }
    }

    void xserver_emscripten::send_stdin_impl(xmessage message) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("stdin"), 0, serialize_msg(message));
        }  
    }

    void xserver_emscripten::publish_impl(xpub_message message, channel c) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("publish"), c == channel::SHELL ? 0:1, serialize_msg(message));
        }        
    }

    void xserver_emscripten::register_js_callback(emscripten::val callback)
    {
        if(p_js_callback == nullptr)
        {   
            p_js_callback = new emscripten::val(std::move(callback));
        }
        else
        {
            throw std::runtime_error("callback is already registerd");
        }
    }

    void xserver_emscripten::start_impl(xpub_message  /*message*/) 
    {
      
    }

    void xserver_emscripten::abort_queue_impl(const listener& /*l*/, long /*polling_interval*/) 
    {

    }

    void xserver_emscripten::stop_impl() 
    {
    }

    void xserver_emscripten::update_config_impl(xconfiguration& /*config*/) const 
    {
    }

    std::unique_ptr<xserver> make_xserver_emscripten(xcontext& /*context*/, const xconfiguration& config, nl::json::error_handler_t /*eh*/)
    {
        return std::make_unique<xserver_emscripten>(config);
    }
}

