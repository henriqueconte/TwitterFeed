#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include "../headers/CommunicationManager.hpp"
#include <list> 

void CommunicationManager::sendPacket(int socket, Packet* packet) {
    int responseCode = write(socket, packet, sizeof(Packet)); // Sends message from client to server
    if (responseCode < 0) {
        std::cout << "Error: packet could not be written to socket." << std::endl;
    } else {
        std::cout << "Sent packet to the server. Packet message: " << packet->message << std::endl;
    }
}

Packet* CommunicationManager::receivePacket(int socket) {
    Packet *receivedPacket = new Packet;
    int responseCode = read(socket, receivedPacket, sizeof(Packet));
    if (responseCode < 0) {
        std::cout << "Error: response packet could not be read from socket." << std::endl;
    } else {
        std::cout << "Received message: " << receivedPacket->message << std::endl;
    }
    return receivedPacket;
}