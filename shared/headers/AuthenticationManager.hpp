#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include "Session.hpp"

class AuthenticationManager {
    private: 
        static AuthenticationManager* shared;

        AuthenticationManager();

    public:
        std::list<Session *> activeSessionsList; // Array with all active sessions, will be used to verify if user has less than two active sessions, and also to disconnect when user logouts
        
        static AuthenticationManager* getInstance();
        static bool login(std::string username);
        
};