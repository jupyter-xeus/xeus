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
    EM_JS(ems::EM_VAL, get_stdin, (), {
      return Asyncify.handleAsync(() => {
        return globalThis.get_stdin().then(msg => {
          return Emval.toHandle(msg);
        });
      });
    });

    EM_JS(void, post_kernel_message, (const char* channel, ems::EM_VAL message_handle), {
      var message = Emval.toValue(message_handle);
      message.channel = UTF8ToString(channel);
      if (typeof self !== 'undefined') {
        self.postMessage(message);
      }
    });

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
    :    p_messenger(new xtrivial_emscripten_messenger(this))
    {
    }

    xserver_emscripten::~xserver_emscripten()
    {
    }

    void xserver_emscripten::js_notify_listener(ems::val js_message)
    {
        const std::string channel = js_message["channel"].as<std::string>();
        auto message = xmessage_from_js_message(js_message);

        if(channel == "shell")
        {   
            this->notify_shell_listener(std::move(message));  
        }
        else if(channel == "control")
        {
            this->notify_control_listener(std::move(message));  
        }
        else if(channel == "stdin")
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
        post_kernel_message("shell", js_message_from_xmessage(message, true).as_handle());
    }

    void xserver_emscripten::send_control_impl(xmessage message) 
    {
        post_kernel_message("control", js_message_from_xmessage(message, true).as_handle());
    }

    void xserver_emscripten::send_stdin_impl(xmessage message) 
    {
        post_kernel_message("stdin", js_message_from_xmessage(message, true).as_handle());
        // Block until a response to the input request is received.
        ems::val js_message = ems::val::take_ownership(get_stdin());
        try
        {
            auto reply = xmessage_from_js_message(js_message);
            xserver::notify_stdin_listener(std::move(reply));
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void xserver_emscripten::publish_impl(xpub_message message, channel) 
    {
        post_kernel_message("iopub", js_message_from_xmessage(message, true).as_handle());
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

