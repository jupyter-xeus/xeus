############################################################################
# Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.1)
project(xeus)

# Dependencies
# ============

find_package(cppzmq REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(cryptopp REQUIRED)

# Compilation flags
# =================

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DGUID_WINDOWS /MP /bigobj")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4996")
    # Allows to compile in debug without requiring each dependencies to
    # be compiled in debug
    if(${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
        set(CMAKE_CXX_FLAGS_DEBUG "/DNDEBUG /MD /Zi /Ob0 /Od /RTC1")
        message(STATUS "Overriding CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
    endif()
endif()

# Source files
# ============

set(XEUS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(XEUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(XEUS_HEADERS
    ${XEUS_INCLUDE_DIR}/xeus/xauthentication.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xeus.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xguid.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xinterpreter.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xjson.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xkernel.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xkernel_configuration.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xmessage.hpp
    ${XEUS_INCLUDE_DIR}/xeus/xserver.hpp
)

set(XEUS_SOURCES
    ${XEUS_SOURCE_DIR}/xauthentication.cpp
    ${XEUS_SOURCE_DIR}/xguid.cpp
    ${XEUS_SOURCE_DIR}/xheartbeat.cpp
    ${XEUS_SOURCE_DIR}/xheartbeat.hpp
    ${XEUS_SOURCE_DIR}/xinterpreter.cpp
    ${XEUS_SOURCE_DIR}/xjson.cpp
    ${XEUS_SOURCE_DIR}/xkernel.cpp
    ${XEUS_SOURCE_DIR}/xkernel_configuration.cpp
    ${XEUS_SOURCE_DIR}/xkernel_core.cpp
    ${XEUS_SOURCE_DIR}/xkernel_core.hpp
    ${XEUS_SOURCE_DIR}/xmessage.cpp
    ${XEUS_SOURCE_DIR}/xmiddleware.cpp
    ${XEUS_SOURCE_DIR}/xmiddleware.hpp
    ${XEUS_SOURCE_DIR}/xpublisher.cpp
    ${XEUS_SOURCE_DIR}/xpublisher.hpp
    ${XEUS_SOURCE_DIR}/xserver.cpp
    ${XEUS_SOURCE_DIR}/xserver_impl.cpp
    ${XEUS_SOURCE_DIR}/xserver_impl.hpp
    ${XEUS_SOURCE_DIR}/xstring_utils.hpp
)

include_directories(${cppzmq_INCLUDE_DIR})
include_directories(${RAPIDJSON_INCLUDE_DIRS})
include_directories(${XEUS_INCLUDE_DIR}/xeus)

# Versioning
# ==========

file(STRINGS "${XEUS_INCLUDE_DIR}/xeus/xeus.hpp" xeus_version_defines
    REGEX "#define XEUS_VERSION_(MAJOR|MINOR|PATCH)")
foreach(ver ${xeus_version_defines})
    if(ver MATCHES "#define XEUS_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()
set(${PROJECT_NAME}_VERSION 
    ${XEUS_VERSION_MAJOR}.${XEUS_VERSION_MINOR}.${XEUS_VERSION_PATCH})
message(STATUS "${PROJECT_NAME} version: v${${PROJECT_NAME}_VERSION}")

# Output
# ======

set(XEUS_TARGET libxeus)
set(XEUS_DEPENDENCIES ${cppzmq_LIBRARY} cryptopp-static)

if(MSVC)
    add_library(${XEUS_TARGET} SHARED ${XEUS_SOURCES} ${XEUS_HEADERS})
    target_link_libraries(${XEUS_TARGET} ${XEUS_DEPENDENCIES})
    set_target_properties(${XEUS_TARGET} PROPERTIES
                          PUBLIC_HEADER "${XEUS_HEADERS}"
                          COMPILE_DEFINITIONS "XEUS_EXPORTS")
endif()

# Examples
# ========

option(BUILD_EXAMPLES "Build examples" OFF)

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()



# Installation
# ============

set(libxeus_link_file ${XEUS_TARGET}${CMAKE_LINK_LIBRARY_SUFFIX})
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# GNUInstallDirs "DATADIR" wrong here; CMake search path wants "share".
set(XEUS_CMAKECONFIG_INSTALL_DIR "share/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeusConfig.cmake")

#install(FILES ${XEUS_HEADERS}
#        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xtensor)

install(TARGETS ${XEUS_TARGET}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus)

configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_CMAKECONFIG_INSTALL_DIR})

if(BUILD_EXAMPLES)
    # TODO install example targets
endif()
