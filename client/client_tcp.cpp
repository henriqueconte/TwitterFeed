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

#define PORT 4000
#include <iostream>

int main(int argc, char *argv[]) {
    
    int sockfd, n;
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

    // TODO PONS: Substituir "jose" pelo nome de usuário inserido pelo usuário

    if (AuthenticationManager::login(username)) {
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            printf("ERROR connecting\n");
            close(sockfd);
            exit(0);
        }             

        while(true) {
            std::cout << "Enter the command: " << std::endl;
            std::string inputString;
            getline(std::cin, inputString);
            std::string commandType = inputString.substr(0, inputString.find(" "));
            
            if (commandType == "send") {
                //****
                // Sends message packet from client to server
                //****
                Packet *messagePacket = new Packet(inputString);
                n = write(sockfd, messagePacket, sizeof(Packet)); // Sends message from client to server
                if (n < 0) {
                    std::cout << "Error: message packet could not be written to socket." << std::endl;
                    continue;
                } else {
                    std::cout << "Sent message packet to the server." << std::endl;
                }

                //****
                // Receives acknowledge packet from server to client
                //****
                Packet *receiveAckPacket = new Packet;
                n = read(sockfd, receiveAckPacket, sizeof(Packet));
                if (n < 0) {
                    std::cout << "Error: couldn't read acknowledge packet from server." << std::endl;
                    continue;
                } else {
                    std::cout << "Received acknowledge packet from the server." << std::endl;
                }
                
                //****
                // Receives reply packet from server to client
                //****
                Packet *receivedPacket = new Packet;
                n = read(sockfd, receivedPacket, sizeof(Packet));
                if (n < 0) {
                    std::cout << "Error: response packet could not be read from socket." << std::endl;
                    continue;
                } else {
                    std::cout << "Received message: " << receivedPacket->message << " " << std::endl;
                }
                
                //****
                // Sends acknowledge packet from client to server
                //****
                Packet *sendAckPacket = new Packet("Client acknowledges the server reply.");
                n = write(sockfd, sendAckPacket, sizeof(Packet));
                if (n < 0) {
                    std::cout << "Error: couldn't send acknowledge packet to server." << std::endl;
                    continue;
                } else {
                    std::cout << "Sent acknowledge packet to the server. \n\n";
                }
                
                free(messagePacket);
                free(receivedPacket);
                free(receiveAckPacket);
                free(sendAckPacket);
            } else if (commandType == "follow") {
                // TODO: Implement follow command
            } else {
                std::cout << "Command not recognized. Please try again." << std::endl;
            }
        }
        close(sockfd);
    }
    return 0;
}