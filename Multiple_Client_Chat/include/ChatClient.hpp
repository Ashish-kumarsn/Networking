#pragma once
#include<sys/socket.h>
#include<string>
#include "common.hpp"
#include<netinet/in.h>

class ChatClient{
    public:

ChatClient(
    const std::string& username,
    const std::string& serverIP = "127.0.0.1",
    int port = Port
);
    ~ChatClient();
    void start();

    private:
    void connectToServer();
    void sendMessage();
    void receiveMessage();


    private:
    int port;
    int clientSocket{-1};
    std::string serverIP;
    sockaddr_in serverAddress{};
        std::string username;

};
