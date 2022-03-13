#include "../headers/UserManager.hpp"
#include "../../shared/headers/User.hpp"
#include "../../shared/headers/FileManager.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <map>
#include <algorithm>
#include "../../shared/headers/notification.hpp"
#include <algorithm>

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
    FileManager fileManager;
    map<string, list<string>> followersMap = fileManager.ReturnUsers("users.txt");

    if (!followersMap.empty()) // Why 
    {   
        for (auto const &pair : followersMap)
        {
            auto key = pair.first;
            auto followers = pair.second;
            User* newUser = new User(key, followers);
            userMap.insert(std::make_pair(key, newUser));
        }
    }
}

bool UserManager::IsFollowing(string followed, string follower)
{
    list<string> followers = userMap[followed]->followers;

    list<string>::iterator it;

    it = std::find(followers.begin(), followers.end(), follower);

    if (it != followers.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}
