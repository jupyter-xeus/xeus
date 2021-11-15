#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xmessage.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xserver_emscripten.hpp"
#include "xeus/xembind.hpp"

#include <iostream>
#include <emscripten.h>

namespace nl = nlohmann;
namespace ems = emscripten;

namespace xeus
{

   
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

    void xserver_emscripten::js_notify_listener(ems::val js_message)
    {
        const std::string channel = js_message["channel"].as<std::string>();
        auto message = xmessage_from_js_message(js_message);

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
            (*p_js_callback)(std::string("shell"), 0, js_message_from_xmessage(message, true));
        }
    }

    void xserver_emscripten::send_control_impl(xmessage message) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("control"), 0, js_message_from_xmessage(message, true));
        }
    }

    void xserver_emscripten::send_stdin_impl(xmessage message) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("stdin"), 0, js_message_from_xmessage(message, true));
        }  
    }

    void xserver_emscripten::publish_impl(xpub_message message, channel c) 
    {
        if(p_js_callback != nullptr)
        {
            (*p_js_callback)(std::string("publish"), c == channel::SHELL ? 0:1, js_message_from_xmessage(message, true));
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

