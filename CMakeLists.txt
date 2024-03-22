cmake_minimum_required(VERSION 3.5)
message(STATUS "CMake version: ${CMAKE_VERSION}")
if (NOT DEFINED NAME) 
    message("Project name is not set. Falling back to VUSERVER")
    set(NAME "VUSERVER")
endif()

message("Project name: " ${NAME})

project(${NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -D_REENTRANT -fPIC -DPIC -Wno-unused-local-typedefs -Wno-psabi -Werror=unused-result")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_SOURCE_DIR}/cmake
)

set(VUSERVER_OUT_DIR ${${NAME}_BINARY_DIR}/../out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/lib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -Wl,--exclude-libs,ALL")

add_subdirectory(src)
