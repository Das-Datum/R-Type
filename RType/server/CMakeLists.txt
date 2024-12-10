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

find_package (nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(server
    ECS
    Shared
    nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(server ws2_32)
endif()
