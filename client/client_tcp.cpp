#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../shared/headers/packet.hpp"
#include "../shared/headers/AuthenticationManager.hpp"
#include "../shared/headers/CommunicationManager.hpp" 

#define PORT 4000
#include <iostream>

int main(int argc, char *argv[]) {
    
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    std::string username;
    char buffer[256];
    CommunicationManager commManager = CommunicationManager();

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

        // commManager.sendPacket(sockfd, username, 0);
        commManager.sendPacket(sockfd, new Packet(username, Login));
        Packet* loginPacket = commManager.receivePacket(sockfd);

        std::cout << "The message is: " << loginPacket->message << std::endl;
        std::cout << "Difference between packet and message: " << strcmp(loginPacket->message, "Login succeeded") << std::endl;
        if (strcmp(loginPacket->message, "Login succeeded") == 0) {
            // commManager.sendPacket(sockfd, "Client logged in successfully!.", 0); 
            commManager.sendPacket(sockfd, new Packet("Client logged in successfully!.", Login)); 
            while(true) {
                std::cout << "Enter the command: " << std::endl;
                std::string inputString;
                getline(std::cin, inputString);
                std::string commandType = inputString.substr(0, inputString.find(" "));
                
                if (commandType == "send") {

                    // Sends message packet from client to server
                    // commManager.sendPacket(sockfd, inputString, 1);
                    commManager.sendPacket(sockfd, new Packet(inputString, Message));

                    // Receives acknowledge packet from server to client
                    commManager.receivePacket(sockfd);

                    // Receives reply packet from server to client
                    commManager.receivePacket(sockfd);
                    
                    // Sends acknowledge packet from client to server
                    // commManager.sendPacket(sockfd, "Client acknowledges the server reply.", 1);
                    commManager.sendPacket(sockfd, new Packet("Client acknowledges the server reply.", Message));

                } else if (commandType == "follow") {
                    // TODO: Implement follow command
                } else {
                    std::cout << "Command not recognized. Please try again." << std::endl;
                }
            }
        } else {
            std::cout << "Failed to login. " << loginPacket->message << std::endl;
        }

        close(sockfd);
    return 0;
}