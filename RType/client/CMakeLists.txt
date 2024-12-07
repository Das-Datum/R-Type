if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_RC_COMPILER_INIT windres)
    find_program(CMAKE_RC_COMPILER NAMES windres)
    if(NOT CMAKE_RC_COMPILER)
        message(FATAL_ERROR "windres not found")
    endif()
    enable_language(RC OPTIONAL)
endif()

file(GLOB CLIENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/Network/src/*.cpp"
)

add_executable(client
    ${CLIENT_SOURCES}
)

target_include_directories(client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/ECS/includes
    ${CMAKE_SOURCE_DIR}/Network/includes
    ${CMAKE_SOURCE_DIR}/shared/includes
)

target_link_libraries(client PUBLIC ECS Shared)

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:client>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:client>/assets
)
