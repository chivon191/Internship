#include <iostream> 
#include <string> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "communication.h"

Communication::Communication(int socketfd) {
    this->socketfd = socketfd;
}

void Communication::send_message(const string& message) {
    send(socketfd, message.c_str(), message.size(), 0);
}

std::string Communication::receive_message() {
    char buffer[1024];
    int byte_read = recv(socketfd, buffer, sizeof(buffer), 0);
    if (byte_read <= 0) {
        return "";
    }
    buffer[byte_read] = '\0';
    return string(buffer);
}