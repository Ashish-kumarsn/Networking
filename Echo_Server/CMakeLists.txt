cmake_minimum_required(VERSION 3.10)

project(EchoServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(
    EchoServer
    src/main.cpp
    src/server.cpp
    src/client.cpp
    src/socket_utils.cpp
)