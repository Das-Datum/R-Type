file(GLOB SERVER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/Network/src/*.cpp"
)

add_executable(server
    ${SERVER_SOURCES}
)

target_include_directories(server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/ECS/includes
    ${CMAKE_SOURCE_DIR}/Network/includes
    ${CMAKE_SOURCE_DIR}/shared/includes
)

target_link_libraries(server
    ECS
    Shared
)

if(WIN32)
    target_link_libraries(server ws2_32)
endif()

add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:server>/stages
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/RType/shared/stages
        $<TARGET_FILE_DIR:server>/stages
)

set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/RType/server
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/RType/server
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/RType/server
)