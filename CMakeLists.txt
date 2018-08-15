############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
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


# Dependencies
# ============

# Dependencies that provide cmake project config

find_package(nlohmann_json 3.1.1 REQUIRED)
find_package(xtl 0.4 REQUIRED)
find_package(cppzmq 4.2.3 REQUIRED)
find_package(cryptopp REQUIRED)

# Dependencies built with autotools

if (WIN32)
    find_package(zeromq 4.2.3 REQUIRED)
else()
    find_package(PkgConfig)
    pkg_check_modules(ZeroMQ libzmq>=4.2.3 REQUIRED)
endif()

# Source files
# ============

set(XEUS_HEADERS
    ${XEUS_INCLUDE_DIR}/xeus/xauthentication.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xcomm.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xeus.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xguid.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xinterpreter.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xjson.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xkernel.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xkernel_configuration.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xmessage.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xserver.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xserver_zmq.hpp
)

set(XEUS_SOURCES
    ${XEUS_SOURCE_DIR}/xauthentication.cpp
    ${XEUS_SOURCE_DIR}/xcomm.cpp
    ${XEUS_SOURCE_DIR}/xguid.cpp
    ${XEUS_SOURCE_DIR}/xheartbeat.cpp
    ${XEUS_SOURCE_DIR}/xheartbeat.hpp
    ${XEUS_SOURCE_DIR}/xinterpreter.cpp
    ${XEUS_SOURCE_DIR}/xkernel.cpp
    ${XEUS_SOURCE_DIR}/xkernel_configuration.cpp
    ${XEUS_SOURCE_DIR}/xkernel_core.cpp
    ${XEUS_SOURCE_DIR}/xkernel_core.hpp
    ${XEUS_SOURCE_DIR}/xmessage.cpp
    ${XEUS_SOURCE_DIR}/xmock_interpreter.cpp
    ${XEUS_SOURCE_DIR}/xmock_interpreter.hpp
    ${XEUS_SOURCE_DIR}/xmiddleware.cpp
    ${XEUS_SOURCE_DIR}/xmiddleware.hpp
    ${XEUS_SOURCE_DIR}/xpublisher.cpp
    ${XEUS_SOURCE_DIR}/xpublisher.hpp
    ${XEUS_SOURCE_DIR}/xserver.cpp
    ${XEUS_SOURCE_DIR}/xserver_zmq.cpp
    ${XEUS_SOURCE_DIR}/xstring_utils.hpp
)

# Output
# ======

add_library(xeus SHARED ${XEUS_SOURCES} ${XEUS_HEADERS})

if (APPLE)
    set_target_properties(xeus PROPERTIES
        MACOSX_RPATH ON
    )
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
else()
    set_target_properties(xeus PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
    )
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

target_include_directories(xeus PUBLIC $<BUILD_INTERFACE:${XEUS_INCLUDE_DIR}>
                                       $<INSTALL_INTERFACE:include>)
target_link_libraries(xeus
                      PUBLIC cppzmq
                      PUBLIC nlohmann_json
                      PUBLIC xtl
                      PRIVATE cryptopp-static)

if(NOT MSVC)
    if(APPLE)
        target_link_libraries(xeus PUBLIC "-framework CoreFoundation")
    else()
        find_package(LibUUID REQUIRED)
        target_link_libraries(xeus PUBLIC LibUUID::LibUUID)
    endif()
endif()

set_target_properties(xeus PROPERTIES
                      PUBLIC_HEADER "${XEUS_HEADERS}"
                      COMPILE_DEFINITIONS "XEUS_EXPORTS"
                      PREFIX ""
                      VERSION ${XEUS_BINARY_VERSION}
                      SOVERSION ${XEUS_BINARY_CURRENT}
                      OUTPUT_NAME "libxeus")

# Configure Checks
# ================

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

cmake_push_check_state()

# HAVE_CRYPTOPP_BYTE_T
get_target_property(cryptopp_INCLUDE_DIR cryptopp-static INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(cryptopp_LIBRARY cryptopp-static LOCATION)
set(CMAKE_REQUIRED_LIBRARIES ${cryptopp_LIBRARY})
set(CMAKE_REQUIRED_INCLUDES ${cryptopp_INCLUDE_DIR})
check_cxx_source_compiles("
#include \"cryptopp/config.h\"
int main(){
  [[maybe_unused]] CryptoPP::byte b = CryptoPP::byte{0x36};
}" HAVE_CRYPTOPP_BYTE_T)

cmake_pop_check_state()

if(HAVE_CRYPTOPP_BYTE_T)
  target_compile_definitions(xeus PRIVATE HAVE_CRYPTOPP_BYTE_T)
endif()

# Compilation flags
# =================

include(CheckCXXCompilerFlag)
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)
OPTION(DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)

target_compile_features(xeus PRIVATE cxx_std_11)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    target_compile_options(xeus PUBLIC -Wunused-parameter -Wextra -Wreorder)
    if (DISABLE_ARCH_NATIVE)
        target_compile_options(xeus PUBLIC -mtune=generic)
    else()
        target_compile_options(xeus PUBLIC -march=native)
    endif()

    # Enable link time optimization and set the default symbol
    # visibility to hidden (very important to obtain small binaries)
    if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        # Check for Link Time Optimization support
        # (GCC/Clang)
        CHECK_CXX_COMPILER_FLAG("-flto" HAS_LTO_FLAG)
        if (HAS_LTO_FLAG)
            target_compile_options(xeus PUBLIC -flto)
        endif()

        # Intel equivalent to LTO is called IPO
        if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
            CHECK_CXX_COMPILER_FLAG("-ipo" HAS_IPO_FLAG)
            if (HAS_IPO_FLAG)
                target_compile_options(xeus PUBLIC -ipo)
            endif()
        endif()
    endif()
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif()

if(MSVC)
    target_compile_definitions(xeus PUBLIC -DNOMINMAX)
    target_compile_options(xeus PUBLIC /DGUID_WINDOWS /MP /bigobj)
    target_compile_options(xeus PUBLIC /wd4251 /wd4996)
    # Allows to compile in debug without requiring each dependencies to
    # be compiled in debug
    if(${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        set(CMAKE_CXX_FLAGS_DEBUG "/DNDEBUG /MD /Zi /Ob0 /Od /RTC1")
        message(STATUS "Overriding CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
    endif()
elseif(APPLE)
    target_compile_definitions(xeus PUBLIC -DGUID_CFUUID)
else()
    target_compile_definitions(xeus PUBLIC -DGUID_LIBUUID)
endif()

# Examples
# ========

option(BUILD_EXAMPLES "Build examples" OFF)

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

# Installation
# ============

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(XEUS_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeusConfig.cmake")

install(TARGETS xeus
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
              DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})
install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})

