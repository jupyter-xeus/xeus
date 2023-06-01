/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          *
* Copyright (c) 2016, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_CONTEXT_HPP
#define XEUS_CONTEXT_HPP

#include <memory>

#include "xeus.hpp"

namespace xeus
{

    struct [[deprecated]] xempty_context_tag
    {
    };

    template <class T>
    class xcontext_impl;

    class XEUS_API xcontext
    {
    public:

        virtual ~xcontext() = default;

        xcontext(const xcontext&) = delete;
        xcontext& operator=(const xcontext&) = delete;

        xcontext(xcontext&&) = delete;
        xcontext& operator=(xcontext&&) = delete;

        template <class T>
        T& get_wrapped_context();

    protected:

        xcontext() = default;
    };

    template <class T>
    class xcontext_impl : public xcontext
    {

    public:

        template <class... U>
        xcontext_impl(U&&... u)
            : m_context(std::forward<U>(u)...)
        {
        }

        virtual ~xcontext_impl() = default;

        T m_context;
    };

    template <class T>
    T& xcontext::get_wrapped_context()
    {
        auto* impl = static_cast<xcontext_impl<T>*>(this);
        return impl->m_context;
    }

    // Deprecated: call the make_xxx_context function from the library providing the
    // server implementation
    template <class T, class... U>
    std::unique_ptr<xcontext_impl<T>> make_context(U&&... u)
    {
        return std::unique_ptr<xcontext_impl<T>>(new xcontext_impl<T>(std::forward<U>(u)...));
    }

    [[deprecated]]
    inline std::unique_ptr<xcontext_impl<xempty_context_tag>> make_empty_context()
    {
        return std::unique_ptr<xcontext_impl<xempty_context_tag>>(
            new xcontext_impl<xempty_context_tag>()
        );
    }
}

#endif
