find_package(Threads)

file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(../util)

add_library(core STATIC ${SOURCES})
target_link_libraries(core util Threads::Threads)
