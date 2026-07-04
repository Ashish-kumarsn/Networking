#include "ChatServer.hpp"
#include "ChatClient.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:\n";
        std::cout << "./Multi_Client server\n";
        std::cout << "./Multi_Client client\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server")
    {
        ChatServer server;
        server.start();
    }
    else if (mode == "client")
    {
        std::string username;

        std::cout << "Enter username: ";
        std::getline(std::cin, username);

        if (username.empty())
        {
            std::cout << "Username cannot be empty.\n";
            return 1;
        }

        ChatClient client(username);
        client.start();
    }
    else
    {
        std::cout << "Invalid argument.\n";
        std::cout << "Use 'server' or 'client'.\n";
    }

    return 0;
}