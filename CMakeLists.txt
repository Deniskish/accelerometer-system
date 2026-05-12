cmake_minimum_required(VERSION 3.16)
project(accelerometer_system LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/server/main.cpp
)

add_executable(node_a
    src/node_a/main.cpp
)

add_executable(node_b
    src/node_b/main.cpp
)