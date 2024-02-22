set(SOURCES main.cpp)

include_directories(
    ../net/core
    ../net/server
    ../util
    ../functions
)

add_executable(server ${SOURCES})

target_link_libraries(server serverlib functions util)
