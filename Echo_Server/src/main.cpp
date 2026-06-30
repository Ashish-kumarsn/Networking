#include "server.hpp"
#include "client.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage:\n";
        std::cout << "./EchoServer server\n";
        std::cout << "./EchoServer client\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server")
    {
        Server server;
        server.start();
    }
    else if (mode == "client")
    {
        Client client;
        client.start();
    }
    else
    {
        std::cout << "Invalid mode\n";
    }

    return 0;
}