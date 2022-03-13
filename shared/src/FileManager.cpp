#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <vector>
#include <sstream>
#include "../headers/FileManager.hpp"

using namespace std;
using std::string;
/*
Arquivo csv, primeiro nome é o usuário, resto seus seguidores.


*/

// CreateFile <- so instancia o arquivo e cria se n tiver
// ReturnUsers
map<string,list <string> > FileManager::CreateFile(string file_name)
{
    ifstream ifile;
    ifile.open("users.txt");
    map<string, list<string> > followersMap;
    if (!ifile)
    {
        ofstream MyFile(file_name);
        return followersMap;
    }
}

map<string, list<string> > FileManager::ReturnUsers(string file_name)
{
    ifstream MyReadFile(file_name);
    string line, word, key;
    map<string, list<string> > followersMap;

    int i;
    while (getline(MyReadFile, line))
    {
        i = 0;
        stringstream str(line);

        while (getline(str, word, ','))
        {
            word.erase( std::remove(word.begin(), word.end(), '\r'), word.end() ); // /r
            if (i == 0)
            {
                list<string> followers;
                followersMap.insert(std::make_pair(word, followers));
                key = word;
                i += 1;
            }

            else
            {
                followersMap[key].push_back(word);
            }
        }
    }
    return followersMap;
}

void FileManager::WriteToFile(string file_name, string followed, string follower)
{
    ifstream MyReadFile(file_name);
    ofstream temp("temp.txt");
    string line, word, key;

    int i;
    bool is_this_line, already_in_file;

    if (followed == follower)
    {
        cout << "Não pode seguir a si mesmo! " << endl;
        return;
    }
    already_in_file = false;
    while (getline(MyReadFile, line))
    {
        i = 0;
        is_this_line = false;
        stringstream str(line);

        while (getline(str, word, ','))
        {
            word.erase( std::remove(word.begin(), word.end(), '\r'), word.end() );
            if (i == 0)
            {
                if (word == followed)
                {
                    is_this_line = true;
                    already_in_file = true;
                }

                temp << word;
            }

            else
            {
                temp << "," << word;
            }
            i++;
        }
        if (is_this_line)
        {
            temp << "," << follower;
        }
        temp << endl;
    }

    if (!already_in_file)
    {
        if (follower == "")
        {
            temp << followed << endl;
        }
        else
        {
            temp << followed << "," << follower << endl;
        }
    }

    remove("users.txt");
    rename("temp.txt", "users.txt");
}

void FileManager::writeFollower(string file_name, string followed, string follower)
{
    WriteToFile(file_name, followed, follower);
}

void FileManager::writeUser(string file_name, string followed)
{
    WriteToFile(file_name, followed, "");
}