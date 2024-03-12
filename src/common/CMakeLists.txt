set(TARGET "commonlib")

file(GLOB SOURCES
    *.h
    *.cpp
)

add_library(${TARGET} ${SOURCES})

