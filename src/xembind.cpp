/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_XEMBIND_HPP
#define XEUS_XEMBIND_HPP

#include <string>
#include <iostream>

#include "xeus/xserver_emscripten.hpp"
#include "xeus/xkernel.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus/xmessage.hpp"

namespace nl = nlohmann;
namespace ems = emscripten;

namespace xeus
{
    nl::json json_from_js_object(ems::val js_obj)
    {
        ems::val js_json_lib = ems::val::global("JSON");
        const std::string json_str = js_json_lib.call<std::string>("stringify",js_obj);
        return nl::json::parse(json_str);      
    }

    // "transport" binary buffers from JS to C++
    void buffer_sequence_from_js_buffer(buffer_sequence& self, ems::val buffers)
    {
        const unsigned n_buffers = buffers["length"].as<unsigned>();
        self.resize(n_buffers);

        std::vector<ems::val> buffers_vec = ems::vecFromJSArray<ems::val>(buffers);
        for(std::size_t i=0; i<buffers_vec.size(); ++i)
        {
            // the typed array an be of any type
            ems::val js_array = buffers_vec[i];

            // data we need to convert js_array into an js Uint8Arra
            ems::val js_array_buffer = js_array["buffer"].as<ems::val>();
            ems::val byteOffset = js_array["byteOffset"].as<ems::val>();
            const unsigned length = js_array["length"].as<unsigned> ();
            const unsigned bytes_per_element = js_array["BYTES_PER_ELEMENT"].as<unsigned>();
            const unsigned length_uint8 = length * bytes_per_element;

            // convert js typed-array into an  Uint8Array
            ems::val js_uint8array = ems::val::global("Uint8Array").new_(
                js_array_buffer, 
                byteOffset, 
                length * bytes_per_element
            );

            // resize array on c++ size
            self[i].resize(length_uint8);

            // copy values from js side to vectors mem
            // - create a javascript "UInt8(!) (not Int8) array"
            //   which is using the vector ptr as buffer
            ems::val heap = ems::val::module_property("HEAPU8");
            ems::val memory = heap["buffer"];
            ems::val memory_view = js_uint8array["constructor"].new_(memory, 
                reinterpret_cast<uintptr_t>(self[i].data()), 
                length_uint8);

            // - copy the js arrays content into the c++ arrays content
            memory_view.call<void>("set", js_uint8array);
        }
    }

    // "transport" binary buffers from C++ to JS
    ems::val js_buffer_array_from_buffer_sequence(const buffer_sequence & binary_buffers, bool copy)
    {
        ems::val js_buffer_array = ems::val::array();
        for(const auto & const_buffer : binary_buffers)
        {   
            auto & buffer = const_cast<binary_buffer & >(const_buffer);

            ems::val mem_view = ems::val(ems::typed_memory_view(buffer.size(), buffer.data()));
            if(copy)
            {
                ems::val mem_copy = ems::val::global("Int8Array").new_(mem_view);
                js_buffer_array.call<void>("push", mem_copy);
            }
            else
            {
                js_buffer_array.call<void>("push", mem_view);
            }
        }
        return js_buffer_array;
    }

    xmessage xmessage_from_js_message(ems::val js_message)
    {   
        // message base data
        // (can be moved into xmessage)
        xmessage_base_data message_base_data;

        // handle buffers
        ems::val js_buffers = js_message["buffers"].as<ems::val>();
        buffer_sequence_from_js_buffer(message_base_data.m_buffers, js_buffers);

        // the json part 
        // (and clear buffer to avoid costly serialization of  buffers)
        js_message.set("buffers", ems::val::array());
        const auto m = json_from_js_object(js_message);
        message_base_data.m_header        = m["header"];
        message_base_data.m_parent_header = m["parent_header"];
        message_base_data.m_metadata      = m["metadata"];
        message_base_data.m_content       = m["content"];
        
        return xmessage(xmessage::guid_list(), std::move(message_base_data));
    }

    ems::val js_message_from_xmessage(const xpub_message & message, bool copy)
    {
        // convert the json part to a string
        auto json_msg = nl::json::object();
        json_msg["header"] = message.header();
        json_msg["parent_header"] = message.parent_header();
        json_msg["metadata"] = message.metadata();
        json_msg["content"] = message.content();
        json_msg["topic"] = message.topic();
        const std::string json_str_msg = json_msg.dump();

        // parse string as js message
        ems::val js_json_lib = ems::val::global("JSON");
        ems::val js_msg = js_json_lib.call<ems::val>("parse", json_str_msg);

        // c++  buffers to js arrays
        ems::val js_buffer_array = js_buffer_array_from_buffer_sequence(message.buffers(), copy);
        js_msg.set("buffers", js_buffer_array);

        return js_msg;
    }

    ems::val js_message_from_xmessage(const xmessage & message, bool copy)
    {
        // convert the json part to a string
        auto json_msg = nl::json::object();
        json_msg["header"] = message.header();
        json_msg["parent_header"] = message.parent_header();
        json_msg["metadata"] = message.metadata();
        json_msg["content"] = message.content();
        const std::string json_str_msg = json_msg.dump();

        // parse string as js message
        ems::val js_json_lib = ems::val::global("JSON");
        ems::val js_msg  =  js_json_lib.call<ems::val>("parse", json_str_msg);

        // c++  buffers to js arrays
        ems::val js_buffer_array = js_buffer_array_from_buffer_sequence(message.buffers(), copy);
        js_msg.set("buffers", js_buffer_array);

        return js_msg;
    }

    //ems::val js_message_from_xmessage(const )
    void export_server_emscripten()
    {
        ems::class_<xserver>("xserver");

        ems::class_<xserver_emscripten, ems::base<xserver> >("xserver_emscripten")
            .function("notify_listener", &xserver_emscripten::js_notify_listener)
        ;
    }

    void export_core()
    {
        #ifdef XEUS_EMSCRIPTEN_WASM_TEST_BUILD
        ems::class_<buffer_sequence>("buffer_sequence")
            .constructor<>()
            .function("size", &buffer_sequence::size)
            .function("push_back", std::function<void(buffer_sequence&, const binary_buffer &)>([](buffer_sequence& self, const binary_buffer & b){
                self.push_back(b);
            }))

            .function("view", std::function<ems::val(buffer_sequence&)>([](buffer_sequence& self){
                return js_buffer_array_from_buffer_sequence(self, /*copy*/ false);
            }))
            .function("copy", std::function<ems::val(buffer_sequence&)>([](buffer_sequence& self){
                return js_buffer_array_from_buffer_sequence(self, /*copy*/ true);
            }))
            .function("from_js", std::function<void(buffer_sequence&, ems::val)>([](buffer_sequence& self, ems::val buffers){
                buffer_sequence_from_js_buffer(self, buffers);
            }), ems::allow_raw_pointers())
        ;
        #endif
        export_server_emscripten();
    }

    xeus::xserver * get_server(xeus::xkernel * kernel)
    {
        return &kernel->get_server();
    }
}

#endif

