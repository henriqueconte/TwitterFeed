#ifndef _PACKET_
#define _PACKET_

#include <iostream>

#define BUFFER_LENGTH 256

enum PacketType { Login, Message, Follow, Logout };

class Packet {
    public:
        PacketType type;          // Packet type (ex: Login | Message | Logout)
        int seqn;          // Sequence number (packet ID)
        int length;        // Payload size (packet size)
        long int timestamp;       // Timestamp from when the packet was created
        char message[BUFFER_LENGTH];         // Packet message (like "bla bla bla")

        Packet() {}

        Packet(std::string msg, PacketType packetType) {
            type = packetType; 
            bzero(&message, BUFFER_LENGTH);
            strncpy(message, msg.c_str(), BUFFER_LENGTH);
            timestamp = time(NULL);
            length = strlen(message);
        }
};

#endif