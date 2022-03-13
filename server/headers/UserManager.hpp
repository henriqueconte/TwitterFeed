#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <map>
#include "../../shared/headers/User.hpp"
#include "../../shared/headers/notification.hpp"
#include <mutex>

using namespace std;
class UserManager
{
    public:
        std::map<string, User*> userMap;
        void addUser(string username);
        void addFollower(string followed, string follower);
        list<string> getFollowers(string username);
        list<Notification> getPendingNotifications(string username);
        list<Notification> getNotifications(string username);
        void loadUsers();
        void addNewNotifications(string username, Notification notification);
        bool IsFollowing(string followed, string follower);
        mutex usersMutex;
};
