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

    std::map<string,User*> UserMap;
    
    list<string> getFollowers(string username);
    list<notification> getPend_Notifications(string username);

    //ou void sla
    void addNewNotifications(string username, notification notification);
    list<notification> getNotifications(string username);

    /* data */
public:
    UserManager(/* args */);
    ~UserManager();
};
