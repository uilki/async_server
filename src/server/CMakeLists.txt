set(TARGET "serverapp")
set(SOURCES main.cpp)

include_directories(
    ../net/core
    ../net/server
    ../util
    ../functions
    ../common
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/bin/server)
include(configure)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} serverlib testfunlib utillib commonlib)
