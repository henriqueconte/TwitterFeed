#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../shared/headers/packet.hpp"
#include "../shared/headers/CommunicationManager.hpp"
#include "headers/SessionManager.hpp"
#include "headers/UserManager.hpp"
#include "../shared/headers/FileManager.hpp"
#include "headers/server_ring.hpp"

#define PORT 4000
#include <iostream>

void *serveClient(void *data);
void *authenticateClient(void *data);
void serverHeartbeat(int socket);
// void handle_signals(void);

SessionManager sessionManager;
CommunicationManager commManager;
UserManager userManager;
ServerRing *serverRing;

std::string fileName = "users.txt";
int portNumber = PORT;
bool isServerConnection = false;
bool receivedSigint = false;
int *serverSocketCopy;

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // handle_signals();
    userManager.loadUsers();

    clilen = sizeof(struct sockaddr_in);
    
    serverRing = serverRing->initServerRing();
    serverRing->connectServerRing(serverRing);

    serverSocketCopy = &sockfd;

    while (true) {
        // newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        newsockfd = accept(serverRing->currentSocket, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd == -1)
        {
            printf("ERROR on accept");
            continue;
        }

        // Adding two sessions with username jose to test scenarios with two active sessions. Remove it later.
        // sessionManager.tryLogin("jose");
        // sessionManager.tryLogin("jose");

        pthread_t authThread;
        std::cout << "Created authentication thread." << std::endl;
        int *socketCopy = (int *)malloc(sizeof(int));
        *socketCopy = newsockfd;
        pthread_create(&authThread, NULL, &authenticateClient, (void *)socketCopy);

        void *authenticationResponse;
        pthread_join(authThread, &authenticationResponse);
        std::cout << "Finished authentication thread." << std::endl
                  << "\n";
        Session *session = (Session *)authenticationResponse;
        session->socket = socketCopy;

        // If the session is open, the authentication was a success.
        if (session->sessionStatus == Open)
        {
            userManager.addUser(session->connectedUserId);
            pthread_t serviceThread;
            std::cout << "Created serving thread." << std::endl;
            pthread_create(&serviceThread, NULL, &serveClient, (void *)session);
        }
        // TALVEZ TENHA QUE INICIAR LEITURA DE NOTIFICAÇÕES AQUI!
    }

    close(sockfd);
    return 0;
}

void *authenticateClient(void *data) {
    int *socketCopy = (int *)data;
    int clientSocket = *socketCopy;

    Packet *receivedPacket = new Packet;
    std::string *finalResult;
    Packet *responsePacket = new Packet;
    Session *session = new Session;

    int readResult = read(clientSocket, receivedPacket, sizeof(Packet));
    if (readResult < 0) {
        std::cout << "Failed to read authentication socket." << std::endl;
        *responsePacket = Packet("Failed to read authentication socket.", Login);
    }

    if (receivedPacket->type == LeaderDiscovery) {
        std::cout << "Received Leader Discovery request. Message: " << receivedPacket->message << std::endl;
        isServerConnection = true;
        *responsePacket = Packet(to_string(serverRing->primaryPort), LeaderFound);
        commManager.sendPacket(clientSocket, responsePacket);
        return socketCopy; // Maybe change to serverRing->primaryPort

    } else if (receivedPacket->type == Heartbeat) {
        return socketCopy;
    } else {
        std::cout << "Received data to authenticate. User login: " << receivedPacket->message << std::endl;
        isServerConnection = false;
        session = sessionManager.tryLogin(receivedPacket->message);
        if (session->sessionStatus == Failed) {
            *responsePacket = Packet("Login failed. Try closing one of the open sessions.", Login);
        }
        else {
            *responsePacket = Packet(session->sessionId, Login);
        }

        std::cout << "Sending login response packet to client. Response: " << responsePacket->message << std::endl;
        commManager.sendPacket(clientSocket, new Packet(responsePacket->message, Login));
        commManager.receivePacket(clientSocket);

        return session;
    }
}

