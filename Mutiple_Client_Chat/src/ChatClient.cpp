#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

#include "ChatClient.hpp"

ChatClient::ChatClient(
    const std::string& username,
    const std::string& serverIP,
    int port
)
    : clientSocket(-1),
      username(username),
      serverIP(serverIP),
      port(port)
{
    std::memset(&serverAddress, 0, sizeof(serverAddress));
}

ChatClient::~ChatClient()
{
    if (clientSocket != -1)
    {
        close(clientSocket);
    }
}



void ChatClient::start()
{
    connectToServer();

    if (clientSocket == -1)
    {
        return;
    }

    std::thread receiveThread(
        &ChatClient::receiveMessage,
        this
    );

    sendMessage();

    receiveThread.join();
}

void ChatClient::connectToServer()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Failed to create socket\n";
        return;
    }
    std::cout << "created a socket" << std::endl;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(
        AF_INET,
        serverIP.c_str(),
        &serverAddress.sin_addr);

    int result = connect(clientSocket,(sockaddr*)&serverAddress,sizeof(serverAddress));
    if(result == -1){
        std::cerr<<"Connection failed"<<std::endl;
        close(clientSocket);
        return ;
    }
    std::cout<<"connected to the server"<<std::endl;

    send(
    clientSocket,
    username.c_str(),
    username.size(),
    0
);
}




void ChatClient::receiveMessage(){
    char buffer[BUFFER_SIZE];
    while(true){
        std::memset(buffer,0,sizeof(buffer));
        int byteReceived = recv(
            clientSocket,
            buffer,
            BUFFER_SIZE,
            0
        );
        if(byteReceived == 0){
            std::cout<<"Disconnected from server"<<std::endl;
            return ;
        }
        std::string message(buffer,byteReceived);
        std::cout<<message<<std::endl;
    }
}

void ChatClient::sendMessage()
{
    std::string message;

    while (true)
    {
        std::getline(std::cin, message);

        if (message.empty())
        {
            continue;
        }

        std::cout<<"send message:"<<std::endl;

        int bytesSent = send(
            clientSocket,
            message.c_str(),
            message.size(),
            0
        );

        if (bytesSent == -1)
        {
            std::cout << "Failed to send message.\n";
            break;
        }
    }
}