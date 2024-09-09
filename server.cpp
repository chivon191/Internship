#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>

using namespace std;

void error(char *msg)
{
	perror(msg);
	exit(1);
}

void receive(int clientfd) {
	char buffer[1024];
	int byte_read = fread(clientfd, buffer, 1024);
	while (byte_read > 0) {
		cout << "Client: " << buffer << endl;
	}
	close(clientfd);
	cout << "Client disconnected!" << endl;
}

void send(int sockfd) {
	string message;
	while (1) {
		getline(cin, message);
		if (message == "exit") 
			close(sockfd);
		send(clientfd, message.c_str(), message.size(), 0);
	}
}

int main()
{
	int sockfd, clientfd, port;
	char buffer[256];
	struct sockaddr_in serverAdd, clientAdd;
	
	cout << "Enter port: ";
	cin >> port;
	if(port == 0)
	{
		cout << stderr << "No port provided." << endl;
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		cout << "Error opening socket." << endl;
		return 1;
	}

	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = INADDR_ANY;
	serverAdd.sin_port = htons(port);
			         
	if (bind(sockfd, (struct sockaddr *) &serverAdd, sizeof(serverAdd)) < 0){
		cout << "ERROR on binding" << endl;
		return 1;
	}

	cout << "Server listening on port " << port << endl;
	listen(sockfd,5);			           				       
	clientfd = accept(sockfd, (struct sockaddr *) &clientAdd, &sizeof(clientAdd);					     
	
	if (clientfd < 0){
		cout << "ERROR on accept" << endl;
		return 1;
	}
	
	if (clientfd != 0) {
		cout << "Client connection established." << endl;
		pthread_t.push_back(thread(receive, clientfd));
		thread(send, clientfd).detach();
	}

	for (auto &thread : threads) 
		thread.join();

	close(clientfd);							
	close(sockfd);								
	return 0; 
}
