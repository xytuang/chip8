cmake_minimum_required(VERSION 3.10)
project(CHIP_8)

set(CMAKE_CXX_STANDARD 11)

# Define your source files
set(MY_SOURCES src/main.cpp src/chip8.cpp)

# Find SDL2 package and its components
find_package(SDL2 REQUIRED COMPONENTS SDL2)

# Add the main executable
add_executable(chip8 ${MY_SOURCES})

# Link SDL2 library to the executable
target_link_libraries(chip8 PRIVATE SDL2::SDL2)
