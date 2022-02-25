#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../shared/headers/packet.hpp"

#define PORT 4000

int main(int argc, char *argv[]) {
    
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    Packet packet;
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

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("ERROR connecting\n");

    while(true) {
        printf("Enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);

        packet.type = 1;
        packet.seqn = 2;
        packet.length = 256;
        packet.timestamp = 0;

        /* write in the socket */
        n = write(sockfd, buffer, 256);
        if (n < 0) 
            printf("ERROR writing to socket\n");

        bzero(buffer,256);

        /* read from the socket */
        n = read(sockfd, buffer, 256);
        if (n < 0) 
            printf("ERROR reading from socket\n");

        printf("%s\n",buffer);
    }

    close(sockfd);
    return 0;
}