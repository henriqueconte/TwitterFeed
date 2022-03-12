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

void *serveClient(void *data);
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
	listen(sockfd, 25);
	
	clilen = sizeof(struct sockaddr_in);
	while (true) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd == -1) {
			printf("ERROR on accept");
			continue;
		}

        // Adding two sessions with username jose to test scenarios with two active sessions. Remove it later.
        // sessionManager.tryLogin("jose");
        // sessionManager.tryLogin("jose");

        pthread_t authThread;
        std::cout << "Created authentication thread." << std::endl;
        int *socketCopy = (int*) malloc(sizeof (int));
        *socketCopy = newsockfd;
        pthread_create(&authThread, NULL, &authenticateClient, (void *) socketCopy);

        void *authenticationResponse;
        pthread_join(authThread, &authenticationResponse);
        std::cout << "Finished authentication thread." << std::endl << "\n";
        Session *session = (Session *) authenticationResponse;
        session->socket = socketCopy;

        // If the session is open, the authentication was a success.
        if (session->sessionStatus == Open) {
            pthread_t serviceThread;
            std::cout << "Created serving thread." << std::endl;
            pthread_create(&serviceThread, NULL, &serveClient, (void *) session);

        }
        // TALVEZ TENHA QUE INICIAR LEITURA DE NOTIFICAÇÕES AQUI! 
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
    Session *session = new Session;

    int readResult = read(clientSocket, receivedPacket, sizeof (Packet));
    if (readResult < 0) {
        std::cout << "Failed to read authentication socket." << std::endl;
        *responsePacket = Packet("Failed to read authentication socket.", Login);
    } else {
        std::cout << "Received data to authenticate. User login: " << receivedPacket->message << std::endl;
        session = sessionManager.tryLogin(receivedPacket->message);
        if(session->sessionStatus == Failed) {
            *responsePacket = Packet("Login failed. Try closing one of the open sessions.", Login);
        } else {
            *responsePacket = Packet(session->sessionId, Login);
        }
    }

    std::cout << "Sending login response packet to client. Response: " << responsePacket->message << std::endl;
    commManager.sendPacket(clientSocket, new Packet(responsePacket->message, Login));
    commManager.receivePacket(clientSocket);

    return session;
}

void *serveClient(void *data) {
    Session *session = (Session *) data;
    int clientSocket = *session->socket;

    bool shouldExit = false;

    while(!shouldExit) {		
        //  Receives message packet from client to server	
        Packet* messagePacket = commManager.receivePacket(clientSocket);

        // Handle command type accordingly
        switch (messagePacket->type) {
            case Login: // TODO: Handle login here
                commManager.sendPacket(clientSocket, new Packet("Your login message was received.", Login));

                // Receives acknowledge packet from client to server
                messagePacket = commManager.receivePacket(clientSocket);
                break;
            case Message:

                // Sends acknowledge packet from server to client
                commManager.sendPacket(clientSocket, new Packet("Server acknowledges to have received a packet.", Login));

                // Sends message packet from server to client
                // commManager.sendPacket(clientSocket, new Packet("Your message content was received.", Message));
                commManager.sendNotification(session->connectedUserId, messagePacket, sessionManager.activeSessionsList);

                // Receives acknowledge packet from client to server
                messagePacket = commManager.receivePacket(clientSocket);
                break;
            case Follow:
                break;
            case Logout:
                std::cout << "User logging out: " << messagePacket->message << std::endl;
                sessionManager.closeSession(messagePacket->message);
                shouldExit = true;
                break;
        }

        delete messagePacket;
    }

    close(clientSocket);

    return 0;
}