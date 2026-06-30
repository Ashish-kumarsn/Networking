
#include <netinet/in.h>

class Client{

    public:
    Client(const char*ip = "127.0.0.1" , int port =8080);
    ~Client();
    void start();


    private:
    void createSocket();
    void connectToServer();
    void sendData();
    void receiveData();
    void closeClient();

    private:
    int clientSocket;
    int port;
    sockaddr_in serverAddress;
    const char* serverIP;

    char buffer[1024];
};