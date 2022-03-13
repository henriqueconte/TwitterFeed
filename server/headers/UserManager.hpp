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
public:
    std::map<string, User*> userMap;
    
    list<string> getFollowers(string username);
    list<Notification> getPendingNotifications(string username);
    list<Notification> getNotifications(string username);
    void loadUsers();
    void addNewNotifications(string username, Notification notification);
};
