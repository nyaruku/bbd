cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_PARALLEL_LEVEL 16)

# enter your project name/application output name
project(bbd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF       CACHE BOOL   "" FORCE)
set(BUILD_EXAMPLES    OFF       CACHE BOOL   "" FORCE)
set(BUILD_GAMES       OFF       CACHE BOOL   "" FORCE)
set(OPENGL_VERSION    "1.1"     CACHE STRING "" FORCE)
set(PLATFORM          "Desktop" CACHE STRING "" FORCE)

if (UNIX AND NOT APPLE)
    set(GLFW_BUILD_X11     ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.14.2
)
FetchContent_MakeAvailable(raylib)
FetchContent_MakeAvailable(cpr)

add_executable(${PROJECT_NAME} "./src/main.cpp")

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib cpr::cpr)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib cpr::cpr m)
endif()