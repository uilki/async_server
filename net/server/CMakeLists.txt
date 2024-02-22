file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(
    ../core
    ../../util
)

add_library(serverlib STATIC ${SOURCES})
target_link_libraries(serverlib util core)
