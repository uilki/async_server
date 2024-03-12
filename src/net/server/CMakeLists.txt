set(TARGET "serverlib")
file(GLOB SOURCES
    *.h
    *.cpp
    */*.h
    */*.cpp
)

include_directories(
    *
    ${PROJECT_SOURCE_DIR}/util
    ${PROJECT_SOURCE_DIR}/net/core
)

add_library(${TARGET} STATIC ${SOURCES})
target_link_libraries(${TARGET} utillib corelib)
