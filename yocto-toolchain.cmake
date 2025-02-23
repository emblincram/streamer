# SPDX-License-Identifier: MIT
# SPDX-Author: Roman Koch <koch.romam@gmail.com>
# SPDX-Copyright: 2024 Roman Koch <koch.romam@gmail.com>

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

execute_process(COMMAND bash -c "echo $CC | awk '{print $1}'" OUTPUT_VARIABLE C_COMPILER_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND bash -c "echo $CXX | awk '{print $1}'" OUTPUT_VARIABLE CXX_COMPILER_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT C_COMPILER_PATH OR NOT CXX_COMPILER_PATH)
    message(FATAL_ERROR "ARM Cross-Compiler nicht gefunden! Stelle sicher, dass das Yocto-SDK geladen ist.")
endif()

set(CMAKE_C_COMPILER ${C_COMPILER_PATH})
set(CMAKE_CXX_COMPILER ${CXX_COMPILER_PATH})

execute_process(COMMAND bash -c "echo $CC | cut -d' ' -f2-" OUTPUT_VARIABLE EXTRACTED_C_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND bash -c "echo $CXX | cut -d' ' -f2-" OUTPUT_VARIABLE EXTRACTED_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_C_FLAGS "${EXTRACTED_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${EXTRACTED_CXX_FLAGS}")

if(NOT DEFINED ENV{SDKTARGETSYSROOT})
    message(FATAL_ERROR "SDKTARGETSYSROOT ist nicht gesetzt! Bitte das Yocto-SDK aktivieren.")
endif()
set(CMAKE_SYSROOT "$ENV{SDKTARGETSYSROOT}")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--sysroot=${CMAKE_SYSROOT} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-z,relro,-z,now")

set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
