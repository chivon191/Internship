#include <string>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

using namespace std;

void send_message(int clientfd) {
    string message;
    while (1) {
        getline(cin, message);
        send(clientfd, message.c_str(), message.size(), 0);
    }
}

void receive_message(int clientfd) {
    char buffer[1024];
    int byte_read = recv(clientfd, buffer, 1024, 0);
    while (byte_read > 0) {
	    buffer[byte_read] = '\0';
	    cout << "Server: " << buffer << endl;
    }
}

int main()
{
    int clientfd, port;
    struct sockaddr_in serverAdd;
    cout << "Enter port to connect: ";
    cin >> port;
    cin.ignore();
    // creating socket
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    connect(clientfd, (struct sockaddr*)&serverAdd,
            sizeof(serverAdd));

    // sending data
    thread send_thread(send_message, clientfd);
    thread receive_thread(receive_message, clientfd);
    
    send_thread.join();
    receive_thread.join();

    // closing socket
    close(clientfd);
    return 0;
}
