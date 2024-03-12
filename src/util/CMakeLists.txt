set(TARGET "utillib")

file(GLOB SOURCES
    *.h
    *.cpp
)

add_library(${TARGET} ${SOURCES})
