#ifndef _SESSION_
#define _SESSION_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cstring>

enum SessionStatus { Open, Closed, Failed };

class Session {
    public:
        std::string sessionId;
        std::string connectedUserId;
        SessionStatus sessionStatus;

        Session(std::string sessionId_, std::string connectedUserId_, SessionStatus sessionStatus_) {
            sessionId = sessionId_;
            connectedUserId = connectedUserId_;
            sessionStatus = sessionStatus_;
        }
        Session(){}
};

#endif