#ifndef _PACKET_
#define _PACKET_

#define BUFFER_LENGTH 256

class Packet {
    public:
        int type;          // Packet type (ex: DATA | CMD)
        int seqn;          // Sequence number (packet ID)
        int length;        // Payload size (packet size)
        long int timestamp;       // Timestamp from when the packet was created
        char* message[BUFFER_LENGTH];         // Packet message (like "bla bla bla")
};

#endif