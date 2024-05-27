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
#include "xeus/xjson.hpp" 
#include "xeus/xmessage.hpp" // for xmessage::guid_list

namespace nl = nlohmann;

namespace xeus
{
    class XEUS_API xrequest_context
    {
    public:
    
        using guid_list = xmessage::guid_list;
            
        xrequest_context() = default;
        xrequest_context(nl::json header, guid_list id);    
        
        const nl::json& header() const; 
        const guid_list& id() const;

    private:
            
        nl::json m_header = nl::json::object();
        guid_list m_id;
    };
}

#endif
