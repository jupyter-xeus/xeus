/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xmock_server.hpp"

namespace xeus
{

    xmock_messenger::xmock_messenger(xmock_server* server)
        : p_server(server)
    {
    }

    nl::json xmock_messenger::send_to_shell_impl(const nl::json& message)
    {
        return p_server->notify_internal_listener(message);
    }

    xmock_server::xmock_server()
        : m_messenger(this)
    {
    }

    void xmock_server::receive_shell(xmessage message)
    {
        m_shell_messages.push(std::move(message));
    }

    void xmock_server::receive_control(xmessage message)
    {
        m_control_messages.push(std::move(message));
    }

    void xmock_server::receive_stdin(xmessage message)
    {
        m_stdin_messages.push(std::move(message));
    }

    std::size_t xmock_server::shell_size() const
    {
        return m_shell_messages.size();
    }

    xmessage xmock_server::read_shell()
    {
        return read_impl(m_shell_messages);
    }

    std::size_t xmock_server::control_size() const
    {
        return m_control_messages.size();
    }

    xmessage xmock_server::read_control()
    {
        return read_impl(m_control_messages);
    }

    std::size_t xmock_server::stdin_size() const
    {
        return m_stdin_messages.size();
    }

    xmessage xmock_server::read_stdin()
    {
        return read_impl(m_stdin_messages);
    }

    std::size_t xmock_server::iopub_size() const
    {
        return m_iopub_messages.size();
    }

    xpub_message xmock_server::read_iopub()
    {
        xpub_message res = std::move(m_iopub_messages.back());
        m_iopub_messages.pop();
        return res;
    }

    xmessage xmock_server::read_impl(message_queue& q)
    {
        xmessage res = std::move(q.back());
        q.pop();
        return res;
    }

    xcontrol_messenger& xmock_server::get_control_messenger_impl()
    {
        return m_messenger;
    }

    void xmock_server::send_shell_impl(xmessage message)
    {
        m_shell_messages.push(std::move(message));
    }

    void xmock_server::send_control_impl(xmessage message)
    {
        m_control_messages.push(std::move(message));
    }

    void xmock_server::send_stdin_impl(xmessage message)
    {
        m_stdin_messages.push(std::move(message));
    }

    void xmock_server::publish_impl(xpub_message message, channel)
    {
        m_iopub_messages.push(std::move(message));
    }

    void xmock_server::start_impl(xpub_message message)
    {
        m_iopub_messages.push(std::move(message));
    }

    void xmock_server::abort_queue_impl(const listener&, long)
    {
    }

    void xmock_server::stop_impl()
    {
    }

    void xmock_server::update_config_impl(xconfiguration&) const
    {
    }

    std::unique_ptr<xserver> make_mock_server(xcontext&,
                                            const xconfiguration&,
                                            nl::json::error_handler_t)
    {
        return std::make_unique<xmock_server>();
    }

    xmock_context::xmock_context() : xcontext()
    {
    }

    std::unique_ptr<xcontext> make_mock_context()
    {
        return std::make_unique<xmock_context>();
    }

} // namespace xeus
