#include "../headers/UserManager.hpp"
#include "../../shared/headers/User.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list> 
#include <map>
#include "../../shared/headers/notification.hpp"


list<string> UserManager::getFollowers(string username)
{
    return UserMap[username]->followers;

}

list<notification> UserManager::getPend_Notifications(string username)
{

    return UserMap[username]->pend_notifications;
}

void UserManager::addNewNotifications(string username, notification notification)
{
    UserMap[username]->pend_notifications.push_front(notification);
    
}

list<notification> UserManager::getNotifications(string username)
{

    return UserMap[username]->pend_notifications;
} 


