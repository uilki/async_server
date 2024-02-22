file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(
    ../core
    ../../util
)

add_library(clientlib STATIC ${SOURCES})
target_link_libraries(clientlib util core)
