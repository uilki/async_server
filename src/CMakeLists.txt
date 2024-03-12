enable_testing()

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(functions)
add_subdirectory(client)
add_subdirectory(util)
add_subdirectory(net)
add_subdirectory(server)
add_subdirectory(test)
add_subdirectory(common)
