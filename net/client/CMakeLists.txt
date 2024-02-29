file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(
    ../core
    ../../util
)

add_library(clientlib STATIC ${SOURCES}
    authentificator.h authentificator.cpp)
target_link_libraries(clientlib util core)
