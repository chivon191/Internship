#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <map>
#include <mutex>

using namespace std;

class Server {
public:
    Server(int port);
    ~Server();
    void start();

private:
    int serverSocket;
    struct sockaddr_in address;
    socklen_t addresslen = sizeof(address);
    vector<thread> threads;
    map<int, string> clientUsernames;
    mutex mtx;  
    bool running;

    void receive_username(int clientSocket);
    void error(const char *msg);
    void broadcast_message(const string &message, int clientSocket);
    void receive_message(int clientSocket);
    void send_message(int clientSocket);
};

Server::Server(int port) : running(true) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        error("Error opening socket.");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&address, addresslen) < 0) {
        error("ERROR on binding");
    }

    cout << "Server listening on port " << port << endl;
    listen(serverSocket, 5);
}

Server::~Server() {
    close(serverSocket);
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

void Server::receive_username(int clientSocket) {
    char buffer[1024];
    int byte_read;
    byte_read = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (byte_read <= 0) {
        cout << "Client disconnected!" << endl;
        close(clientSocket);
        return;
    }
    buffer[byte_read] = '\0';
    string userName(buffer);
    {
        lock_guard<mutex> lock(mtx);
        clientUsernames[clientSocket] = userName;
    }
    cout << userName << " connected to server." << endl;
}

void Server::broadcast_message(const string &message, int clientSocket) {
    lock_guard<mutex> lock(mtx);
    string fullMessage = clientUsernames[clientSocket] + ": " + message;
    for (const auto& [socket, username] : clientUsernames) {
        if (socket != clientSocket) {
            if (clientUsernames[clientSocket] == "") {
                fullMessage = "Server: " + message;
            }
            send(socket, fullMessage.c_str(), fullMessage.size(), 0);
        }
    }
}

void Server::receive_message(int clientSocket) {
    char buffer[1024];
    int byte_read;
    while (true) {
        byte_read = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (byte_read <= 0) {
            {
                lock_guard<mutex> lock(mtx);
                cout << clientUsernames[clientSocket] << " disconnected!" << endl;
                clientUsernames.erase(clientSocket);
            }
            close(clientSocket);
            break;
        }
        buffer[byte_read] = '\0';
        string message = buffer;
        {
            lock_guard<mutex> lock(mtx);
            cout << clientUsernames[clientSocket] << ": " << message << endl;
        }
        broadcast_message(message, clientSocket);
    }
}

void Server::send_message(int clientSocket) {
    string message;
    while (running) {
        getline(cin, message);
        if (message == "exit") {
            running = false;
            break;
        }
        broadcast_message(message, -1);
    }
}

void Server::start() {
    while (running) {
        int clientSocket = accept(serverSocket, (struct sockaddr *)&address, &addresslen);
        if (clientSocket < 0) {
            error("ERROR on accept");
        }
        thread(&Server::receive_username, this, clientSocket).detach();
        thread(&Server::receive_message, this, clientSocket).detach();
        thread(&Server::send_message, this, clientSocket).detach();
    }
} 

int main() {
    int port;
    cout << "Enter port: ";
    cin >> port;
    cin.ignore();

    if (port == 0) {
        cerr << "No port provided." << endl;
        return 1;
    }

    Server server(port);
    server.start();

    return 0;
}
