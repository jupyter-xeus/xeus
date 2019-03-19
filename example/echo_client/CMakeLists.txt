############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

find_package(Threads REQUIRED)

set(ECHO_CLIENT_SOURCES
    echo_client.cpp
    echo_client.hpp
    main.cpp)

add_executable(echo_client ${ECHO_CLIENT_SOURCES})
target_link_libraries(echo_client ${xeus_TARGET} Threads::Threads)

target_compile_features(echo_client PRIVATE cxx_std_11)

set(CONNECTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../echo_kernel/connection.json)

add_custom_command(TARGET echo_client POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E
                   copy "${CONNECTION_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_command(TARGET echo_client POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E
                   copy "${CMAKE_BINARY_DIR}/${XEUS_RUNTIME_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/")
