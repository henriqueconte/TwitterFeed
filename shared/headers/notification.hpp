#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>

using namespace std;

class Notification {
    public:
        string id;
        string message;
        string senderId;
        long int timestamp;

        Notification::Notification
        (
            string id,
            string message,
            string senderId,
            long int timestamp
        ) {
            this->id = id;
            this->message = message;
            this->senderId = senderId;
            this->timestamp = timestamp;
        }

        Notification(){}
};