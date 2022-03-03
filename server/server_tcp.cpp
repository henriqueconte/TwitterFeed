#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "../shared/headers/packet.hpp"
#include "../shared/headers/CommunicationManager.hpp"
#include "headers/SessionManager.hpp"

#define PORT 4000
#include <iostream>

void *authenticateClient(void *data);

SessionManager sessionManager;
CommunicationManager commManager;

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

        // Adding two sessions with username jose to test scenarios with two active sessions. Remove it later.
        sessionManager.tryLogin("jose");
        sessionManager.tryLogin("jose");

        pthread_t authThread;
        std::cout << "Created authentication thread." << std::endl;
        int *socketCopy = (int*) malloc(sizeof (int));
        *socketCopy = newsockfd;
        pthread_create(&authThread, NULL, &authenticateClient, (void *) socketCopy);

        void *authenticationResponse;
        pthread_join(authThread, &authenticationResponse);
        std::cout << "Finished authentication thread." << std::endl << "\n";
        std::string *authString = (std::string *) authenticationResponse;


        // Se a autenticação falhar, a string de resposta contém 'failed'. Isso é uma péssima maneira de verificar o resultado.
        if (authString->find("failed") == std::string::npos) {
            while(true) {		

                // Receives message packet from client to server	
                Packet* messagePacket = commManager.receivePacket(newsockfd);

                if (messagePacket->type == Logout) {
                    std::cout << "User logging out: " << messagePacket->message << std::endl;
                    sessionManager.closeSession(messagePacket->message);
                }
                
                // Sends acknowledge packet from server to client
                commManager.sendPacket(newsockfd, new Packet("Server acknowledges to have received a packet.", Login));

                // Sends message packet from server to client
                commManager.sendPacket(newsockfd, new Packet("Your message was received.", Message));

                // Receives acknowledge packet from client to server
                messagePacket = commManager.receivePacket(newsockfd);

                if (messagePacket->type == Logout) {
                    std::cout << "User logging out: " << messagePacket->message << std::endl;
                    sessionManager.closeSession(messagePacket->message);
                }
		    }
        }
		

		close(newsockfd);
	}	
	
	close(sockfd);
	return 0; 
}

void *authenticateClient(void *data) {
    int *socketCopy = (int *) data;
    int clientSocket = *socketCopy;

    Packet *receivedPacket = new Packet;
    std::string *finalResult;
    Packet *responsePacket = new Packet;

    int readResult = read(clientSocket, receivedPacket, sizeof (Packet));
    if (readResult < 0) {
        std::cout << "Failed to read authentication socket." << std::endl;
        *responsePacket = Packet("Failed to read authentication socket.", Login);
    } else {
        std::cout << "Received data to authenticate. User login: " << receivedPacket->message << std::endl;
        std::string sessionId = sessionManager.tryLogin(receivedPacket->message);
        if(sessionId == "Authentication failed") {
            *responsePacket = Packet("Login failed. Try closing one of the open sessions.", Login);
        } else {
            *responsePacket = Packet(sessionId, Login);
        }
    }

    std::cout << "Sending login response packet to client. Response: " << responsePacket->message << std::endl;
    commManager.sendPacket(clientSocket, new Packet(responsePacket->message, Login));

    return responsePacket->message;
}