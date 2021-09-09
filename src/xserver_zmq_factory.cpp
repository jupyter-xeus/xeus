#include "xeus/xserver_zmq_factory.hpp"
#include "xeus/xserver_zmq.hpp"
#include "xeus/xserver_control_main.hpp"
#include "xeus/xserver_shell_main.hpp"

namespace xeus
{
    std::unique_ptr<xserver> make_xserver(zmq::context_t& context,
                                          const xconfiguration& config,
                                          nl::json::error_handler_t eh)
    {
        return std::make_unique<xserver_zmq>(context, config, eh);
    }

    std::unique_ptr<xserver> make_xserver_control_main(zmq::context_t& context,
                                                       const xconfiguration& config,
                                                       nl::json::error_handler_t eh)
    {
        return std::make_unique<xserver_control_main>(context, config, eh);
    }

    std::unique_ptr<xserver> make_xserver_shell_main(zmq::context_t& context,
                                                     const xconfiguration& config,
                                                     nl::json::error_handler_t eh)
    {
        return std::make_unique<xserver_shell_main>(context, config, eh);
    }
}

