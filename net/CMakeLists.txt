find_package(Threads)

include_directories(${Boost_INCLUDE_DIRS} ../util)

configure_file(${CMAKE_SOURCE_DIR}/schema.json.in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/schema.json)



add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
