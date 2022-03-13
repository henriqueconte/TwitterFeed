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

FileManager fileManager;

void UserManager::addUser(string username)
{
    std::lock_guard<std::mutex> sessions_lock(usersMutex);
    
    if ( userMap.find(username) == userMap.end() ) { // User not yet in map
        User* newUser = new User(username);
        userMap.insert(std::make_pair(username, newUser));
        fileManager.writeUser("users.txt", username);
    }
}

void UserManager::addFollower(string followed, string follower)
{
    std::lock_guard<std::mutex> sessions_lock(usersMutex);

    // Test if not following yet, test if followed user exists
    userMap[followed]->followers.push_back(follower);
    fileManager.writeFollower("users.txt", followed, follower);
}

list<string> UserManager::getFollowers(string username)
{
    std::lock_guard<std::mutex> sessions_lock(usersMutex);
    return userMap[username]->followers;
}

list<Notification> UserManager::getPendingNotifications(string username)
{
    std::lock_guard<std::mutex> sessions_lock(usersMutex);
    return userMap[username]->pendingNotifications;
}

void UserManager::addNewNotifications(string username, Notification notification)
{
    std::lock_guard<std::mutex> sessions_lock(usersMutex);
    userMap[username]->pendingNotifications.push_front(notification);
}

list<Notification> UserManager::getNotifications(string username)
{
    std::lock_guard<std::mutex> sessions_lock(usersMutex);
    return userMap[username]->pendingNotifications;
}

void UserManager::loadUsers()
{
    map<string, list<string> > followersMap = fileManager.ReturnUsers("users.txt");

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
    std::lock_guard<std::mutex> sessions_lock(usersMutex);

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
