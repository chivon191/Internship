#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include <iostream> 
#include <string> 
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std; 

class Communication 
{ 
public:
    Communication(int socket_fd);
    int socket_fd;
    void send_message(const string& message);
    string receive_message();
};

Communication::Communication(int socket_fd) : socket_fd(socket_fd) {}

void Communication::send_message(const string& message) {
    send(socket_fd, message.c_str(), message.size(), 0);
}

std::string Communication::receive_message() {
    char buffer[1024];
    int byte_read = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (byte_read <= 0) {
        return "";
    }
    buffer[byte_read] = '\0';
    return string(buffer);
}

#endif