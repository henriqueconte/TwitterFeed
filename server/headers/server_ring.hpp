#include <pthread.h>

#define RING_SIZE 10

using namespace std;

class ServerRing {
    public: 
        int ringPorts[RING_SIZE];  
        int currentIndex, currentSocket, nextIndex, nextSocket, heartbeatSocket, primaryPort;
        char *ringAddress;
        bool isPrimary, isInElection;
        pthread_t heartbeatThread;
        pthread_mutex_t electionMutex;

        ServerRing *initServerRing();
        void connectServerRing(ServerRing * ring);
        void connectNextServerToRing(ServerRing *ring);
        void bindServerRing(ServerRing *ring);
        void listenServerRing(ServerRing *ring);
        int getNextServerRingIndex(ServerRing *ring, int currentIndex);
};