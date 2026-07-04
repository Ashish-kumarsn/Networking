cmake_minimum_required(VERSION 3.10)

project(Multiple_Client_Chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)


add_executable(
    Multi_Client
    src/ChatClient.cpp
    src/ChatServer.cpp
    src/main.cpp
    )

    
    find_package(Threads REQUIRED)
    
    target_link_libraries(
        Multi_Client
        PRIVATE Threads::Threads
    )