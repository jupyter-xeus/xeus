############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

if(MSVC)
    set(libxeus_runtime_file ${CMAKE_BINARY_DIR}/${XEUS_TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
    set(libxeus_runtime_file
        "${CMAKE_BINARY_DIR}/lib${XEUS_TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

add_subdirectory(echo_client)
add_subdirectory(echo_kernel)
