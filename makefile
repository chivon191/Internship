all: server client

server: server.cpp communication.cpp
	g++ -o server server.cpp communication.cpp -pthread


client: client.cpp communication.cpp
	g++ -o client client.cpp communication.cpp -pthread

clean:
	rm -f server client
