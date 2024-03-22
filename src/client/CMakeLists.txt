set(TARGET "clientapp")

file(GLOB SOURCES
    *.h
    *.cpp
)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(
    ${PROJECT_SOURCE_DIR}/net/core
    ${PROJECT_SOURCE_DIR}/net/client
    ${PROJECT_SOURCE_DIR}/util
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/bin/client)
set(SSL_FILES server.crt)

include(configure)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} clientlib ${Boost_LIBRARIES})

