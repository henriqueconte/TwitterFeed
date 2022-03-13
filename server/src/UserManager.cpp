#include "../headers/UserManager.hpp"
#include "../../shared/headers/User.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <map>
#include "../../shared/headers/notification.hpp"
#include <algorithm>
#include "../../shared/headers/FileManager.hpp"

list<string> UserManager::getFollowers(string username)
{
    return userMap[username]->followers;
}

list<Notification> UserManager::getPendingNotifications(string username)
{
    return userMap[username]->pendingNotifications;
}

void UserManager::addNewNotifications(string username, Notification notification)
{
    userMap[username]->pendingNotifications.push_front(notification);
}

list<Notification> UserManager::getNotifications(string username)
{

    return userMap[username]->pendingNotifications;
}

void UserManager::loadUsers()
{
    map<string, list<string>> userMap = FileManager::ReturnUsers("users.txt");
    if (!userMap.empty()) // Why 
    {
        for (auto const &pair : userMap)
        {
            auto key = pair.first;
            auto followers = pair.second;
            userMap[key]->followers = followers;
        }
    }
}

bool UserManager::IsFollowing(string followed, string follower)
{

    list<string> followers = userMap[followed]->followers;

    list<string>::iterator it;

    it = find(followers.begin(), followers.end(), follower);

    if (it != followers.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}
