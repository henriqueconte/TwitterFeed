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
    char buffer[256];

    if (argc < 2) {
        fprintf(stderr,"usage %s hostname\n", argv[0]);
        exit(0);
    }
    
    server = gethostbyname(argv[1]);
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
    if (AuthenticationManager::login("jose")) {
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
            printf("ERROR connecting\n");

        while(true) {
            std::cout << "Enter the message: " << std::endl;
            std::string inputString;
            getline(std::cin, inputString);

            Packet *messagePacket = new Packet(inputString);
            n = write(sockfd, messagePacket, sizeof(Packet));
            if (n < 0) {
                std::cout << "Error: message packet could not be written to socket." << std::endl;
                continue;
            }

            Packet *receivedPacket = new Packet;
            n = read(sockfd, receivedPacket, sizeof(Packet));
            if (n < 0) {
                std::cout << "Error: response packet could not be read from socket." << std::endl;
                continue;
            }
            
            std::cout << "Received message: " << receivedPacket->message << " " << std::endl;
            
            free(messagePacket);
            free(receivedPacket);
        }

        close(sockfd);
    }

    return 0;
}