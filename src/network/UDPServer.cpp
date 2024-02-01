#include "../../include/network/UDPServer.hpp"
#include <stdio.h>

namespace hdb {

namespace ct {

UDPServer::UDPServer(const char* address, unsigned short port) {
	this->~UDPServer();

	serverAddress.sin_addr.s_addr = address ? inet_addr(address) : htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);
	serverAddress.sin_family = AF_INET;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	bind(fd, (sockaddr*)&serverAddress, sizeof(SocketAddress));
}

UDPServer::~UDPServer() {
	if (fd != -1) {
		close(fd);
		fd = -1;
	}
}

int UDPServer::recieve(void* data, size_t maxLength, bool trackClient, SocketAddress* client) {
	socklen_t length = sizeof(SocketAddress); 
	
	SocketAddress clientBackup;
	if (!client) {
		client = &clientBackup;
	}

	int n = recvfrom(fd, data, maxLength, 0, (sockaddr*)client, &length);

	if (trackClient) {
		for (auto i: clients) {
			if (i == *client) {
				break;
			}
		}

		clients.push_back(*client);
	}

	return n;
}

int UDPServer::send(unsigned int client, void* data, size_t length) {
	SocketAddress sa = clients.at(client);
	return sendto(fd, data, length, 0, (sockaddr*)&sa, sizeof(SocketAddress));
}

}

}