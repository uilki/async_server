set(TARGET "corelib")
find_package(Threads)

file(GLOB SOURCES
    *.h
    *.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/util)

add_library(${TARGET} STATIC ${SOURCES})
target_link_libraries(${TARGET} utillib Threads::Threads)
