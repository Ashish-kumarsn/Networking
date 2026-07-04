#include <sys/socket.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <netinet/in.h>

class EpollServer
{
private:
    std::unordered_map<int, std::string> Clients;
    int ServerSocket = -1;
    int Port = 8080;
    int EpollFd = -1;
    char buffer[1024];
    sockaddr_in serverAddress;
    sockaddr_in clientAddress;

    void createSocket();
    void bindSocket();
    void listenSocket();
    void createEpoll();
    void addServerSocketToEpoll();
    void acceptClient();

    void handleClientMessages(int ClientFd);
    void broadcastMessage(const std::string &Message, int SenderFd);
    void removeClient(int clientSocket);

public:
    explicit EpollServer(int port = 8080);
    EpollServer();
    ~EpollServer();
    void start();
};
