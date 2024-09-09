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

void receive(int sock) {
	char buffer[1024];
	int byte_read;
	while (true) {
		byte_read = recv(sock, buffer, sizeof(buffer), 0);
		if (byte_read <= 0) 
		{
			cout << "Client disconnected!" << endl;
			close(sock);
			break;
		}
		buffer[byte_read] = '\0';
		cout << "Client: " << buffer << endl;
	}
}

void send_message(int sock) {
	string message;
	while(running) {
		getline(cin, message);
		if (message == "exit") { 
			running = 0;
			break;
		}
		send(sock, message.c_str(), message.size(), 0);
	}
}

int main()
{
	int server, client, port;
	struct sockaddr_in address;
	vector<thread> threads;

	cout << "Enter port: ";
	cin >> port;
	cin.ignore();

	if(port == 0)
	{
		cerr << "No port provided." << endl;
		return 1;
	}

	server = socket(AF_INET, SOCK_STREAM, 0);
	if(server < 0)
	{
		error("Error opening socket.");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	socklen_t addresslen = sizeof(address);
			         
	if (bind(server, (struct sockaddr *)&address, addresslen) < 0)
	{
		error("ERROR on binding");
	}
	
	cout << "Server listening on port " << port << endl;
	listen(server,5);						     
		
	while (running) 
	{
	        client = accept(server, (struct sockaddr *) &address, &addresslen);
	        if (client < 0) {
	              error("ERROR on accept");
	        }
		cout << "Client connection established." << endl;
	    	threads.push_back(thread(receive, client));
	    	thread(send_message, client).detach();
	}

	for (auto &thread : threads) 
		thread.join();

	close(server);								
	return 0; 
}
