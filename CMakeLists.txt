cmake_minimum_required(VERSION 3.15)

project(scp_remake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        interface3d
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        raylib
)