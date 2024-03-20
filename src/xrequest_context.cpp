#include "xeus/xrequest_context.hpp"

namespace xeus
{
    xrequest_context::xrequest_context(nl::json header, channel origin, guid_list id)
        : m_header(std::move(header)), m_origin(origin), m_id(std::move(id))
    {
    }

    const nl::json& xrequest_context::header() const
    {
        return m_header;
    }

    channel xrequest_context::origin() const
    {
        return m_origin;
    }

    const xmessage::guid_list& xrequest_context::id() const
    {
        return m_id;
    }


    void xexecute_request_context::send_reply(nl::json reply)
    {
        m_on_send_reply(*this, std::move(reply));
    }


    xexecute_request_context::xexecute_request_context(nl::json header, 
                                                       channel origin, 
                                                       guid_list id, 
                                                       std::function<void(const xexecute_request_context&,nl::json)> on_send_reply)
        : xrequest_context(std::move(header), 
          origin, 
          std::move(id)), 
          m_on_send_reply(std::move(on_send_reply))
    {
    }

    // get and set callback
    void xexecute_request_context::set_on_send_callback(std::function<void(const xexecute_request_context&,nl::json)> augment_reply)
    {
        m_on_send_reply = std::move(augment_reply);
    }

    // get callback
    std::function<void(const xexecute_request_context&, nl::json)> xexecute_request_context::get_on_send_callback() const
    {
        return m_on_send_reply;
    }
}