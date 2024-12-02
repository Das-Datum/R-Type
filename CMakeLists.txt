cmake_minimum_required(VERSION 3.14)
project(RType)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# include(FetchContent)

# FetchContent_Declare(
#     raylib
#     GIT_REPOSITORY https://github.com/raysan5/raylib.git
#     GIT_TAG 4.5.0  # Specify the desired version
# )

# FetchContent_GetProperties(raylib)
# if(NOT raylib_POPULATED)
#     FetchContent_Populate(raylib)
#     add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
# endif()


set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")

# download and install vcpkg
function(install_vcpkg)
    if(NOT EXISTS "${VCPKG_ROOT}/vcpkg")
        message(STATUS "vcpkg not found. Installing...")
        file(DOWNLOAD "https://github.com/microsoft/vcpkg/archive/refs/heads/master.zip"
                      "${CMAKE_BINARY_DIR}/vcpkg.zip" SHOW_PROGRESS)
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/vcpkg.zip")
        execute_process(COMMAND ${CMAKE_COMMAND} -E rename "vcpkg-master" "vcpkg")
        if(WIN32)
            execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.bat")
        else()
            execute_process(COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.sh")
        endif()
    else()
        message(STATUS "Using existing vcpkg at ${VCPKG_ROOT}")
    endif()
endfunction()

install_vcpkg()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Toolchain file for vcpkg")

execute_process(
    COMMAND ${VCPKG_ROOT}/vcpkg install raylib:x64-linux
    COMMAND ${VCPKG_ROOT}/vcpkg install libx11:x64-linux
    RESULT_VARIABLE VCPKG_INSTALL_RESULT
)

if(NOT VCPKG_INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to install dependencies with vcpkg.")
else()
    message(STATUS "All dependencies installed successfully via vcpkg.")
endif()

add_subdirectory(ECS)
add_subdirectory(RType/client)
add_subdirectory(RType/server)
add_subdirectory(RType/shared)
