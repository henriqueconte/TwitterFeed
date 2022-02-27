#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include "Session.hpp"
#include "packet.hpp"

class CommunicationManager {
    public:
        CommunicationManager();
        void sendPacket(int socket, Packet* packet);
        void receivePacket();
};