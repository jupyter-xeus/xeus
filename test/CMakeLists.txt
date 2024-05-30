############################################################################
# Copyright (c) 2016, Sylvain Corlay, Johan Mabille, Martin Renou          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

# Unit tests
# ==========
cmake_minimum_required(VERSION 3.8)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(xeus-test)

    enable_testing()

    find_package(xeus REQUIRED CONFIG)
    find_package(nlohmann_json QUIET CONFIG)

    set(XEUS_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
endif ()

message(STATUS "Forcing tests build type to Release")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

include(CheckCXXCompilerFlag)

if(nlohmann_json_FOUND)
  add_definitions(-DHAVE_NLOHMANN_JSON)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU OR CMAKE_CXX_COMPILER_ID MATCHES Intel)
    add_compile_options(-Wunused-parameter -Wextra -Wreorder -Wconversion -Wsign-conversion)

    CHECK_CXX_COMPILER_FLAG(-march=native HAS_MARCH_NATIVE)
    if (HAS_MARCH_NATIVE)
        add_compile_options(-march=native)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_compile_options(/EHsc /MP /bigobj)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

find_package(doctest REQUIRED)
find_package(Threads)

set(XEUS_TESTS
    test_xbase64.cpp
    test_xbasic_fixed_string.cpp
    test_xhash.cpp
    test_xin_memory_history_manager.cpp
    test_xsystem.cpp
    test_unit_kernel.cpp
)

if(nlohmann_json_FOUND)
    # Version up to 3.1.2 export the target `nlohmann_json`
    if(TARGET nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json)
    # Newer versions export the namespaced target `nlohmann_json::nlohmann_json`
    elseif(TARGET nlohmann_json::nlohmann_json)
      set(nlohmann_json_TARGET nlohmann_json::nlohmann_json)
    endif()
endif()

if (TARGET xeus)
    set(xeus_TARGET xeus)
    message("Found xeus shared library.")
elseif (TARGET xeus-static)
    set(xeus_TARGET xeus-static)
    message("Found xeus static library.")
endif ()

set(XEUS_TEST_SRCS
    xmock_interpreter.hpp
    xmock_interpreter.cpp
    xmock_server.hpp
    xmock_server.cpp)

foreach(filename IN LISTS XEUS_TESTS)
    get_filename_component(targetname ${filename} NAME_WE)

    add_executable(${targetname} test_main.cpp ${filename} ${XEUS_TEST_SRCS})
    target_compile_definitions(${targetname} PRIVATE DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING)
    target_link_libraries(${targetname} PRIVATE ${xeus_TARGET} doctest::doctest Threads::Threads ${nlohmann_json_TARGET})
    add_test(NAME ${targetname} COMMAND ${targetname})
endforeach()
