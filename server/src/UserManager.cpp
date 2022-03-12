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
    return followersMap[username]->followers;
}

list<Notification> UserManager::getPendingNotifications(string username)
{

    return followersMap[username]->pendingNotifications;
}

void UserManager::addNewNotifications(string username, Notification notification)
{
    followersMap[username]->pendingNotifications.push_front(notification);
}

list<Notification> UserManager::getNotifications(string username)
{

    return followersMap[username]->pendingNotifications;
}

map<string, User*> UserManager::loadUsers()
{
    // 
    map<string, list<string>> followersMap = FileManager::readUsersFromFile("users.txt");
    map<string, User*> userMap;
    if(!followersMap.empty())
    {

        for(auto const& pair : followersMap)
        {
            auto key = pair.first;

            auto followers = pair.second;
            
            userMap[key]->followers=followers;

        }
    return userMap;
}

bool UserManager::IsFollowing(string followed, string follower)
{

    list<string> followers = followersMap[followed]->followers;

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
