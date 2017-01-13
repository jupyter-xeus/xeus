############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

set(libxeus_runtime_file ${CMAKE_BINARY_DIR}/${XEUS_TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX})

add_subdirectory(echo_client)
add_subdirectory(echo_kernel)
