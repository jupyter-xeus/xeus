/***************************************************************************
* Copyright (c) 2024, Dr. Thorsten Beier                                   
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_REQUEST_CONTEXT_HPP
#define XEUS_REQUEST_CONTEXT_HPP

#include <string>
#include <vector>

#include "xeus/xeus.hpp"
#include "nlohmann/json.hpp" 
#include "xeus/xmessage.hpp" // for xmessage::guid_list
#include "xeus/xserver.hpp"  // for channel

namespace nl = nlohmann;

namespace xeus
{

    class XEUS_API xrequest_context
    {
        public:
            using guid_list = xmessage::guid_list;

            xrequest_context(nl::json header, channel origin, guid_list id);    
            const nl::json& header() const; 
            channel origin() const;
            const guid_list& id() const;
        private:
            
            nl::json m_header;
            channel m_origin;
            guid_list m_id;
        
    };
}

#endif
