cmake_minimum_required(VERSION 3.10)

project(Epoll_Chat_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)





add_executable(
    Epoll_Server
    src/main.cpp
    src/Client.cpp
    src/EpollManager.cpp
    src/Server.cpp
    src/Utils.cpp
    )
    
    #find_package(threads REQUIRED)
    target_include_directories(Epoll_Server PRIVATE include)


    target_link_libraries(
        Epoll_Server

    )