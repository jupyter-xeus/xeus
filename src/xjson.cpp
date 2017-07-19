/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xjson.hpp"

namespace xeus
{
    xjson::xjson()
    {
        init_root();
    }

    xjson::xjson(allocator_type& alloc)
        : m_document(&alloc)
    {
    }

    xjson::xjson(document_type&& document)
        : m_document(std::move(document))
    {
    }

    xjson xjson::copy() const
    {
        document_type::AllocatorType& allocator = const_cast<document_type&>(m_document).GetAllocator();
        document_type res;
        res.CopyFrom(m_document, allocator);
        return xjson(std::move(res));
    }

    xjson::allocator_type& xjson::get_allocator()
    {
        return m_document.GetAllocator();
    }

    void xjson::parse(const char* buffer, std::size_t length)
    {
        m_document.Parse(buffer, length);
    }

    void xjson::init_root()
    {
        if (!m_document.IsObject())
        {
            m_document.SetObject();
        }
    }
}
