#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char** argv) {
	char buffer[100]; 
	char *message = "Hello Server"; 
	int sockfd, n; 
	struct sockaddr_in servaddr; 
	  
	// clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(26000); 
	servaddr.sin_family = AF_INET; 
	  
	// create datagram socket 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	 
	// connect to server 
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
		printf("\n Error : Connect Failed \n"); 
		return 1;
	}

	// request to send datagram 
	// no need to specify server address in sendto 
	// connect stores the peers IP and port 
	sendto(sockfd, message, 100, 0, (struct sockaddr*)NULL, sizeof(servaddr));
	  
	// waiting for response 
	recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
	puts(buffer); 
		
	// close the descriptor 
	close(sockfd); 

	return 0;
}