#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Session.hpp"

class AuthenticationManager {
    public:
        static Session* activeSessionsList; // Array with all active sessions, will be used to verify if user has less than two active sessions, and also to disconnect when user logouts
        
        static bool login(std::string username);
};