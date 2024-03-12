set(TARGET "testfunlib")

file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/util)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/lib)

add_library(${TARGET} ${SOURCES})

