############################################################################
# Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

# Module to set compiler and linker options for WASM builds

function(xeus_wasm_compile_options target)
     target_compile_options("${target}"
        PUBLIC --std=c++17
        PUBLIC -Wno-deprecated
        PUBLIC "SHELL: -s USE_PTHREADS=0"
        PUBLIC "SHELL: -fwasm-exceptions"
    )
endfunction()

function(xeus_wasm_link_options target environment)
    target_link_options("${target}"
        PUBLIC --bind
        PUBLIC -Wno-unused-command-line-argument
        PUBLIC "SHELL: -fwasm-exceptions"
        PUBLIC "SHELL: -s MODULARIZE=1"
        PUBLIC "SHELL: -s EXPORT_NAME=\"createXeusModule\""
        PUBLIC "SHELL: -s EXPORT_ES6=0"
        PUBLIC "SHELL: -s USE_ES6_IMPORT_META=0"
        PUBLIC "SHELL: -s DEMANGLE_SUPPORT=0"
        PUBLIC "SHELL: -s ASSERTIONS=0"
        PUBLIC "SHELL: -s ALLOW_MEMORY_GROWTH=1"
        PUBLIC "SHELL: -s EXIT_RUNTIME=1"
        PUBLIC "SHELL: -s WASM=1"
        PUBLIC "SHELL: -s USE_PTHREADS=0"
        PUBLIC "SHELL: -s ENVIRONMENT=${environment}"
        PUBLIC "SHELL: -s TOTAL_STACK=32mb"
        PUBLIC "SHELL: -s INITIAL_MEMORY=128mb"
    )
endfunction()

function(xeus_wasm_fs_options target)
    target_link_options("${target}"
        PUBLIC "SHELL: -s FORCE_FILESYSTEM"
    )
endfunction()

function(xeus_wasm_async_options target)
    target_link_options("${target}"
        INTERFACE "SHELL: -s ASYNCIFY=1"
        INTERFACE "SHELL: -s 'ASYNCIFY_IMPORTS=[\"get_stdin\"]'"
        INTERFACE "SHELL: -s 'ASYNCIFY_STACK_SIZE=100000'"
    )
endfunction()
