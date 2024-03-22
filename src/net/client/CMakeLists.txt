set(TARGET "clientlib")

file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/net/core
    ${PROJECT_SOURCE_DIR}/util
    ${PROJECT_SOURCE_DIR}/common
)

add_library(${TARGET} STATIC ${SOURCES})

target_link_libraries(${TARGET} utillib OpenSSL::Crypto OpenSSL::SSL corelib)
