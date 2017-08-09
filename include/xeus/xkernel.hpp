/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XKERNEL_HPP
#define XKERNEL_HPP

#include "xeus.hpp"
#include "xinterpreter.hpp"
#include "xkernel_configuration.hpp"
#include "xserver.hpp"
#include <memory>
#include <string>

namespace xeus
{

    XEUS_API
    std::string get_user_name();

    class XEUS_API xkernel
    {
    public:

        using interpreter_ptr = std::unique_ptr<xinterpreter>;
        using server_ptr = std::unique_ptr<xserver>;
        using server_builder = server_ptr (*)(zmq::context_t& context,
                                              const xconfiguration& config);

        xkernel(const xconfiguration& config,
                const std::string& user_name,
                interpreter_ptr interpreter,
                server_builder builder = make_xserver);

        void start();

    private:

        xconfiguration m_config;
        std::string m_user_name;
        interpreter_ptr p_interpreter;
        server_builder m_builder;
    };
}

#endif
