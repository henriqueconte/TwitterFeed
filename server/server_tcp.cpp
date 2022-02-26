#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "../shared/headers/packet.hpp"

#define PORT 4000
#include <iostream>

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
    
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	bzero(&(serv_addr.sin_zero), 8);     
    
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		printf("ERROR on binding");
	
    // Allows 5 clients at the same time
	listen(sockfd, 5);
	
	clilen = sizeof(struct sockaddr_in);
	while (true) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd == -1) {
			printf("ERROR on accept");
			continue;
		}

		while(true) {			
			Packet *receivedPacket = new Packet;
			n = read(newsockfd, receivedPacket, sizeof(Packet));
			if (n < 0) {
				std::cout << "Error: message packet could not be read from socket." << std::endl;
				continue;
			}
			std::cout << "Received message: " << receivedPacket->message << " " << std::endl;

			Packet *responsePacket = new Packet("Your message was received.");
			n = write(newsockfd, responsePacket, sizeof(Packet));			
			if (n < 0) {
				std::cout << "Error: response packet could not be written to socket." << std::endl;
				continue;
			}

			free(receivedPacket);
			free(responsePacket);
		}

		close(newsockfd);
	}	
	
	close(sockfd);
	return 0; 
}