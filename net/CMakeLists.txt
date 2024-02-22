find_package(Threads)

include_directories(${Boost_INCLUDE_DIRS} ../util)

add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
