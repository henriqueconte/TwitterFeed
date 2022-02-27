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

            //****
            // Receives message packet from client to server
            //****	
			Packet *receivedPacket = new Packet;
			n = read(newsockfd, receivedPacket, sizeof(Packet));
			if (n < 0) {
				std::cout << "Error: message packet could not be read from socket." << std::endl;
				continue;
			} else {
                std::cout << "Received message: " << receivedPacket->message << " " << std::endl;
            }
			
            //****
            // Sends acknowledge packet from server to client
            //****	
			Packet *sendAckPacket = new Packet("Server acknowledges to have received a packet.");
			n = write(newsockfd, sendAckPacket, sizeof(Packet));			
			if (n < 0) {
				std::cout << "Error: send acknowledge packet could not be written to socket." << std::endl;
				continue;
			} else {
                std::cout << "Sent acknowledge packet to client." << std::endl;
            }

            //****
            // Sends message packet from server to client
            //****	
			Packet *responsePacket = new Packet("Your message was received.");
			n = write(newsockfd, responsePacket, sizeof(Packet));			
			if (n < 0) {
				std::cout << "Error: response packet could not be written to socket." << std::endl;
				continue;
			} else {
                std::cout << "Sent response packet to client." << std::endl;
            }

            //****
            // Receives acknowledge packet from client to server
            //****	
			Packet *receiveAckPacket = new Packet;
			n = read(newsockfd, receiveAckPacket, sizeof(Packet));
			if (n < 0) {
				std::cout << "Error: couldn't read acknowledge packet from socket." << std::endl;
				continue;
			} else {
                std::cout << "Received acknowledge packet from the client. \n\n";
            }
			
			free(receivedPacket);
			free(responsePacket);
            free(sendAckPacket);
            free(receiveAckPacket);
		}

		close(newsockfd);
	}	
	
	close(sockfd);
	return 0; 
}