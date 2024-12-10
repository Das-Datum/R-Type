include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 4.5.0
)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.2.0
)

set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(raylib nlohmann_json)

add_library(Shared INTERFACE)

target_include_directories(Shared INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_link_libraries(Shared INTERFACE raylib nlohmann_json::nlohmann_json)

if(WIN32)
    target_link_libraries(Shared INTERFACE ws2_32 opengl32 gdi32 winmm)
elseif(UNIX)
    target_link_libraries(Shared INTERFACE pthread)
endif()