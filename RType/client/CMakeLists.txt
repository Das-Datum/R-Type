cmake_minimum_required(VERSION 3.14)
project(RTYPE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_RC_COMPILER_INIT windres)
    find_program(CMAKE_RC_COMPILER NAMES windres)
    if(NOT CMAKE_RC_COMPILER)
        message(FATAL_ERROR "windres not found")
    endif()
    enable_language(RC OPTIONAL)
endif()

include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 4.5.0
)

FetchContent_MakeAvailable(raylib)

file(GLOB HEADER_FILES "includes/*.hpp")
file(GLOB HEADER_FILES_ECS "../ECS/includes/*.hpp")
set(SOURCES
    src/main.cpp
    ${HEADER_FILES}
    ${HEADER_FILES_ECS}
)

add_executable(main ${SOURCES})

target_include_directories(main PRIVATE includes)
target_link_libraries(main PRIVATE raylib)

if(WIN32)
  target_link_libraries(main PRIVATE opengl32 gdi32 winmm)
endif()
