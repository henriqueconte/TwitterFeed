#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include <map>
#include "../../shared/headers/User.hpp"
#include "../../shared/headers/notification.hpp"

using namespace std;
class UserManager
{
private:

    std::map<string, User*> userMap;
    
    list<string> getFollowers(string username);
    list<Notification> getPendingNotifications(string username);
    map<string, User*> loadUsers();
    //ou void sla
    void addNewNotifications(string username, Notification notification);
    list<Notification> getNotifications(string username);

    /* data */
public:
    UserManager(/* args */);
    ~UserManager();
};
