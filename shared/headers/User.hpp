#ifndef USER_H
#define USER_H

#include <string>
#include <list>
#include <thread>
#include <map>
#include "../headers/Session.hpp"
#include "../headers/notification.hpp"

using namespace std;
using std::string;
class User
{
private:
    /* data */
public:
    list<string> followers;

    map<string, Session*> sessions;

    string username;
    pthread_t threadId;
    // Aqui pode ser uma lista do tipo Notification tambem
    list<Notification> pendingNotifications;

    User(/* args */);
    User(string username);
    User(string username, list<Notification> pendingNotifications, list<string> followers);

    ~User();
};

#endif
