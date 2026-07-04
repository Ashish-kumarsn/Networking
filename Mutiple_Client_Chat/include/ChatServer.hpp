#include <sys/socket.h>
#include "common.hpp"
#include <vector>
#include <mutex>
#pragma once
#include <string>
#include <netinet/in.h>
#include <unordered_map>


class ChatServer
{
public:
    ChatServer(int port = Port);
    ~ChatServer();
    void start();

private:
    void acceptClient();
    void handleClient(int clientSocket);
    void broadcastMessage(const std::string &message, int senderSocket);
    void removeClient(int clientSocket);

private:
    int serverSocket{-1};
    int port;

    std::unordered_map<int, std::string> clients;
    std::mutex clientsMutex;

    sockaddr_in serverAddress;
};