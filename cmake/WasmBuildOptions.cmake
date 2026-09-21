############################################################################
# Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "Emscripten is required for WebAssembly build")
endif()

if (EMSCRIPTEN_VERSION VERSION_LESS "4.0.0")
   message(FATAL_ERROR "Emscripten version must be >= 4.0.0")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Targeting wasm64")
    SET(MAX_MEM "16GB")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "Targeting wasm32")
    SET(MAX_MEM "4GB")
else()
    message(WARNING "Unknown WebAssembly pointer size")
endif()

message(STATUS "Emscripten version >= 4.0.0")
function(xeus_wasm_compile_options target)
    target_compile_options("${target}"
        PUBLIC --std=c++17
        PUBLIC -Wno-deprecated
        PUBLIC "SHELL: -fwasm-exceptions"
    )
    set_property(TARGET ${target} PROPERTY POSITION_INDEPENDENT_CODE ON)
endfunction()

function(xeus_wasm_link_options target environment)
    target_link_options("${target}"
        PUBLIC -Wno-unused-command-line-argument
        PUBLIC "SHELL: -lembind"
        PUBLIC "SHELL: -fwasm-exceptions"
        PUBLIC "SHELL: -s MODULARIZE=1"
        PUBLIC "SHELL: -s EXPORT_NAME=\"createXeusModule\""
        PUBLIC "SHELL: -s EXPORT_ES6=0"
        PUBLIC "SHELL: -s ASSERTIONS=0"
        PUBLIC "SHELL: -s ALLOW_MEMORY_GROWTH=1"
        PUBLIC "SHELL: -s EXIT_RUNTIME=1"
        PUBLIC "SHELL: -s WASM=1"
        PUBLIC "SHELL: -s ENVIRONMENT=${environment}"
        PUBLIC "SHELL: -s STACK_SIZE=32mb"
        PUBLIC "SHELL: -s INITIAL_MEMORY=64MB"
        PUBLIC "SHELL: -s MAXIMUM_MEMORY=${MAX_MEM}"
        PUBLIC "SHELL: -s EXPORTED_RUNTIME_METHODS='[\"FS\",\"ENV\",\"PATH\",\"ERRNO_CODES\",\"getExceptionMessage\",\"getCppExceptionTag\"]'"
        PUBLIC "SHELL: -s FORCE_FILESYSTEM"
        PUBLIC "SHELL: -s MAIN_MODULE=1"
    )
endfunction()
