/***************************************************************************
* Copyright (c) 2024, Dr. Thorsten Beier                                   *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_ARESPONSE_SENDER_HPP
#define XEUS_ARESPONSE_SENDER_HPP


#include "xeus.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
 

namespace nl = nlohmann;
 
namespace xeus
{

   

    class xaresponse_sender
    {
        public:
        xaresponse_sender() = default;
        //inline 
        void operator()(nl::json msg)
        {
            if(m_post_send == nullptr)
            {
                std::cout<<" sender is not set "<<std::endl;
                throw std::runtime_error("sender is not set");
            }
           
            std::cout<<" in xaresponse_sender sending reply "<<std::endl;
            m_post_send(std::move(msg), m_metadata);

        }
        
        nl::json m_metadata;
        std::function<void(nl::json, nl::json)> m_post_send = nullptr;
    };

} // namespace xeus

#endif