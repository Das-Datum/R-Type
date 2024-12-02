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

file(GLOB HEADER_FILES "includes/*.hpp")
file(GLOB HEADER_FILES_ECS "../ECS/includes/*.hpp")
set(SOURCES
    src/main.cpp
    ${HEADER_FILES}
    ${HEADER_FILES_ECS}
)

add_executable(server ${SOURCES})

target_include_directories(server PRIVATE includes)
target_link_libraries(server PRIVATE raylib)

if(WIN32)
  target_link_libraries(server PRIVATE opengl32 gdi32 winmm)
endif()
