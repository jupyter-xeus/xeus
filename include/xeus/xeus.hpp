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
    #ifdef XEUS_EXPORTS
        #define XEUS_API __declspec(dllexport)
    #else
        #define XEUS_API __declspec(dllimport)
    #endif
#else
    #define XEUS_API
#endif

#define XEUS_VERSION_MAJOR 0
#define XEUS_VERSION_MINOR 7
#define XEUS_VERSION_PATCH 0

#endif
