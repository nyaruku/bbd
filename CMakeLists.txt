cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_PARALLEL_LEVEL 16)

# enter your project name/application output name
project(bbd LANGUAGES C)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

set(BUILD_EXAMPLES     OFF        CACHE BOOL   "" FORCE)
set(BUILD_GAMES        OFF        CACHE BOOL   "" FORCE)
set(OPENGL_VERSION     "Software" CACHE STRING "" FORCE)
set(PLATFORM           "SDL"      CACHE STRING "" FORCE)

if (UNIX AND NOT APPLE)
    set(SDL_X11       ON  CACHE BOOL "" FORCE)
    set(SDL_WAYLAND   ON  CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME} "./src/main.c")

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib m)
endif()