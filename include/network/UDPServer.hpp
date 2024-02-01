#ifndef HDB_CT_UDPSERVER_HPP
#define HDB_CT_UDPSERVER_HPP

#include <sys/types.h>
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

namespace hdb {

namespace ct {

class UDPServer {
public:
	UDPServer() {}

	UDPServer(const char* address, unsigned short port);

	~UDPServer();

	void operator=(UDPServer& server) {
		this->fd = server.fd;
		this->serverAddress = server.serverAddress;

		server.fd = -1;
	}

	void operator=(UDPServer&& server) {
		this->fd = server.fd;
		this->serverAddress = server.serverAddress;
	}

	struct SocketAddress : public sockaddr_in {
		bool operator==(SocketAddress socket) const {
			return
				this->sin_addr.s_addr == socket.sin_addr.s_addr &&
				this->sin_family == socket.sin_family &&
				this->sin_port == socket.sin_port;
		}

		operator sockaddr() const {
			return *(sockaddr*)this;
		}
	};

	int recieve(void* data, size_t maxLength, bool trackClient, SocketAddress* client);
	int send(unsigned int client, void* data, size_t length);

	std::vector<SocketAddress> clients;

private:
	int fd = -1;
	SocketAddress serverAddress;
};

}

}

#endif