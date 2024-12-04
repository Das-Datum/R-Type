

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

file(GLOB CLIENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(client
    ${CLIENT_SOURCES}
)

target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/ECS/includes
    ${CMAKE_SOURCE_DIR}/shared/includes
)

target_link_libraries(client
    ECS
    Shared
    raylib
)

if(WIN32)
    target_link_libraries(client ws2_32 PRIVATE opengl32 gdi32 winmm)
elseif(UNIX)
    target_link_libraries(client pthread)
endif()

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:client>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:client>/assets
)
