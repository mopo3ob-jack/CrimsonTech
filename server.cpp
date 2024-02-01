#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include "crimson.hpp"

using namespace hdb;
using namespace hdb::ct;

int main(int argc, char** argv) {
	// int fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	// sockaddr_in serverAddress, clientAddress;
	// serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	// serverAddress.sin_port = htons(26000);
	// serverAddress.sin_family = AF_INET;

	// bind(fd, (sockaddr*)&serverAddress, sizeof(sockaddr_in));

	// char buffer[1024];

	// unsigned int length = sizeof(clientAddress); 
	// int n = recvfrom(fd, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddress, &length);
	// buffer[n] = '\0'; 
	// puts(buffer);

	// char msg[] = "Hello From Server!\n";
	// sendto(fd, msg, sizeof(msg), 0, (sockaddr*)&clientAddress, sizeof(sockaddr_in));

	UDPServer server;
	server = UDPServer(nullptr, 26000);

	char buffer[1024];
	int n = 1;
	// do {
		server.recieve(buffer, 1024, true, nullptr);
	// } while (!n);
	// printf("%d\n", n);
	// server.send(0, buffer, 1024);

	return 0;
}