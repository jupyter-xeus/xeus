############################################################################
# Copyright (c) 2016, Johan Mabille, Sylvain Corlay, Martin Renou          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.8)
project(xeus)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(XEUS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(XEUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(XEUS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

# Versionning
# ===========

# Project version
file(STRINGS "${XEUS_INCLUDE_DIR}/xeus/xeus.hpp" xeus_version_defines
     REGEX "#define XEUS_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xeus_version_defines})
    if(ver MATCHES "#define XEUS_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(XEUS_VERSION
    ${XEUS_VERSION_MAJOR}.${XEUS_VERSION_MINOR}.${XEUS_VERSION_PATCH})
message(STATUS "xeus version: v${XEUS_VERSION}")

# Binary version
# See the following URL for explanations about the binary versionning
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info
file(STRINGS "${XEUS_INCLUDE_DIR}/xeus/xeus.hpp" xeus_version_defines
    REGEX "#define XEUS_BINARY_(CURRENT|REVISION|AGE)")
foreach(ver ${xeus_version_defines})
    if(ver MATCHES "#define XEUS_BINARY_(CURRENT|REVISION|AGE) +([^ ]+)$")
        set(XEUS_BINARY_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(XEUS_BINARY_VERSION
    ${XEUS_BINARY_CURRENT}.${XEUS_BINARY_REVISION}.${XEUS_BINARY_AGE})
message(STATUS "xeus binary version: v${XEUS_BINARY_VERSION}")

# Build options
# =============

# Compilation options
option(XEUS_BUILD_SHARED_LIBS "Build xeus shared library." ON)
option(XEUS_BUILD_STATIC_LIBS "Build xeus static library (default if BUILD_SHARED_LIBS is OFF)." ON)
option(XEUS_STATIC_DEPENDENCIES "link statically with xeus dependencies" OFF)
option(XEUS_EMSCRIPTEN_WASM_BUILD  "build for wasm via emscripten" OFF)

# Test options
option(XEUS_BUILD_TESTS "xeus test suite" OFF)

# Emscripten wasm build configuration
# ===================================

if(XEUS_EMSCRIPTEN_WASM_BUILD)
    SET(XEUS_BUILD_SHARED_LIBS OFF)
    SET(XEUS_BUILD_STATIC_LIBS ON)
endif()

# Print build configuration
# ==========================

message(STATUS "XEUS_BUILD_SHARED_LIBS:          ${XEUS_BUILD_SHARED_LIBS}")
message(STATUS "XEUS_BUILD_STATIC_LIBS:          ${XEUS_BUILD_STATIC_LIBS}")
message(STATUS "XEUS_STATIC_DEPENDENCIES:        ${XEUS_STATIC_DEPENDENCIES}")  
message(STATUS "XEUS_EMSCRIPTEN_WASM_BUILD:      ${XEUS_EMSCRIPTEN_WASM_BUILD}")
message(STATUS "XEUS_BUILD_TESTS:                ${XEUS_BUILD_TESTS}")  

# Dependencies
# ============

set(nlohmann_json_REQUIRED_VERSION 3.2.0)
set(xtl_REQUIRED_VERSION 0.7)

if (NOT TARGET nlohmann_json)
    find_package(nlohmann_json ${nlohmann_json_REQUIRED_VERSION} REQUIRED)
endif ()

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()

# Source files
# ============

set(XEUS_HEADERS
    ${XEUS_INCLUDE_DIR}/xeus/xcomm.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xcontrol_messenger.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xdebugger.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xeus.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xeus_context.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xguid.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xhistory_manager.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xinput.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xinterpreter.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xkernel.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xkernel_configuration.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xlogger.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xmessage.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xhelper.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xserver.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xstring_utils.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xsystem.hpp
)

set(XEUS_SOURCES
    ${XEUS_SOURCE_DIR}/xcomm.cpp
    ${XEUS_SOURCE_DIR}/xcontrol_messenger.cpp
    ${XEUS_SOURCE_DIR}/xdebugger.cpp
    ${XEUS_SOURCE_DIR}/xguid.cpp
    ${XEUS_SOURCE_DIR}/xhistory_manager.cpp
    ${XEUS_SOURCE_DIR}/xinput.cpp
    ${XEUS_SOURCE_DIR}/xin_memory_history_manager.hpp
    ${XEUS_SOURCE_DIR}/xin_memory_history_manager.cpp
    ${XEUS_SOURCE_DIR}/xinterpreter.cpp
    ${XEUS_SOURCE_DIR}/xkernel.cpp
    ${XEUS_SOURCE_DIR}/xkernel_configuration.cpp
    ${XEUS_SOURCE_DIR}/xkernel_core.cpp
    ${XEUS_SOURCE_DIR}/xkernel_core.hpp
    ${XEUS_SOURCE_DIR}/xlogger.cpp
    ${XEUS_SOURCE_DIR}/xlogger_impl.hpp
    ${XEUS_SOURCE_DIR}/xlogger_impl.cpp
    ${XEUS_SOURCE_DIR}/xmessage.cpp
    ${XEUS_SOURCE_DIR}/xhelper.cpp
    ${XEUS_SOURCE_DIR}/xmock_interpreter.cpp
    ${XEUS_SOURCE_DIR}/xmock_interpreter.hpp
    ${XEUS_SOURCE_DIR}/xserver.cpp
    ${XEUS_SOURCE_DIR}/xsystem.cpp
)

# Targets and link
# ================

include(CheckCXXCompilerFlag)

include(cmake/CompilerWarnings.cmake)
include(WasmBuildOptions)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

macro(xeus_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    # Output
    # ======

    add_library(${target_name} ${linkage_upper} ${XEUS_SOURCES} ${XEUS_HEADERS})

    target_include_directories(
        ${target_name}
        PUBLIC $<BUILD_INTERFACE:${XEUS_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(
        ${target_name}
        PUBLIC nlohmann_json::nlohmann_json
        PUBLIC xtl
    )

    if (NOT MSVC AND NOT XEUS_EMSCRIPTEN_WASM_BUILD)
        if (APPLE)
            target_link_libraries(${target_name} PUBLIC "-framework CoreFoundation")
        else ()
            if (XEUS_STATIC_DEPENDENCIES)
                find_path(LIBUUID_INCLUDE_DIR uuid/uuid.h)
                find_library(LIBUUID_LIBRARY libuuid.a)
                target_include_directories(${target_name} PRIVATE ${LIBUUID_INCLUDE_DIR})
                target_link_libraries(${target_name} PRIVATE ${LIBUUID_LIBRARY}) 
            else ()
                find_package(LibUUID REQUIRED)
                target_link_libraries(${target_name} PRIVATE LibUUID::LibUUID)
            endif ()
        endif ()
    endif ()

    if (UNIX)
        # CMake does not compute the version number of so files as libtool
        # does on Linux. Strictly speaking, we should exclude FreeBSD and
        # Apple from this, but that would require having different version
        # numbers depending on the platform. We prefer to follow the
        # libtool pattern everywhere.
        math(EXPR XEUS_BINARY_COMPATIBLE "${XEUS_BINARY_CURRENT} - ${XEUS_BINARY_AGE}")
        set_target_properties(
            ${target_name}
            PROPERTIES
            PUBLIC_HEADER "${XEUS_HEADERS}"
            COMPILE_OPTIONS "-fvisibility=hidden"
            COMPILE_DEFINITIONS "XEUS_EXPORTS"
            PREFIX ""
            VERSION "${XEUS_BINARY_COMPATIBLE}.${XEUS_BINARY_REVISION}.${XEUS_BINARY_AGE}"
            SOVERSION ${XEUS_BINARY_COMPATIBLE}
            OUTPUT_NAME "lib${output_name}"
        )
    else()
        set_target_properties(
            ${target_name}
            PROPERTIES
            PUBLIC_HEADER "${XEUS_HEADERS}"
            COMPILE_DEFINITIONS "XEUS_EXPORTS"
            PREFIX ""
            VERSION ${XEUS_BINARY_VERSION}
            SOVERSION ${XEUS_BINARY_CURRENT}
            OUTPUT_NAME "lib${output_name}"
        )
    endif()

    # Compilation flags
    # =================

    target_compile_features(${target_name} PRIVATE cxx_std_14)
    xeus_target_add_compile_warnings(${target_name})

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")
        message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()

    if (${linkage_upper} STREQUAL "STATIC")
        target_compile_definitions(${target_name} PUBLIC XEUS_STATIC_LIB)
    endif ()

    if (MSVC)
        target_compile_definitions(${target_name} PUBLIC -DNOMINMAX)
        target_compile_options(${target_name} PUBLIC /DGUID_WINDOWS /MP /bigobj)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4996)
    elseif (APPLE)
        target_compile_definitions(${target_name} PUBLIC -DGUID_CFUUID)
    else ()
        target_compile_definitions(${target_name} PUBLIC -DGUID_LIBUUID)
    endif ()

    if (XEUS_STATIC_DEPENDENCIES)
        if (CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS})
        endif ()
        if (UNIX AND NOT APPLE)
            target_link_libraries(${target_name} PRIVATE util rt)
        endif ()
    endif ()

    if (XEUS_EMSCRIPTEN_WASM_BUILD)
       target_compile_definitions(${target_name} PRIVATE XEUS_EMSCRIPTEN_WASM_BUILD)
       xeus_wasm_compile_options(${target_name})
    endif()

    if (MSVC)
        message(STATUS "XEUS Linkage: ${linkage}")
        message(STATUS "CXX flags: ${CMAKE_CXX_FLAGS}
        ${CMAKE_CXX_FLAGS_RELEASE}")
    endif()
endmacro()

set(xeus_targets "")

if (XEUS_BUILD_SHARED_LIBS)
    xeus_create_target(xeus SHARED xeus)
    list(APPEND xeus_targets xeus)
endif ()

if (XEUS_BUILD_STATIC_LIBS)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xeus_create_target(xeus-static STATIC xeus-static)
    else ()
        xeus_create_target(xeus-static STATIC xeus)
    endif ()

    list(APPEND xeus_targets xeus-static)
endif ()

# Tests
# =====

# We need to control from outside whether we enable testing or not. We cannot
# rely on BUILD_TESTING since it doe snot exist until CTest is included.

include(CTest)

if(XEUS_BUILD_TESTS)
    set(BUILD_TESTING ON)
    message(STATUS "tests enabled")
else ()
    set(BUILD_TESTING OFF)
    message(STATUS "tests disabled")
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

# Installation
# ============

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(XEUS_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeusConfig.cmake")

install(TARGETS ${xeus_targets}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus)

# Makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}-targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

# Configure 'xeusConfig.cmake' for a build tree
set(XEUS_CONFIG_CODE "####### Expanded from \@XEUS_CONFIG_CODE\@ #######\n")
set(XEUS_CONFIG_CODE "${XEUS_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_CONFIG_CODE "${XEUS_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeusConfig.cmake' for an install tree
set(XEUS_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})


write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${XEUS_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindLibUUID.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WasmBuildOptions.cmake
              ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake
              DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})
