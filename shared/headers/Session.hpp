#ifndef _SESSION_
#define _SESSION_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>

class Session {
    public:
        char* sessionId;
        char* connectedUserId;

        Session(std::string sessionId_, std::string connectedUserId_) {
            strncpy(sessionId, sessionId_.c_str(), sessionId_.length());
            strncpy(connectedUserId, connectedUserId_.c_str(), connectedUserId_.length());
        }
};

#endif