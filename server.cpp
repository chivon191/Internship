#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <map>
#include <mutex>
#include "communication.h"

using namespace std;

class Server : public Communication {
public:
    Server();
    ~Server();
    void start();

private:
    string serverIP = "127.0.0.1";
    int port = 8000;
    int listener;
    struct sockaddr_in address;
    socklen_t addresslen = sizeof(address);
    vector<thread> threads;
    map<int, string> clientUsernames;
    mutex mtx;
    bool running;

    void receive_username(int sockfd);
    void error(const char *msg);
    void broadcast_message(const string &message, int sockfd);
    void receive_message(int sockfd);
    void send_message();
};

Server::Server() : Communication(-1), running(true) {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        error("Error opening socket.");
    }

    address.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &address.sin_addr);
    address.sin_port = htons(port);

    int opt = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(listener, (struct sockaddr *)&address, addresslen) < 0) {
        error("ERROR on binding");
    }

    cout << "Server listening on port " << port << endl;
    listen(listener, 5);
}

Server::~Server() {
    close(listener);
    for (auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void Server::error(const char *msg) {
    perror(msg);
    exit(1);
}

void Server::receive_username(int sockfd) {
    Communication comm(sockfd);
    string userName = comm.receive_message();
    if (userName.empty()) {
        cout << "Client disconnected!" << endl;
        close(sockfd);
        return;
    }
    {
        lock_guard<mutex> lock(mtx);
        clientUsernames[sockfd] = userName;
    }
    cout << userName << " connected to server." << endl;
}

void Server::broadcast_message(const string &message, int sockfd) {
    lock_guard<mutex> lock(mtx);
    string fullMessage = clientUsernames[sockfd] + ": " + message;
    for (const auto& [socket, username] : clientUsernames) {
        if (socket != sockfd) {
            if (clientUsernames[sockfd] == "") {
                fullMessage = "Server: " + message;
            }
            Communication comm(socket);
            comm.send_message(fullMessage);
        }
    }
}

void Server::receive_message(int sockfd) {
    Communication comm(sockfd);
    while (true) {
        string message = comm.receive_message();
        if (message.empty()) {
            {
                lock_guard<mutex> lock(mtx);
                cout << clientUsernames[sockfd] << " disconnected!" << endl;
                clientUsernames.erase(sockfd);
            }
            close(sockfd);
            break;
        }
        {
            lock_guard<mutex> lock(mtx);
            cout << clientUsernames[sockfd] << ": " << message << endl;
        }
        broadcast_message(message, sockfd);
    }
}

void Server::send_message() {
    string message;
    while (running) {
        getline(cin, message);
        broadcast_message(message, -1);
    }
}

void Server::start() {
    threads.emplace_back(&Server::send_message, this);
    while (running) {
        int newSocket = accept(listener, (struct sockaddr *)&address, &addresslen);
        if (newSocket < 0) {
            error("ERROR on accept");
        }
        threads.emplace_back(&Server::receive_username, this, newSocket);
        threads.emplace_back(&Server::receive_message, this, newSocket);
    }
}

int main() {
    Server server;
    server.start();
    return 0;
}
