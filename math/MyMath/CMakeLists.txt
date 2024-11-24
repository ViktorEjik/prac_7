cmake_minimum_required(VERSION 3.29)
file(GLOB SOURCES *.cpp *.h *.hpp)

add_library(MyMath ${SOURCES}
        Functions.cpp
        Functions.hpp
        Fuctory.hpp)
target_include_directories(MyMath PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR})
