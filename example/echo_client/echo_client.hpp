/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <string>
#include "zmq.hpp"
#include "xeus/xconfiguration.hpp"
#include "xeus/xauthentication.hpp"

namespace echo_client
{

    class xclient
    {

    public:

        xclient(const xeus::xconfiguration& config,
                const std::string& user_name,
                int nb_msg,
                zmq::context_t& context);

        void send_code(const std::string& code);
        void send_stop();

    private:

        void wait_for_reply(zmq::socket_t& socket);
        void run_io_suscriber();

        zmq::socket_t m_shell;
        zmq::socket_t m_control;
        zmq::socket_t m_iosub;

        using authentication_ptr = std::unique_ptr<xeus::xauthentication>;
        authentication_ptr p_authentication;
        authentication_ptr p_io_authentication;

        std::string m_user_name;
        std::string m_session_id;
        int m_nb_msg;
    };

}
