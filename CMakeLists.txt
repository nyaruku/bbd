cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_PARALLEL_LEVEL 16)

# enter your project name/application output name
project(bbd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(BASE_INCLUDE_PATH  "${CMAKE_SOURCE_DIR}/includes")
set(BASE_LIB_PATH      "${CMAKE_SOURCE_DIR}/libs")

set(BUILD_SHARED_LIBS  OFF       CACHE BOOL   "" FORCE)
set(BUILD_EXAMPLES     OFF       CACHE BOOL   "" FORCE)
set(BUILD_GAMES        OFF       CACHE BOOL   "" FORCE)
set(OPENGL_VERSION     "1.1"     CACHE STRING "" FORCE)
set(PLATFORM           "Desktop" CACHE STRING "" FORCE)

# FetchContent
include(FetchContent)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.14.2
)
FetchContent_MakeAvailable(cpr)

include_directories(${BASE_INCLUDE_PATH})
add_executable(${PROJECT_NAME} "./src/main.cpp")

if (WIN32)
    target_link_directories(${PROJECT_NAME} PRIVATE "${BASE_LIB_PATH}/raylib-6.0_win64_msvc16")
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib cpr::cpr)
elseif (UNIX AND NOT APPLE)
    target_link_directories(${PROJECT_NAME} PRIVATE "${BASE_LIB_PATH}/raylib-6.0_linux_amd64")
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib cpr::cpr m X11)
endif()