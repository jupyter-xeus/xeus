############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

set(ECHO_KERNEL_SOURCES
    echo_interpreter.cpp
    echo_interpreter.hpp
    main.cpp)

add_executable(echo_kernel ${ECHO_KERNEL_SOURCES})
target_link_libraries(echo_kernel xeus "-lpthread")

target_compile_features(echo_kernel PRIVATE cxx_std_11)

set(CONNECTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/connection.json)

add_custom_command(TARGET echo_kernel POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E
                   copy "${CONNECTION_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_command(TARGET echo_kernel POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E
                   copy "${CMAKE_BINARY_DIR}/${XEUS_RUNTIME_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/")
