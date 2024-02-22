file(GLOB SOURCES
    *.h
    *.cpp
)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(
    ../net/core
    ../net/client
    ../util
)

add_executable(client ${SOURCES})

target_link_libraries(client clientlib ${Boost_LIBRARIES})
