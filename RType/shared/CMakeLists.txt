file(GLOB SHARED_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(Shared INTERFACE)

target_include_directories(Shared INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)