#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include "../headers/packet.hpp"

class CommunicationManager {
    public:
        void sendPacket(int socket, Packet* packet);
        Packet* receivePacket(int socket);
};