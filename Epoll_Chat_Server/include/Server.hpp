#pragma once

#include <string>
#include <unordered_map>

#include <sys/socket.h>
#include <netinet/in.h>

class EpollServer
{
public:
    explicit EpollServer(int Port = 8080);
    ~EpollServer();

    void start();

private:
    void createSocket();
    void bindSocket();
    void listenSocket();

    void createEpoll();
    void addServerSocketToEpoll();

    void acceptClient();
    void handleClientMessages(int ClientFd);
    void broadcastMessage(const std::string& Message, int SenderFd);
    void removeClient(int ClientFd);

private:
    std::unordered_map<int, std::string> Clients;

    int ServerSocket{-1};
    int EpollFd{-1};
    int Port;

    char Buffer[1024]{};

    sockaddr_in serverAddress{};
    sockaddr_in clientAddress{};
};