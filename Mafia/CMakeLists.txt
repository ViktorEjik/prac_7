cmake_minimum_required(VERSION 3.15)

# Название вашего проекта
project(Mafia)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the checked out libcoro code in your CMakeLists.txt file
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libcoro)
add_subdirectory(thirdparty/CLI11)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

# Ваш файл исходного кода
add_executable(my_app main.cpp ${SRC})

# Линковка с cppcoro
target_link_libraries(my_app libcoro CLI11::CLI11)