void *serveClient(void *data) {
    int clientSocket;
    Session *session;

    if (isServerConnection) {
        clientSocket = *(int *)data;
    } else {
        session = (Session *)data;
        clientSocket = *session->socket;
    }


    bool shouldExit = false;

    while (!shouldExit)
    {
        //  Receives message packet from client to server
        Packet *messagePacket = commManager.receivePacket(clientSocket);

        // Handle command type accordingly
        switch (messagePacket->type) {
        case Login:  // TODO: Handle login here
            commManager.sendPacket(clientSocket, new Packet("Your login message was received.", Login));

            // Receives acknowledge packet from client to server
            messagePacket = commManager.receivePacket(clientSocket);
            break;
        case Message:

            // Sends acknowledge packet from server to client
            commManager.sendPacket(clientSocket, new Packet("Server acknowledges to have received a packet.", Login));

            // Sends message packet from server to client
            // commManager.sendPacket(clientSocket, new Packet("Your message content was received.", Message));
            commManager.sendNotification(session->connectedUserId, messagePacket, sessionManager.activeSessionsList, userManager.userMap);

            // Receives acknowledge packet from client to server
            messagePacket = commManager.receivePacket(clientSocket);
            break;

        case Follow: {
            commManager.sendPacket(clientSocket, new Packet("Server acknowledges to have received a Follow packet.", Login));

            std::string usersString = messagePacket->message;

            std::string delimiter = "|";
            std::string follower = usersString.substr(0, usersString.find(delimiter)); 
            std::string followed = usersString.substr(follower.length() + 1, usersString.length()); 

            userManager.addFollower(followed, follower);

            break;
        }

        case Logout:
            std::cout << "User logging out: " << messagePacket->message << std::endl;
            sessionManager.closeSession(messagePacket->message);
            shouldExit = true;
            break;

        case LeaderDiscovery:
            // Sends acknowledge packet from server to server
            commManager.sendPacket(clientSocket, new Packet(to_string(clientSocket), LeaderFound));

            break;

        case LeaderFound:
            std::cout << "Received a LeaderFound packet" << std::endl;

            break;
        
        case Heartbeat:

            if (serverRing->isPrimary == false) {
                cout << "Shouldn't receive hearbeat from not primary" << endl;
            }
            std::cout << "Received a heartbeat" << std::endl;
            serverHeartbeat(clientSocket);
            break;
        }

        delete messagePacket;
    }

    close(clientSocket);

    return 0;
}

void serverHeartbeat(int socket) {
    while (true) {
        sleep(1);
        int sendStatusCode = write(socket, new Packet("HEARTBEAT RECEIVED", Heartbeat), sizeof(Packet));
        if (sendStatusCode < 0) {
            if (errno == EPIPE)
            {
            //  logger_info("[Socket %d] When sending keepalive to client. Must be dead. Stopping answering keep alives\n", sockfd);
            cout << "Error to send heartbeat to client, maybe it's dead. The socket is: " << socket << endl;
            return;
            }

            //  logger_error("[Socket %d] When sending keepalive to client. Not an EPIPE. Will retry to send...\n", sockfd);
            cout << "Error sending heartbeat to client, it will try again. Socket: " << socket << endl;
            continue;
        }

        Packet *receivedPacket = new Packet; 
        int readStatusCode = read(socket, receivedPacket, sizeof(Packet));
        if (readStatusCode < 0) {
            cout << "Error receiving heartbeat from client, maybe it's dead. The socket is: " << socket << endl;
            return;
        }
    }
}

// void sigint_handler(int _sigint) {
//     if (receivedSigint == false) {
//         receivedSigint = true;
//         cout << "Received SIGINT. Time to close the server." << endl;
//         // logger_warn("SIGINT received, closing descriptors and finishing...\n");
//         // cleanup(0);
//         // close(*serverSocketCopy);
//     }
// }

// void handle_signals(void) {
//     struct sigaction sigintAction;
//     sigintAction.sa_handler = sigint_handler;
//     sigaction(SIGINT, &sigintAction, NULL);
//     sigaction(SIGINT, &sigintAction, NULL); // Activating it twice works, so don't remove this ¯\_(ツ)_/¯

//     struct sigaction sigint_ignore;
//     sigintAction.sa_handler = SIG_IGN;
//     sigaction(SIGPIPE, &sigintAction, NULL);
//     sigaction(SIGPIPE, &sigintAction, NULL);

//     // Create a different thread to handle CTRL + D
//     // pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));
//     // pthread_create(thread, NULL, (void *(*)(void *)) & handle_eof, (void *)NULL);
//     // logger_debug("Created new thread to handle EOF detection\n");

//     // chained_list_append_end(chained_list_threads, (void *)thread);
// }