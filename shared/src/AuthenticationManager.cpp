#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "../headers/AuthenticationManager.hpp"
#include "../headers/Session.hpp"
#include <list> 

using namespace std;

// ******
// This class is using a singleton pattern, in order to keep track of the authentication and sessions along the program execution
// ******

AuthenticationManager* AuthenticationManager::shared = 0; // Singleton that will be used by client and server

// Verifies if user is valid to login, and returns if the login succeeded
bool AuthenticationManager::login(string username) {
    // TODO: verify if there is already two sessions from the same user
    AuthenticationManager* authManager = AuthenticationManager::getInstance();

    // TODO: Generate ID for the session
    Session* session = new Session("abc123", username);
    cout << "Sessão criada com sucesso pelo usuário: " << session->connectedUserId << " \n";
    authManager->activeSessionsList.push_back(session); 
    cout << "Autenticação do usuário " << username << " realizada com sucesso\n";
    return true;
}

// I don't know what this does, I only know it's necessary for the sigleton pattern (https://gist.github.com/pazdera/1098119)
AuthenticationManager::AuthenticationManager() {}

// Retrieves the shared version of the singleton; therefore, we can't have instances with different values on the application 
AuthenticationManager* AuthenticationManager::getInstance() {
    if (shared == 0) {
        shared = new AuthenticationManager();
    }

    return shared;
}