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

#include "xeus/xeus.hpp"

namespace xeus
{

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

}

#endif
