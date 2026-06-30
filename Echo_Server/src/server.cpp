#include <server.hpp>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

Server::Server(int port)
    : serverSocket(-1),
      clientSocket(-1),
      port(port),
      clientAddressLength(sizeof(clientAddress))
{
}

Server::~Server()
{
    closeServer();
}

void Server::start()
{
    std::cout << "Echo server is starting..." << std::endl;

    createSocket();

    int opt = 1;
    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt));

    bindSocket();
    startListening();
    acceptClient();

    while(true){

        receiveData();     // Receive first
        sendData();        // Then echo back
    }

}

void Server::createSocket()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1)
    {
        std::cout << "Failed to create the socket..." << std::endl;
        return;
    }

    std::cout << "Socket created successfully with socketFD "
              << serverSocket << std::endl;
}

void Server::bindSocket()
{
    std::memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int result = bind(
        serverSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress));

    if (result == -1)
    {
        std::cerr << "Failed to bind socket!" << std::endl;
        return;
    }

    std::cout << "Socket bound successfully." << std::endl;
}

void Server::startListening()
{
    int result = listen(serverSocket, 5);

    if (result == -1)
    {
        std::cout << "Failed to listen." << std::endl;
    }
    else
    {
        std::cout << "Socket is listening on port "
                  << port << std::endl;
    }
}

void Server::acceptClient()
{
    clientSocket = accept(
        serverSocket,
        (sockaddr*)&clientAddress,
        &clientAddressLength);

    if (clientSocket == -1)
    {
        std::cout << "Failed to accept client." << std::endl;
        return;
    }

    std::cout << "Client connected successfully." << std::endl;

    std::cout << "Client connected from "
              << inet_ntoa(clientAddress.sin_addr)
              << ":"
              << ntohs(clientAddress.sin_port)
              << std::endl;
}

void Server::receiveData()
{
    std::memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0);

    if (bytesReceived == -1)
    {
        std::cout << "Failed to receive bytes." << std::endl;
        return;
    }

    buffer[bytesReceived] = '\0';

    std::cout << "Client is saying : "
              << buffer << std::endl;
}

void Server::sendData()
{
    int bytesSent = send(
        clientSocket,
        buffer,
        strlen(buffer),
        0);

    if (bytesSent == -1)
    {
        std::cout << "Failed to send response from server." << std::endl;
        return;
    }

    std::cout << "Echo sent back to client." << std::endl;
}

void Server::closeServer()
{
    if (serverSocket != -1)
    {
        close(serverSocket);
        serverSocket = -1;
    }

    if (clientSocket != -1)
    {
        close(clientSocket);
        clientSocket = -1;
    }
}