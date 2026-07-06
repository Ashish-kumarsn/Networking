#include "Client.hpp"
#include "Server.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " server|client\n";
        return 1;
    }

    std::string Mode = argv[1];

    if (Mode == "server")
    {
        EpollServer Server;
        Server.start();
    }
    else if (Mode == "client")
    {
        std::string UserName;

        std::cout << "Enter your username: ";
        std::getline(std::cin, UserName);

        if (UserName.empty())
        {
            std::cout << "Username cannot be empty.\n";
            return 1;
        }

        ChatClient Client(UserName);
        Client.start();
    }
    else
    {
        std::cout << "Invalid mode.\n";
        std::cout << "Usage: " << argv[0] << " server|client\n";
        return 1;
    }

    return 0;
}