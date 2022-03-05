#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "../headers/SessionManager.hpp"
#include "../../shared/headers/Session.hpp"
#include <list> 

using namespace std;

std::string SessionManager::tryLogin(std::string username) {
    // Verifies how many sessions are active with the same username
    int activeUserSessionCount = 0;
    // TODO Persistência de dados: buscar dados salvo na base de dados para verificar as sessões ativas
    // TODO (Pedro): Use map instead of a list to handle active sessions
    for (auto const& element: activeSessionsList) {
        if (element->connectedUserId == username) {
            activeUserSessionCount++;
        }
    }

    if (activeUserSessionCount < 2) {
        Session* session = new Session(generateSessionId(), username);
        cout << "Sessão criada com sucesso para o usuário: " << session->connectedUserId << ". Id da sessão: " << session->sessionId << " \n";
        // TODO Persistência de dados: salvar sessão criada em uma base de dados 
        activeSessionsList.push_back(session);
        cout << "Autenticação do usuário " << username << " realizada com sucesso." << endl;
        return session->sessionId;
    } else {
        cout << "User has already too many sessions! Please close one of them before logging in in another machine." << endl;
        return "Authentication failed";
    }
}

void SessionManager::closeSession(std::string sessionId) {
    int initialNumberOfSessions = activeSessionsList.size();

    for (auto i = activeSessionsList.begin(); i != activeSessionsList.end(); i++) {
        if ((*i)->sessionId == sessionId) {
            i = activeSessionsList.erase(i);
            cout << "Session with id " << sessionId << " closed with success." << endl;
            break;
        }
    }
    
    if (activeSessionsList.size() == initialNumberOfSessions) {
        cout << "Couldn't close session with id " << sessionId << endl;
    }
}

string SessionManager::generateSessionId() {
    int idLength = 8;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(idLength);

    for (int i = 0; i < idLength; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)]; // TODO: Prevent the repetition of session ids?? (Low priority)
    }
    
    return tmp_s;
}
