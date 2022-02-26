#ifndef _PACKET_
#define _PACKET_

#include <iostream>

#define BUFFER_LENGTH 256

class Packet {
    public:
        int type;          // Packet type (ex: DATA | CMD)
        int seqn;          // Sequence number (packet ID)
        int length;        // Payload size (packet size)
        long int timestamp;       // Timestamp from when the packet was created
        char message[BUFFER_LENGTH];         // Packet message (like "bla bla bla")

        Packet() {}

        Packet(std::string msg) {
            type = 1; // TODO: Update this when values of type are defined
            bzero(&message, BUFFER_LENGTH);
            strncpy(message, msg.c_str(), BUFFER_LENGTH);
            timestamp = time(NULL);
            length = strlen(message);
        }
};

#endif