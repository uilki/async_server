cmake_minimum_required(VERSION 3.5)

if (NOT DEFINED NAME) 
    message("Project name is not set. Falling back to VUSERVER")
    set(NAME "VUSERVER")
endif()

message("Project name:" ${NAME})

project(${NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include(GNUInstallDirs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -D_REENTRANT -fPIC -DPIC -Wno-unused-local-typedefs -Wno-psabi -Werror=unused-result")

if ("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" STREQUAL "" OR 
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" STREQUAL "" OR
    "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" STREQUAL "")
    message("Binary output is not set. Using fallback values")
    set(VUSERVER_OUT_DIR ${${NAME}_BINARY_DIR}/../out)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/lib)
endif()

message("RUNTIME_OUTPUT_DIRECTORY " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("LIBRARY_OUTPUT_DIRECTORY " ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message("ARCHIVE_OUTPUT_DIRECTORY " ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -Wl,--exclude-libs,ALL")
enable_testing()
add_subdirectory(functions)
add_subdirectory(client)
add_subdirectory(util)
add_subdirectory(net)
add_subdirectory(server)
add_subdirectory(test)
