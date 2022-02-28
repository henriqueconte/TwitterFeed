#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include "../../shared/headers/Session.hpp"

class SessionManager {
    public:
        std::list<Session *> activeSessionsList;
        bool tryLogin(std::string username);
        void closeSession(std::string sessionId);
    
    private:
        std::string generateSessionId();
};