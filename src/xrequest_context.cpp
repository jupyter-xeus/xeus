#include "xeus/xrequest_context.hpp"

namespace xeus
{
    xrequest_context::xrequest_context(nl::json header, guid_list id)
        : m_header(std::move(header)), m_id(std::move(id))
    {
    }

    const nl::json& xrequest_context::header() const
    {
        return m_header;
    }

    const xmessage::guid_list& xrequest_context::id() const
    {
        return m_id;
    }
}
