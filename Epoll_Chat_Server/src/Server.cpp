#include "Server.hpp"

#include <sys/socket.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

EpollServer::EpollServer(int Port)
    : ServerSocket(-1),
      EpollFd(-1),
      Port(Port)
{
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    std::memset(&clientAddress, 0, sizeof(clientAddress));
}

EpollServer::~EpollServer()
{
    if (ServerSocket != -1)
    {
        close(ServerSocket);
    }
    if (EpollFd != -1)
    {
        close(EpollFd);
    }
}

void EpollServer::start()
{
    std::cout << "server is running..." << std::endl;
    createSocket();
    bindSocket();
    listenSocket();
    createEpoll();
    addServerSocketToEpoll();

    epoll_event Events[10];
    while (true)
    {
        int NumberOfEvents = epoll_wait(
            EpollFd,
            Events,
            10,
            -1);

        if (NumberOfEvents == -1)
        {
            std::cerr << "epoll wait() failed" << std::endl;
            continue;
        }

        for (int i = 0; i < NumberOfEvents; i++)
        {
            if (Events[i].data.fd == ServerSocket)
            {
                acceptClient();
            }
            else
            {
                handleClientMessages(Events[i].data.fd);
            }
        }
    }
}

void EpollServer::createSocket()
{
    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == -1)
    {
        std::cerr << "Error in creating socket" << std::endl;
        return;
    }
    std::cout << "Socket created" << std::endl;
}
void EpollServer::bindSocket()
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(Port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int result = bind(
        ServerSocket,
        (sockaddr *)&serverAddress,
        sizeof(serverAddress)

    );

    if (result == -1)
    {
        std::cerr << "Error in binding socket" << std::endl;
        close(ServerSocket);
        return;
    }

    std::cout << "Binding successfull" << std::endl;
}

void EpollServer::listenSocket()
{
    if (listen(ServerSocket, 10) == -1)
    {
        std::cerr << "Failed to listen " << std::endl;
        close(ServerSocket);
        return;
    }
    std::cout << "Listening to port" << Port << std::endl;
}

void EpollServer::createEpoll()
{
    EpollFd = epoll_create1(0);
    if (EpollFd == -1)
    {
        std::cerr << "Failed to create epoll instance." << std::endl;
        return;
    }

    std::cout << "Epoll instance created successfully." << std::endl;
}

void EpollServer::addServerSocketToEpoll()
{
    epoll_event event;
    std::memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = ServerSocket;

    int temp = epoll_ctl(EpollFd, EPOLL_CTL_ADD, ServerSocket, &event);

    if (temp == -1)
    {
        std::cerr << "Failed to add server socket to epoll" << std::endl;
        return;
    }
    std::cout << "Server socekt added to epoll" << std::endl;
}

void EpollServer::acceptClient()
{
    socklen_t clientAddressleng = sizeof(clientAddress);
    int ClientSocket = accept(
        ServerSocket,
        (sockaddr *)&clientAddress,
        &clientAddressleng);
    if (ClientSocket == -1)
    {
        std::cerr << "client acceptation fail" << std::endl;
    }
    std::cout << "Client Accepted" << std::endl;

    int Flags = fcntl(ClientSocket, F_GETFL, 0);

    if (Flags == -1)
    {
        std::cerr << "Failed to get socket flags" << std::endl;
        close(ClientSocket);
        return;
    }

    if (fcntl(ClientSocket, F_SETFL, Flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to set socket as non-blocking" << std::endl;
        close(ClientSocket);
        return;
    }

    epoll_event Event;
    std::memset(&Event, 0, sizeof(Event));

    Event.events = EPOLLIN;
    Event.data.fd = ClientSocket;

    if (epoll_ctl(EpollFd, EPOLL_CTL_ADD, ClientSocket, &Event) == -1)
    {
        std::cerr << "Failed to add Client socket to epoll" << std::endl;
        close(ClientSocket);
        return;
    }
    Clients[ClientSocket] = "";
    std::cout << "new client connected to Socket " << ClientSocket << std::endl;
}

void EpollServer::handleClientMessages(int ClientFd)
{
    std::memset(Buffer, 0, sizeof(Buffer));

    int BytesReceived = recv(
        ClientFd,
        Buffer,
        sizeof(Buffer),
        0);

    if (BytesReceived == 0)
    {
        std::cerr << "Client Disconnected" << ClientFd << std::endl;
        removeClient(ClientFd);
        return;
    }
    if (BytesReceived == -1)
    {
        std::cerr << "Failed to receive message from client" << std::endl;
        removeClient(ClientFd);
        return;
    }
    std::string Message(Buffer, BytesReceived);
    std::cout << "Client" << ClientFd << ":" << Message << std::endl;
    broadcastMessage(Message, ClientFd);
}

void EpollServer::broadcastMessage(const std::string &Message, int SenderFd)
{
    for (const auto &Client : Clients)
    {
        int ClientFd = Client.first;

        if (ClientFd == SenderFd)
        {
            continue;
        }
        int BytesSent = send(
            ClientFd,
            Message.c_str(),
            Message.size(),
            0);
        if (BytesSent == -1)
        {
            std::cerr << "Failed to send message to client: "
                      << ClientFd << std::endl;
        }
    }
}

void EpollServer::removeClient(int ClientFd)
{
    // Remove client from epoll
    if (epoll_ctl(EpollFd, EPOLL_CTL_DEL, ClientFd, nullptr) == -1)
    {
        std::cerr << "Failed to remove client from epoll: "
                  << ClientFd << std::endl;
    }

    // Remove from client list
    Clients.erase(ClientFd);

    // Close socket
    close(ClientFd);

    std::cout << "Client disconnected: "
              << ClientFd << std::endl;
}