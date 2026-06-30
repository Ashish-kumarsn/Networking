#include<netinet/in.h>
#include <sys/socket.h> 


class Server{
    public:

    Server(int port=8080);
    ~Server();
    void start();

    private:

    void createSocket();
    void bindSocket();
    void startListening();
    void acceptClient();
    void receiveData();
    void sendData();
    void closeServer();
    

    private:
    int serverSocket;
    int clientSocket;
    int port;

    sockaddr_in serverAddress;
    sockaddr_in clientAddress;
    socklen_t clientAddressLength;

    char buffer[1024];

};