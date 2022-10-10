#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>

#include "../headers/server_ring.hpp"
#include "../../shared/headers/packet.hpp"
#include "../../shared/headers/CommunicationManager.hpp"

int ringPorts[] = { 4000, 4001, 4002, 4003, 4004, 4005, 4006, 4007, 4008, 4009 };
char *ringAddress = "127.0.0.1"; 

using namespace std;
void heartbeat(void *ringData);
void startElection(ServerRing* ring);

ServerRing* ServerRing::initServerRing() {
    ServerRing *ring = (ServerRing *)malloc(sizeof(ServerRing));
 
    ring->ringPorts[0] = 4000;
    ring->ringPorts[1] = 4001;
    ring->ringPorts[2] = 4002;
    ring->ringPorts[3] = 4003;
    ring->ringPorts[4] = 4004;
    ring->ringPorts[5] = 4005;
    ring->ringPorts[6] = 4006;
    ring->ringPorts[7] = 4007;
    ring->ringPorts[8] = 4008;
    ring->ringPorts[9] = 4009; 

    ring->isPrimary = false;
    ring->currentIndex = -1; // We start incrementing it
    ring->isInElection = false;
    ring->ringAddress = "127.0.0.1";
    pthread_mutex_init(&ring->electionMutex, NULL);

     // Creating and configuring sockfd for this node to receive messages
    if ((ring->currentSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "Problem opening initial server ring socket" << endl;
        exit(0);
    }

    int socketReuseOption = 1;
    setsockopt(ring->currentSocket, SOL_SOCKET, SO_REUSEADDR, &socketReuseOption, sizeof(int));
    
    // Creating and configuring sockfd for the node in the ring
    if ((ring->nextSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Error opening ring socket" << endl; 
        exit(0);
    }

    setsockopt(ring->nextSocket, SOL_SOCKET, SO_REUSEADDR, &socketReuseOption, sizeof(int));

    return ring;
}

void ServerRing::connectServerRing(ServerRing *ring) {
     // Configuring this server connection
    bindServerRing(ring);
    listenServerRing(ring);

     // Configuring this server connection with his next node in the list
    connectNextServerToRing(ring);

     // Start a thread for the keep alive, to know when primary is down
     if (ring->isPrimary == false) {
         pthread_create(&ring->heartbeatThread, NULL, (void *(*)(void *)) & heartbeat, (void *)ring);
     }
}

void ServerRing::bindServerRing(ServerRing *ring) {
    struct sockaddr_in serv_addr;

    int portCount = 0;
    ring->currentIndex = portCount;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(ring->ringPorts[portCount]);
    bzero(&(serv_addr.sin_zero), 8);

    
    while (bind(ring->currentSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        portCount += 1;
        ring->currentIndex = portCount;
        struct hostent *in_addr = gethostbyname(ring->ringAddress);
        serv_addr.sin_addr = *((struct in_addr *)in_addr->h_addr);
        serv_addr.sin_port = htons(ring->ringPorts[portCount]);
    }
}

void ServerRing::listenServerRing(ServerRing *ring) {
     if (listen(ring->currentSocket, RING_SIZE) < 0) {
         cout << "Error on ring to start to listen" << endl;

         close(ring->currentSocket);
         exit(0);
     }

     cout << "Going to listen to port " << ring->ringPorts[ring->currentIndex] << endl;
}

void ServerRing::connectNextServerToRing(ServerRing *ring) {
    CommunicationManager commManager;
    struct sockaddr_in next_addr;

    next_addr.sin_family = AF_INET;
    bzero(&(next_addr.sin_zero), 8);

    ring->nextIndex = ring->currentIndex;
    ring->nextIndex = getNextServerRingIndex(ring, ring->nextIndex);
    next_addr.sin_port = htons(ring->ringPorts[ring->nextIndex]);
    struct hostent *in_addr = gethostbyname(ring->ringAddress);
    next_addr.sin_addr = *((struct in_addr *)in_addr->h_addr);

    while (ring->nextIndex != ring->currentIndex && (connect(ring->nextSocket, (struct sockaddr *)&next_addr, sizeof(next_addr)) < 0)) {
        ring->nextIndex = getNextServerRingIndex(ring, ring->nextIndex);
        next_addr.sin_port = htons(ring->ringPorts[ring->nextIndex]);
        struct hostent *in_addr = gethostbyname(ring->ringAddress);
        next_addr.sin_addr = *((struct in_addr *)in_addr->h_addr);
    }

    // Went all the list around and couldn't connect to anyone, so I'm the primary
    if (ring->nextIndex == ring->currentIndex) {
        cout << "Ring didn't find any other servers connected, so here is the new leader!" << endl;
        ring->isPrimary = true;
        ring->primaryPort = ring->ringPorts[ring->nextIndex];
        return;
    }

    // Need to ask who is the primary
    cout << "Connected with follower in port " << ring->ringPorts[ring->nextIndex]  << " The next step is to find the primary port" << endl;


    // NOTIFICATION notification = {.type = NOTIFICATION_TYPE__LEADER_QUESTION};
    
    commManager.sendPacket(ring->nextSocket, new Packet("Looking for primary port", LeaderDiscovery));

    Packet *receivedPacket = commManager.receivePacket(ring->nextSocket);

    if (receivedPacket->type == LeaderFound) {
        ring->primaryPort = stoi(receivedPacket->message);
        cout << "Found the primary port: " << ring->primaryPort << endl;;
    } else {
        cout << "Error finding the leader on ring." << endl;
        exit(0);
    }
 }

int ServerRing::getNextServerRingIndex(ServerRing *ring, int currentIndex) {
    if (currentIndex - 1 >= 0) {
        return currentIndex - 1;
    }
 
    // Need to go through the port list to find what is the last index
    int lastIndex = RING_SIZE - 1;
    for (;ring->ringPorts[lastIndex] == 0; lastIndex--);

    return lastIndex;
}

void heartbeat(void *ringData) {
    ServerRing *ring = (ServerRing *)ringData;
    int socketReuseOption = 1;
    pthread_t electionThread;

    // Creating and configuring sockfd for the keepalive
    if ((ring->heartbeatSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "Error opening heartbeat socket" << endl;
        exit(0);
    }

    setsockopt(ring->heartbeatSocket, SOL_SOCKET, SO_REUSEADDR, &socketReuseOption, sizeof(int));

    // 3 seconds timeout
    struct timeval timeInterval;
    int heartbeatSeconds = 5;
    timeInterval.tv_sec = 2;
    timeInterval.tv_usec = 0;
    setsockopt(ring->heartbeatSocket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeInterval, sizeof(timeInterval));

    // Connecting per se
    struct sockaddr_in heartbeatAddress;

    heartbeatAddress.sin_family = AF_INET;
    bzero(&(heartbeatAddress.sin_zero), 8);

    heartbeatAddress.sin_port = htons(ring->primaryPort);
    struct hostent *in_addr = gethostbyname("127.0.0.1");
    heartbeatAddress.sin_addr = *((struct in_addr *)in_addr->h_addr);

    if (connect(ring->heartbeatSocket, (struct sockaddr *)&heartbeatAddress, sizeof(heartbeatAddress)) < 0) {
        cout << "Error connecintg heartbeat socket with id: " << ring->heartbeatSocket << endl;
        exit(0);
    }

    while (true) {
        // Sleep for some time, before checking the master again
        sleep(heartbeatSeconds);
        cout << "Sending heartbeat to " << ring->primaryPort << endl;

        int successCodeWrite = write(ring->heartbeatSocket, new Packet("HEARTBEAT", Heartbeat), sizeof(Packet));

        if (successCodeWrite < 0) {
            // Leader is not active
            if (errno == EPIPE) {
                cout << "Leader is disconnected! Failed to send heartbeat" << endl;
                startElection(ring);
                return;
            }
            cout << "Error sending heartbeat: " << errno << endl;
        }

        Packet *receivedPacket = new Packet; 
        int successCodeRead = read(ring->heartbeatSocket, receivedPacket, sizeof(Packet));

        if (successCodeRead < 0) {
            // Connection to master timed out, need to start an election
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                cout << "Failed to receive heartbeat from client, maybe it died." << endl;
                startElection(ring);
                return;
            }
            cout << "Error receiving heartbeat" << endl;
            return;
        }
    }
}

void startElection(ServerRing* ring) {
    cout << "ENTERED ELECTION METHOD" << endl;

    int sockfd;
    int socketReuseOption = 1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "Error opening election socket" << endl;
        exit(0);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &socketReuseOption, sizeof(int));

    pthread_mutex_lock(&ring->electionMutex);

    if (!ring->isInElection) {
        ring->isInElection = true;
        pthread_mutex_unlock(&ring->electionMutex);

        struct sockaddr_in next_addr;
        next_addr.sin_family = AF_INET;
        bzero(&(next_addr.sin_zero), 8);

        int connectionStatus;

        ring->nextIndex = ring->currentIndex;
        do {
            ring->nextIndex = ring->getNextServerRingIndex(ring, ring->nextIndex);
            next_addr.sin_port = htons(ring->ringPorts[ring->nextIndex]);
            struct hostent *in_addr = gethostbyname("127.0.0.1");
            next_addr.sin_addr = *((struct in_addr *)in_addr->h_addr);
            cout << "Let's try to connect to " << ring->ringPorts[ring->nextIndex] << endl;

            connectionStatus = connect(sockfd, (struct sockaddr *)&next_addr, sizeof(next_addr));
            cout << "Result of the connection attempt: " << endl;
        } while (ring->nextIndex != ring->currentIndex && connectionStatus < 0);

        // Couldn't connect to anyone, so this is the primary server
        if (ring->nextIndex == ring->currentIndex) {
            cout << "Didn't find any other servers, so I'm the new leader!" << endl;
            ring->isPrimary = true;

            // Uses mutex because we don't know if it counts as a single memory access or two because of the pointer
            pthread_mutex_lock(&ring->electionMutex);
            ring->isInElection = false;
            pthread_mutex_unlock(&ring->electionMutex);

            ring->primaryPort = ring->ringPorts[ring->nextIndex];
            return;
        }

        int writeResult = write(sockfd, new Packet(to_string(ring->nextIndex), Election), sizeof(Packet));
        if (writeResult < 0) {
            cout << "Error sending election message" << endl;
            exit(0);
        }
        close(sockfd);

    } else {
        // If it is already in an election, can just unlock it again
        pthread_mutex_unlock(&ring->electionMutex);
    }
}

