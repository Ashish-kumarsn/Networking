#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>



class ChatClient
{
public:
    ChatClient(
        const std::string& UserName,
        const std::string& ServerIP = "127.0.0.1",
        int Port = 8080
    );

    ~ChatClient();

    void start();

private:
    void createSocket();
    void connectToServer();
    void sendMessage();
    void receiveMessage();

private:
    int ClientSocket{-1};
    int Port;

    std::string UserName;
    std::string ServerIP;

    sockaddr_in ServerAddress{};
};