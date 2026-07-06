#include "Client.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>

#include <cstring>
#include <iostream>

ChatClient::ChatClient(
    const std::string& UserName,
    const std::string& ServerIP,
    int Port)
    : UserName(UserName),
      ServerIP(ServerIP),
      Port(Port)
{
}

ChatClient::~ChatClient()
{
    if (ClientSocket != -1)
    {
        close(ClientSocket);
    }
}

void ChatClient::createSocket()
{
    ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ClientSocket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
}

void ChatClient::connectToServer()
{
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(Port);

    if (inet_pton(AF_INET,
                  ServerIP.c_str(),
                  &ServerAddress.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(ClientSocket,
                reinterpret_cast<sockaddr*>(&ServerAddress),
                sizeof(ServerAddress)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server.\n";
}

void ChatClient::sendMessage()
{
    std::string Message;

    if (!std::getline(std::cin, Message))
    {
        return;
    }

    if (Message.empty())
    {
        return;
    }

    Message = UserName + ": " + Message;

    ssize_t BytesSent = send(
        ClientSocket,
        Message.c_str(),
        Message.size(),
        0);

    if (BytesSent < 0)
    {
        perror("send");
    }
}

void ChatClient::receiveMessage()
{
    char Buffer[1024];

    memset(Buffer, 0, sizeof(Buffer));

    ssize_t BytesReceived = recv(
        ClientSocket,
        Buffer,
        sizeof(Buffer) - 1,
        0);

    if (BytesReceived == 0)
    {
        std::cout << "\nServer disconnected.\n";
        close(ClientSocket);
        ClientSocket = -1;
        exit(EXIT_SUCCESS);
    }

    if (BytesReceived < 0)
    {
        perror("recv");
        return;
    }

    Buffer[BytesReceived] = '\0';

    std::cout << "\n" << Buffer << std::endl;
}

void ChatClient::start()
{
    createSocket();
    connectToServer();

    int EpollFD = epoll_create1(0);

    if (EpollFD < 0)
    {
        perror("epoll_create1");
        return;
    }

    epoll_event Event{};

    // Monitor socket
    Event.events = EPOLLIN;
    Event.data.fd = ClientSocket;

    if (epoll_ctl(
            EpollFD,
            EPOLL_CTL_ADD,
            ClientSocket,
            &Event) < 0)
    {
        perror("epoll_ctl socket");
        return;
    }

    // Monitor keyboard
    Event.events = EPOLLIN;
    Event.data.fd = STDIN_FILENO;

    if (epoll_ctl(
            EpollFD,
            EPOLL_CTL_ADD,
            STDIN_FILENO,
            &Event) < 0)
    {
        perror("epoll_ctl stdin");
        return;
    }

    constexpr int MaxEvents = 10;
    epoll_event Events[MaxEvents];

    std::cout << "===== Chat Started =====\n";

    while (true)
    {
        int Ready = epoll_wait(
            EpollFD,
            Events,
            MaxEvents,
            -1);

        if (Ready < 0)
        {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < Ready; i++)
        {
            int FD = Events[i].data.fd;

            if (FD == ClientSocket)
            {
                receiveMessage();
            }
            else if (FD == STDIN_FILENO)
            {
                sendMessage();
            }
        }
    }

    close(EpollFD);
}