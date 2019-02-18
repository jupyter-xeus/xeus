############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

if (NOT TARGET xeus AND NOT TARGET xeus_static)
    cmake_minimum_required(VERSION 3.8)
    project(xeus-echo-example)

    find_package(xeus CONFIG REQUIRED)
endif ()

if (TARGET xeus)
    set(xeus_TARGET xeus)
    message("Found xeus shared library.")
elseif (TARGET xeus_static)
    set(xeus_TARGET xeus_static)
    message("Found xeus static library.")
endif ()

add_subdirectory(echo_client)
add_subdirectory(echo_kernel)
