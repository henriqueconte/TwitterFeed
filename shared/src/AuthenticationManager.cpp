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
    AuthenticationManager* authManager = AuthenticationManager::getInstance();

    // Verifies how many sessions are active with the same username
    int activeUserSessionCount = 0;
    // TODO Persistência de dados: buscar dados salvo na base de dados para verificar as sessões ativas
    for (auto const& element: authManager->activeSessionsList) {
        if (element->connectedUserId == username) {
            activeUserSessionCount++;
        }
    }

    if (activeUserSessionCount < 2) {
        Session* session = new Session(authManager->generateSessionId(), username);
        cout << "Sessão criada com sucesso pelo usuário: " << session->connectedUserId << " Id da sessão: " << session->sessionId << " \n";
        // TODO Persistência de dados: salvar sessão criada em uma base de dados 
        authManager->activeSessionsList.push_back(session);
        cout << "Autenticação do usuário " << username << " realizada com sucesso\n";
        return true;
    } else {
        cout << "User has already too many sessions! Please close one of them before logging in in another machine." << endl;
        return false;
    }
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

string AuthenticationManager::generateSessionId() {
    int idLength = 8;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(idLength);

    for (int i = 0; i < idLength; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}
