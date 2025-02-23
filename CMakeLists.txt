# SPDX-License-Identifier: MIT
# SPDX-Author: Roman Koch <koch.romam@gmail.com>
# SPDX-Copyright: 2024 Roman Koch <koch.romam@gmail.com>

cmake_minimum_required(VERSION 3.10)

project(da LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# libwebsockets finden
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBWEBSOCKETS REQUIRED libwebsockets)

include_directories(${LIBWEBSOCKETS_INCLUDE_DIRS})
link_directories(${LIBWEBSOCKETS_LIBRARY_DIRS})

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/streamer/application_streamer.cpp
)

set(HEADERS
    ${SRC_DIR}/streamer/application_streamer.hpp
)

set(INCLUDE_DIRS
    ${SRC_DIR}
    ${SRC_DIR}/streamer
)

add_executable(streamer ${SRC_DIR}/streamer.cpp ${SOURCES} ${HEADERS})
target_include_directories(streamer PRIVATE ${INCLUDE_DIRS})
target_link_libraries(streamer ${LIBWEBSOCKETS_LIBRARIES})

set(CMAKE_INSTALL_PREFIX "/usr")

install(TARGETS streamer
    RUNTIME DESTINATION bin
    #LIBRARY DESTINATION lib
    #ARCHIVE DESTINATION lib
)
