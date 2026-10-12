cmake_minimum_required(VERSION 3.15)

project(scp_remake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(raylib 5.0 QUIET)
if(NOT raylib_FOUND)
    message(STATUS "raylib not found, downloading it")
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(raylib)
endif()

if(MSVC)
    set(SCP_WARNINGS /W3)
    add_compile_options(/utf-8)
else()
    set(SCP_WARNINGS -Wall -Wextra -Wpedantic)
endif()
add_library(scp_assets STATIC
    engine/core/math/math.cpp
    engine/assets/AssetResolver.cpp
    engine/assets/RMesh.cpp
    engine/assets/XMesh.cpp
    engine/assets/RoomLoader.cpp
)
target_include_directories(scp_assets PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_options(scp_assets PRIVATE ${SCP_WARNINGS})

add_executable(${PROJECT_NAME}
    main.cpp
    model_viewer/model_viewer.cpp
    engine/renderer/Renderer.cpp
    engine/renderer/RaylibRenderer.cpp
    engine/input/Input.cpp
    engine/input/RaylibInput.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        SCP_DEFAULT_GFX="${CMAKE_SOURCE_DIR}/GFX/map"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        scp_assets
        raylib
)

target_compile_options(${PROJECT_NAME} PRIVATE ${SCP_WARNINGS})
add_executable(rmesh_check tools/rmesh_check.cpp)
target_link_libraries(rmesh_check PRIVATE scp_assets)
target_compile_options(rmesh_check PRIVATE ${SCP_WARNINGS})
