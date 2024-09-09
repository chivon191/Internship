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
int running = 1;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void receive(int clientfd) {
	char buffer[1024];
	int byte_read = recv(clientfd, buffer, 1024, 0);
	while (byte_read > 0) {
		buffer[byte_read] = '\0';
		cout << "Client: " << buffer << endl;
	}
	if (byte_read < 0) {
		perror("ERROR reading from client.");
	}
	close(clientfd);
	cout << "Client disconnected!" << endl;
}

void send_message(int clientfd) {
	string message;
	while(running) {
		getline(cin, message);
		if (message == "exit") { 
			running = 0;
			break;
		}
		send(clientfd, message.c_str(), message.size(), 0);
	}
}

int main()
{
	int sockfd, clientfd, port;
	struct sockaddr_in serverAdd, clientAdd;
	socklen_t clientlen = sizeof(clientAdd);
	vector<thread> threads;

	cout << "Enter port: ";
	cin >> port;
	cin.ignore();

	if(port == 0)
	{
		cerr << "No port provided." << endl;
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("Error opening socket.");
	}

	serverAdd.sin_family = AF_INET;
	serverAdd.sin_addr.s_addr = INADDR_ANY;
	serverAdd.sin_port = htons(port);
			         
	if (bind(sockfd, (struct sockaddr *) &serverAdd, sizeof(serverAdd)) < 0){
		error("ERROR on binding");
	}


	cout << "Server listening on port " << port << endl;
	listen(sockfd,5);			           				       
	clientfd = accept(sockfd, (struct sockaddr *) &clientAdd, &clientlen);					     
	
	if (clientfd < 0){
		error("ERROR on accept");
	}
	
	while (clientfd !=0) {
	    cout << "Client connection established." << endl;
	    threads.push_back(thread(receive, clientfd));
	    thread(send_message, clientfd).detach();
	}

	for (auto &thread : threads) 
		thread.join();

	close(sockfd);								
	return 0; 
}
