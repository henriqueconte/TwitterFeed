#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <mutex>

#include "../shared/headers/packet.hpp"
#include "../shared/headers/CommunicationManager.hpp" 

#define PORT 4000
#define SESSIONLENGTH 8

void disconnect(int socket);

int sockfd;
std::string sessionId;
std::mutex clientMutex; 

CommunicationManager commManager;

int main(int argc, char *argv[]) {

    // Responsible for handling interruptions like CTRL C.
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = disconnect;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    int n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    std::string username;
    char buffer[256];

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname username\n", argv[0]);
        exit(0);
    }
    
    server = gethostbyname(argv[1]);
    username = argv[2];

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket\n");

    serv_addr.sin_family = AF_INET;     
    serv_addr.sin_port = htons(PORT);    
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting\n");
        close(sockfd);
        exit(0);
    }             

    commManager.sendPacket(sockfd, new Packet(username, Login));
    Packet* loginPacket = commManager.receivePacket(sockfd);

    std::cout << "Authentication result: " << loginPacket->message << std::endl << "\n";

    if (loginPacket->length == SESSIONLENGTH) {
        commManager.sendPacket(sockfd, new Packet("Client logged in successfully!.", Login));
        sessionId = loginPacket->message;
        while(true) {
            std::cout << "Enter the command: " << std::endl;
            std::string inputString;
            getline(std::cin, inputString);
            std::string commandType = inputString.substr(0, inputString.find(" "));
            
            if (commandType == "send") {

                // Sends message packet from client to server
                commManager.sendPacket(sockfd, new Packet(inputString, Message));

                // Receives acknowledge packet from server to client
                commManager.receivePacket(sockfd);

                // Receives reply packet from server to client
                commManager.receivePacket(sockfd);
                
                // Sends acknowledge packet from client to server
                commManager.sendPacket(sockfd, new Packet("Client acknowledges the server reply.", Message));

            } else if (commandType == "follow") {
                // TODO: Implement follow command
            } else {
                std::cout << "Command not recognized. Please try again." << std::endl;
            }
        }
    }
    return 0;
}

void disconnect(int signal) {
    std::cout << "Closing the session... " << std::endl;
    clientMutex.lock();
    commManager.sendPacket(sockfd, new Packet(sessionId, Logout));
    clientMutex.unlock();
    clientMutex.lock();
    commManager.receivePacket(sockfd);
    clientMutex.unlock();
    close(sockfd);
    exit(1);
}