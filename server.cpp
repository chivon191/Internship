#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

class Server {
public:
	Server(int port);
	~Server();
	void start();

private:
	int serverSocket, clientSocket;
    struct sockaddr_in address;
    socklen_t addresslen = sizeof(address);
	vector<thread> threads;
	bool running;
	void error(const char *msg);
	void handle_client(int clientSocket);
	void receive_message(int clientSocket);
	void send_message(int clientSocket);
};

Server::Server(int port) : running(true) {
	struct sockaddr_in address;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0)
	{
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

void Server::handle_client(int clientSocket) {
    threads.push_back(thread(&Server::receive_message, this, clientSocket));
    thread(&Server::send_message, this, clientSocket).detach();
}

void Server::receive_message(int clientSocket) {
    char buffer[1024];
    int byte_read;
    while (true) {
        byte_read = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (byte_read <= 0) {
            cout << "Client disconnected!" << endl;
            close(clientSocket);
            break;
        }
        buffer[byte_read] = '\0';
        cout << "Client: " << buffer << endl;
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
        send(clientSocket, message.c_str(), message.size(), 0);
    }
}

void Server::start() {
    while (running) {
        int clientSocket = accept(serverSocket, (struct sockaddr *)&address, &addresslen);
        if (clientSocket < 0) {
            error("ERROR on accept");
        }
        cout << "Client connection established." << endl;
        handle_client(clientSocket);
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