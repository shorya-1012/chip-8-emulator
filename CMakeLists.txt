cmake_minimum_required(VERSION 3.5)

project(Chip_8_Emulator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME} 
  ${SOURCE_DIR}/main.cpp
  ${SOURCE_DIR}/chip8.cpp
)

add_subdirectory(vendor)
target_link_libraries(${PROJECT_NAME} PRIVATE vendor)
