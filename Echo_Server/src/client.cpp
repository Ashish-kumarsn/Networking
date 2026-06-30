#include <client.hpp>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

Client::Client(
    const char* ip,
    int port)
    : clientSocket(-1),
      port(port),
      serverIP(ip)
{
}

Client::~Client()
{
    closeClient();
}

void Client::start()
{
    std::cout << "Client is starting..." << std::endl;

    createSocket();
    connectToServer();
    while(true){

        sendData();
        receiveData();
    }
}

void Client::createSocket()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1)
    {
        std::cout << "Failed to create client socket." << std::endl;
        return;
    }

    std::cout << "Client socket created successfully." << std::endl;
}

void Client::connectToServer()
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    inet_pton(
        AF_INET,
        serverIP,
        &serverAddress.sin_addr);

    int result = connect(
        clientSocket,
        (sockaddr*)&serverAddress,
        sizeof(serverAddress));

    if (result == -1)
    {
        std::cout << "Failed to connect with the server." << std::endl;
        return;
    }

    std::cout << "Connected to server." << std::endl;
}

void Client::sendData()
{
    std::cout << "Enter the message" << std::endl;

    std::cin.getline(buffer, sizeof(buffer));

    int bytesSent = send(
        clientSocket,
        buffer,
        strlen(buffer),
        0);

    if (bytesSent == -1)
    {
        std::cout << "Failed to send data." << std::endl;
        return;
    }

    std::cout << "Data sent." << std::endl;
}

void Client::receiveData()
{
    std::memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0);

    if (bytesReceived == -1)
    {
        std::cout << "Failed to receive data." << std::endl;
        return;
    }

    buffer[bytesReceived] = '\0';

    std::cout << "Server replied : "
              << buffer << std::endl;
}

void Client::closeClient()
{
    if (clientSocket != -1)
    {
        close(clientSocket);
        clientSocket = -1;
    }
}