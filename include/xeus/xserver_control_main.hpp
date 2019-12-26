/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_CONTROL_MAIN_HPP
#define XEUS_SERVER_CONTROL_MAIN_HPP

#include "xeus/xeus.hpp"
#include "xeus/xserver_zmq_split.hpp"
#include "xeus/xkernel_configuration.hpp"
#include "xeus/xcontrol_messenger.hpp"

namespace xeus
{
    class XEUS_API xserver_control_main : public xserver_zmq_split
    {
    public:

        xserver_control_main(zmq::context_t& context, const xconfiguration& config);
        virtual ~xserver_control_main();

    protected:

        void start_impl(zmq::multipart_t& message) override;
    };
}

#endif

