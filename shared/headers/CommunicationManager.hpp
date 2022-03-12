#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include "../headers/packet.hpp"
#include "../headers/Session.hpp"

class CommunicationManager {
    public:
        void sendPacket(int socket, Packet* packet);
        Packet* receivePacket(int socket);
        void sendNotification(std::string senderId, Packet* packet, std::list<Session *> activeSessionsList);
};