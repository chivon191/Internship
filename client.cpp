#include <string>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <thread>

using namespace std;

void send_message(int sock) {
    string message;
    while (true) {
        getline(cin, message);
        if (message == "exit") {
            break;
        }
        send(sock, message.c_str(), message.size(), 0);
    }
}

void receive_message(int sock) {
    char buffer[1024];
    int byte_read;
    while (true) 
    {
	int byte_read = recv(sock, buffer, sizeof(buffer), 0);
        if (byte_read <= 0) {
                cout << "Server disconnected" <<endl;
	        break;
        }
        buffer[byte_read] = '\0';
	cout << "Server: " << buffer << endl;
    }
}

int main()
{
    int sock_client, port;
    struct sockaddr_in address;
    cout << "Enter port to connect: ";
    cin >> port;
    cin.ignore();
    // creating socket
    sock_client = socket(AF_INET, SOCK_STREAM, 0);
    // specifying address
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    
    const char* ip_loopback = "127.0.0.1";
    inet_pton(AF_INET, ip_loopback, &address.sin_addr);

    // sending connection request
    connect(sock_client, (struct sockaddr*)&address,
            sizeof(address));

    // sending data
    thread send_thread(send_message, sock_client);
    thread receive_thread(receive_message, sock_client);
    
    send_thread.join();
    receive_thread.join();

    // closing socket
    close(sock_client);
    return 0;
}
