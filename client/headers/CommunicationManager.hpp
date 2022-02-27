#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include "../../shared/headers/packet.hpp"

class CommunicationManager {
    public:
        void sendPacket(int socket, std::string message);
        void receivePacket(int socket);
};