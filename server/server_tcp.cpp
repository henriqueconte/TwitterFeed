#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "../common/headers/packet.h"

#define PORT 4000

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	packet packet;
    
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
	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1) 
		printf("ERROR on accept");
	
	memset(&packet, 0, sizeof (packet));
	
	/* read from the socket */
	n = read(newsockfd, &packet, sizeof(packet));
	if (n < 0) 
		printf("ERROR reading from socket");

	printf("type: %d\n", packet.type);
	printf("seqn: %d\n", packet.seqn);
	printf("length: %d\n", packet.length);
	printf("timestamp: %d\n", packet.timestamp);
	printf("payload: %s\n", packet._payload);
	
	/* write in the socket */ 
	n = write(newsockfd,"I got your message", 18);
    
	if (n < 0) 
		printf("ERROR writing to socket");

	close(newsockfd);
	close(sockfd);
	return 0; 
}