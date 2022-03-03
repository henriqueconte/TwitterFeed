#ifndef _SESSION_
#define _SESSION_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>

class Session {
    public:
        std::string sessionId;
        std::string connectedUserId;

        Session(std::string sessionId_, std::string connectedUserId_) {
            sessionId = sessionId_;
            connectedUserId = connectedUserId_;
        }
};

#endif