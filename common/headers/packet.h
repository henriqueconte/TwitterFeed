#ifndef _PACKET_
#define _PACKET_

typedef struct __packet {
    uint16_t type;          // Packet type (ex: DATA | CMD)
    uint16_t seqn;          // Sequence number (packet ID)
    uint16_t length;        // Payload size (packet size)
    time_t timestamp;       // Timestamp from when the packet was created
    char* _payload;         // Packet information (like "bla bla bla")
} packet;

#endif