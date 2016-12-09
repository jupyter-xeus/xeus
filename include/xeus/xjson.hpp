/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XJSON_HPP
#define XJSON_HPP

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <cstddef>
#include "xeus_export.hpp"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/writer.h"

namespace xeus
{

    class XEUS_API xjson
    {

    public:

        using document_type = rapidjson::Document;
        using node_type = document_type::ValueType;

        xjson() = default;
        xjson(document_type&& document);
        ~xjson() = default;

        xjson(xjson&&) = default;
        xjson& operator=(xjson&&) = default;

        // Avoids copy and forces to use move semantics ...
        xjson(const xjson&) = delete;
        xjson& operator=(const xjson&) = delete;

        // ... unless it is absolutely what we want
        xjson copy() const;

        template <class char_type, std::size_t N>
        bool get_bool(const char_type(&name)[N]) const;

        template <class char_type, std::size_t N>
        bool get_bool(const char_type(&name)[N], bool default_value) const;

        template <class char_type, std::size_t N>
        int get_int(const char_type(&name)[N]) const;

        template <class char_type, std::size_t N>
        int get_int(const char_type(&name)[N], int default_value) const;

        template <class char_type, std::size_t N>
        std::string get_string(const char_type(&name)[N]) const;

        template <class char_type, std::size_t N>
        std::string get_string(const char_type(&name)[N], const std::string& default_value) const;

        template <class char_type, std::size_t N>
        const node_type* get_node(const char_type(&name)[N]) const;

        template <class char_type, std::size_t N, class T>
        void set_value(const char_type(&name)[N], T value);

        template <class char_type, std::size_t N>
        void set_value(const char_type(&name)[N], const std::string& value);

        void parse(const char* buffer, std::size_t length);
        
        template <class stream>
        void parse(stream& input);

        template <class stream>
        void write(stream& output) const;

    private:

        template <class char_type, std::size_t N>
        const node_type* get_bool_impl(const char_type(&name)[N]) const;

        template <class char_type, std::size_t N>
        const node_type* get_int_impl(const char_type(&name)[N]) const;

        template <class char_type, std::size_t N>
        const node_type* get_string_impl(const char_type(&name)[N]) const;

        rapidjson::Document m_document;
    };

    template <class char_type, std::size_t N>
    inline bool xjson::get_bool(const char_type(&name)[N]) const
    {
        const node_type* node = get_bool_impl(name);
        if (node == nullptr)
        {
            throw std::runtime_error("Bool attribute not found in json");
        }
        return node->GetBool();
    }

    template <class char_type, std::size_t N>
    inline bool xjson::get_bool(const char_type(&name)[N], bool default_value) const
    {
        const node_type* node = get_bool_impl(name);
        return node != nullptr ? node->GetBool() : default_value;
    }

    template <class char_type, std::size_t N>
    inline int xjson::get_int(const char_type(&name)[N]) const
    {
        const node_type* node = get_int_impl(name);
        if (node == nullptr)
        {
            throw std::runtime_error("Int attribute not found in json");
        }
        return node->GetInt();
    }

    template <class char_type, std::size_t N>
    inline int xjson::get_int(const char_type(&name)[N], int default_value) const
    {
        const node_type* node = get_int_impl(name);
        return node != nullptr ? node->GetInt() : default_value;
    }

    template <class char_type, std::size_t N>
    inline std::string xjson::get_string(const char_type(&name)[N]) const
    {
        const node_type* node = get_string_impl(name);
        if (node == nullptr)
        {
            throw std::runtime_error("String attribute not found in json");
        }
        return std::string(node->GetString(), node->GetStringLength());
    }

    template <class char_type, std::size_t N>
    inline std::string xjson::get_string(const char_type(&name)[N], const std::string& default_value) const
    {
        const node_type* node = get_string_impl(name);
        return node != nullptr ? std::string(node->GetString(), node->GetStringLength()) : std::move(default_value);
    }

    template <class char_type, std::size_t N>
    inline auto xjson::get_node(const char_type(&name)[N]) const -> const node_type*
    {
        return rapidjson::Pointer(name).Get(m_document);
    }

    template <class char_type, std::size_t N, class T>
    inline void xjson::set_value(const char_type(&name)[N], T value)
    {
        rapidjson::SetValueByPointer(m_document, name, value);
    }

    template <class char_type, std::size_t N>
    inline void xjson::set_value(const char_type(&name)[N], const std::string& value)
    {
        rapidjson::SetValueByPointer(m_document, name, value);
    }

    template <class stream>
    inline void xjson::parse(stream& input)
    {
        m_document.ParseStream(input);
    }

    template <class stream>
    inline void xjson::write(stream& output) const
    {
        rapidjson::Writer<stream> writer(output);
        m_document.Accept(writer);
    }

    template <class char_type, std::size_t N>
    inline auto xjson::get_bool_impl(const char_type(&name)[N]) const -> const node_type*
    {
        const node_type* node = rapidjson::GetValueByPointer(m_document, name);
        if (node != nullptr && !node->IsBool())
        {
            throw std::runtime_error("Wrong type in JSON, expected bool");
        }
        return node;
    }

    template <class char_type, std::size_t N>
    inline auto xjson::get_int_impl(const char_type(&name)[N]) const -> const node_type*
    {
        const node_type* node = rapidjson::GetValueByPointer(m_document, name);
        if(node != nullptr && !node->IsInt())
        {
            throw std::runtime_error("Wrong type in JSON, expected int");
        };
        return node;
    }

    template <class char_type, std::size_t N>
    inline auto xjson::get_string_impl(const char_type(&name)[N]) const -> const node_type*
    {
        const node_type* node = rapidjson::GetValueByPointer(m_document, name);
        if (node != nullptr && !node->IsString())
        {
            throw std::runtime_error("Wrong type in JSON, expected string");
        }
        return node;
    }

}

#endif
