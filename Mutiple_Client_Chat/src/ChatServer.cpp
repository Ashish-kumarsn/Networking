#include "ChatServer.hpp"

#include <sys/socket.h>
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

ChatServer::ChatServer(int port)
    : serverSocket(-1),
      port(port)
{
    std::memset(&serverAddress, 0, sizeof(serverAddress));
}

ChatServer::~ChatServer()
{
    if (serverSocket != -1)
    {
        close(serverSocket);
    }
}

void ChatServer::start()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1)
    {
        std::cout << "Failed to create socket\n";
        return;
    }

    std::cout << "Socket created successfully\n";

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket,
             (sockaddr*)&serverAddress,
             sizeof(serverAddress)) == -1)
    {
        std::cout << "Failed to bind socket\n";
        close(serverSocket);
        return;
    }

    std::cout << "Bind successful\n";

    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        std::cout << "Listen failed\n";
        close(serverSocket);
        return;
    }

    std::cout << "Server listening on port " << port << std::endl;

    acceptClient();
}

void ChatServer::acceptClient()
{
    while (true)
    {
        sockaddr_in clientAddress{};
        socklen_t clientSize = sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            (sockaddr*)&clientAddress,
            &clientSize
        );

        if (clientSocket == -1)
        {
            std::cout << "Failed to accept client\n";
            continue;
        }

        std::cout << "New client connected\n";

        std::thread clientThread(
            &ChatServer::handleClient,
            this,
            clientSocket
        );

        clientThread.detach();
    }
}

void ChatServer::handleClient(int clientSocket)
{
    char buffer[BUFFER_SIZE];

    // Receive username first
    std::memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(
        clientSocket,
        buffer,
        BUFFER_SIZE,
        0
    );

    if (bytesReceived <= 0)
    {
        close(clientSocket);
        return;
    }

    std::string username(buffer, bytesReceived);

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients[clientSocket] = username;
    }

    broadcastMessage(username + " joined the chat.", clientSocket);

    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));

        bytesReceived = recv(
            clientSocket,
            buffer,
            BUFFER_SIZE,
            0
        );

        if (bytesReceived <= 0)
        {
            std::cout << username << " disconnected.\n";

            removeClient(clientSocket);

            close(clientSocket);

            break;
        }

        std::string message(buffer, bytesReceived);

        std::cout << username << ": "
                  << message
                  << std::endl;

        broadcastMessage(
            username + ": " + message,
            clientSocket
        );
    }
}

void ChatServer::broadcastMessage(
    const std::string& message,
    int senderSocket
)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto& client : clients)
    {
        if (client.first == senderSocket)
        {
            continue;
        }

        send(
            client.first,
            message.c_str(),
            message.size(),
            0
        );
    }
}


void ChatServer::removeClient(int clientSocket)
{
    std::string username;

    {
        std::lock_guard<std::mutex> lock(clientsMutex);

        auto it = clients.find(clientSocket);

        if (it == clients.end())
            return;

        username = it->second;
        clients.erase(it);
    }

    broadcastMessage(
        username + " left the chat.",
        clientSocket
    );
}