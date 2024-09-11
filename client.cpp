#include <string>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <thread>

using namespace std;

class Client {
public:
    Client(const string& serverIP, int port, const string& username);
    ~Client();
    void start();

private:
    int clientSocket;
    struct sockaddr_in address;
    string serverIP;
    string username;
    bool running;

    void send_message();
    void receive_message();
    void error(const string& msg);
};

Client::Client(const string& serverIP, int port, const string& username) 
    : serverIP(serverIP), username(username), running(true) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        error("Error opening socket.");
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &address.sin_addr) <= 0) {
        error("Invalid address or address not supported.");
    }

    if (connect(clientSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        error("Connection failed.");
    }

    string username_message = username + '\0';  
    send(clientSocket, username_message.c_str(), username_message.size(), 0);
}

Client::~Client() {
    close(clientSocket);
}

void Client::error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

void Client::receive_message() {
    char buffer[1024];
    int byteRead;
    while (running) {
        byteRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (byteRead <= 0) {
            cout << "Server disconnected!" << endl;
            running = false;
            break;
        }
        buffer[byteRead] = '\0';
        cout << buffer << endl;
    }
}

void Client::send_message() {
    string message;
    while (running) {
        getline(cin, message);
        if (message.empty()) {
            continue;
        }
        if (message == "exit") {
            running = false;
            break;
        }
        send(clientSocket, message.c_str(), message.size(), 0);
    }
}

void Client::start() {
    thread send_thread(&Client::send_message, this);
    thread receive_thread(&Client::receive_message, this);
    send_thread.join();
    receive_thread.join();
}

int main() {
    string server_ip = "127.0.0.1";
    int port;
    string username;

    cout << "Enter your username: ";
    cin >> username;
    cin.ignore();

    cout << "Enter port to connect: ";
    cin >> port;
    cin.ignore();

    Client client(server_ip, port, username);
    client.start();

    return 0;
}
