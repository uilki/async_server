include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VUSERVER_OUT_DIR}/bin/test)
include(configure)

add_executable(serverlib_test
    serverautentificator_test.cpp
    filePasswordServer_test.cpp
    requestparser_test.cpp
)

target_link_libraries(serverlib_test
    PRIVATE
    GTest::GTest
    serverlib
    utillib
)

add_test(serverlib_gtests serverlib_test)
