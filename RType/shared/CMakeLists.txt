include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 4.5.0
)

FetchContent_MakeAvailable(raylib)

add_library(Shared INTERFACE)

target_include_directories(Shared INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

target_link_libraries(Shared INTERFACE raylib)

if(WIN32)
    target_link_libraries(Shared INTERFACE ws2_32 opengl32 gdi32 winmm)
elseif(UNIX)
    target_link_libraries(Shared INTERFACE pthread)
endif()