find_package(Threads)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
