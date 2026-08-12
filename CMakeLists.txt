cmake_minimum_required(VERSION 3.15)

# enter your project name/application output name
project(bbd LANGUAGES C)

set(CMAKE_C_STANDARD 11)

# Base directories
set(BASE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/src/includes")
set(BASE_LIB_PATH "${CMAKE_SOURCE_DIR}/src/libs")

# STATIC LIB, you are a stupid fuck if u dont
option(USE_STATIC_LIB "Use static library" ON)

# Add the include directories for the project
include_directories(${BASE_INCLUDE_PATH} ${BUILD_INCLUDE_PATH})

# Add the library directories for linking
link_directories(${BASE_LIB_PATH} ${BUILD_LIB_PATH})

# Project-specific CMakeLists.txt
add_executable(${PROJECT_NAME} "./src/main.c")