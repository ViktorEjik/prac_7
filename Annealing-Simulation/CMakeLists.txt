cmake_minimum_required(VERSION 3.28)
project(Annealing_Simulation)

set(CMAKE_CXX_STANDARD 23)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_subdirectory(thirdparty/CLI11)

add_executable(Annealing_Simulation main.cpp ${SRC})
add_executable(GenTests GenerateTests.cpp ${SRC})

target_link_libraries(GenTests CLI11::CLI11)
target_link_libraries(Annealing_Simulation CLI11::CLI11)
