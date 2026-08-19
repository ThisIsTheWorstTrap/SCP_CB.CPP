cmake_minimum_required(VERSION 3.15)

project(scp_remake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    engine/core/handle/handle.cpp
    engine/core/math/math.cpp
    engine/renderer/Renderer.cpp
    engine/renderer/RaylibRenderer.cpp
    engine/input/Input.cpp
    engine/input/RaylibInput.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        raylib
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)