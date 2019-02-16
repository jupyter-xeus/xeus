/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_EXPORT_HPP
#define XEUS_EXPORT_HPP

#ifdef _WIN32
    #ifdef XEUS_STATIC_LIB
        #define XEUS_API
    #else
        #ifdef XEUS_EXPORTS
            #define XEUS_API __declspec(dllexport)
        #else
            #define XEUS_API __declspec(dllimport)
        #endif
    #endif
#else
    #define XEUS_API
#endif

// Project version
#define XEUS_VERSION_MAJOR 0
#define XEUS_VERSION_MINOR 18
#define XEUS_VERSION_PATCH 1

// Binary version
#define XEUS_BINARY_CURRENT 1
#define XEUS_BINARY_REVISION 0
#define XEUS_BINARY_AGE 1

// Kernel protocol version
#define XEUS_KERNEL_PROTOCOL_VERSION_MAJOR 5
#define XEUS_KERNEL_PROTOCOL_VERSION_MINOR 1

// Composing the protocol version string from major, and minor
#define XEUS_CONCATENATE(A, B) XEUS_CONCATENATE_IMPL(A, B)
#define XEUS_CONCATENATE_IMPL(A, B) A##B
#define XEUS_STRINGIFY(a) XEUS_STRINGIFY_IMPL(a)
#define XEUS_STRINGIFY_IMPL(a) #a

#define XEUS_KERNEL_PROTOCOL_VERSION XEUS_STRINGIFY(XEUS_CONCATENATE(    XEUS_KERNEL_PROTOCOL_VERSION_MAJOR,\
                                                    XEUS_CONCATENATE(.,  XEUS_KERNEL_PROTOCOL_VERSION_MINOR)))

#endif
