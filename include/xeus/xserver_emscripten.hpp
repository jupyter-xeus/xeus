#ifndef XEUS_XEMSCRIPTEN_SERVER_HPP
#define XEUS_XEMSCRIPTEN_SERVER_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"

#include <emscripten/bind.h>

namespace nl = nlohmann;
namespace ems = emscripten;

namespace xeus
{

    class xserver_emscripten;

    class XEUS_API xtrivial_emscripten_messenger : public xcontrol_messenger
    {
    public:
        
        explicit xtrivial_emscripten_messenger(xserver_emscripten* server);
        virtual ~xtrivial_emscripten_messenger ();

    private:

        nl::json send_to_shell_impl(const nl::json& message) override;

        xserver_emscripten* p_server;
    };

    class XEUS_API xserver_emscripten : public xserver
    {
    public:

        xserver_emscripten(const xconfiguration& config);
        ~xserver_emscripten();

        void js_notify_listener(ems::val js_message);
        
        using xserver::notify_internal_listener;

    protected:

        xcontrol_messenger& get_control_messenger_impl() override;

        void send_shell_impl(xmessage message) override;
        void send_control_impl(xmessage message) override;
        void send_stdin_impl(xmessage message) override;
        void publish_impl(xpub_message message, channel c) override;

        void start_impl(xpub_message message) override;
        void abort_queue_impl(const listener& l, long polling_interval) override;
        void stop_impl() override;
        void update_config_impl(xconfiguration& config) const override;

        using trivial_messenger_ptr = std::unique_ptr<xtrivial_emscripten_messenger>;
        trivial_messenger_ptr p_messenger;
    };


    XEUS_API
    std::unique_ptr<xserver> make_xserver_emscripten(xcontext& context, const xconfiguration& config, nl::json::error_handler_t eh = nl::json::error_handler_t::strict);
}

#endif
