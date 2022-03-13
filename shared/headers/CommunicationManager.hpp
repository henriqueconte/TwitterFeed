#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include <map> 
#include "../headers/packet.hpp"
#include "../headers/Session.hpp"
#include "../headers/User.hpp"

class CommunicationManager {
    public:
        void sendPacket(int socket, Packet* packet);
        Packet* receivePacket(int socket);
        void sendNotification(std::string senderId, Packet* packet, std::list<Session *> activeSessionsList, std::map<std::string, User*> userMap);
};