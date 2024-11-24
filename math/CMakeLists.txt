cmake_minimum_required(VERSION 3.29)
project(math_test)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.0)
FetchContent_MakeAvailable(googletest)

add_subdirectory(MyMath)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE MyMath gtest_main gmock_main)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE MyMath)
