#include <string>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <thread>
#include "communication.h"

using namespace std;

class Client : public Communication {
public:
    Client(const string& username);
    ~Client();
    void start();

private:
    string serverIP = "127.0.0.1";
    int port = 8000;
    int sockfd;
    string username;
    bool running;

    void send_message();  
    void receive_message();
    void error(const string& msg);
};

Client::Client(const string& username) 
    : username(username), running(true), Communication(-1) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket.");
    }

    Communication::socketfd = sockfd;  

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &address.sin_addr) <= 0) {
        error("Invalid address or address not supported.");
    }

    if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        error("Connection failed.");
    }
    Communication::send_message(username);
}


Client::~Client() {
    close(sockfd);
}

void Client::error(const string& msg) {
    cerr << msg << endl;
    exit(1);
}

void Client::receive_message() {
    while (running) {
        string message = Communication::receive_message();
        if (message.empty()) {
            cout << "Server disconnected!" << endl;
            running = false;
            close(sockfd);
            return;
        }
        cout << message << endl;
    }
}

void Client::send_message() { 
    string message;
    while (running) {
        getline(cin, message);
        Communication::send_message(message);
    }
    close(sockfd);  
}

void Client::start() {
    thread send_thread(&Client::send_message, this);
    thread receive_thread(&Client::receive_message, this);
    send_thread.join();
    receive_thread.join();
}

int main() {
    string username;
    cout << "Enter your username: ";
    cin >> username;
    cin.ignore();
    Client client(username);
    client.start();
    return 0;
}