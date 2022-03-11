#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <sstream>
using namespace std;
using std::string;
/*
Arquivo csv, primeiro nome é o usuário, resto seus seguidores.


*/
void CreateFile(string file_name)
{

    ofstream MyFile(file_name);
}

map<string,list<string>> ReturnFollowers(string file_name)
{
    ifstream MyReadFile(file_name);
    
    string line,word,key;

    map<string,list<string>> followers_map;
    int i;
    while(getline(MyReadFile,line))
    {   
        i = 0;
        stringstream str(line);
        
        while (getline(str,word,','))
        {
            if (i == 0)
            {   list<string> followers;
                followers_map.insert({word,followers});
                key = word;
                i += 1;
            }

            else
            {
                followers_map[key].push_back(word);
            }

        }
    }
    return followers_map;

}


void WriteToFile(string file_name, string followed, string follower)
{
    ifstream MyReadFile(file_name);
    ofstream temp("temp.txt"); 
    string line,word,key;

    int i;
    bool is_this_line,already_in_file;

    if(followed == follower)
    {
        cout << "Não pode seguir a si mesmo! " << endl;
        return;
    }
    already_in_file = false;
    while(getline(MyReadFile,line))
    {   
        i = 0;
        is_this_line = false;
        stringstream str(line);
        
        while (getline(str,word,','))
        {
            
            if(i == 0)
            {
                if(word == followed)
                {
                    is_this_line = true;
                    already_in_file = true;
                    
                }

                temp << word;
            
            }

           else
           {
               temp << ", " << word;
           }
            i++;

        }
        if(is_this_line)
        {
            temp << ", " << follower;
        }
        temp << endl;
    }

    if(!already_in_file)
    {
        temp << followed << ", " << follower << endl;
    }

    remove("users.txt");
    rename("temp.txt","users.txt");

}



void WriteFollowers(string file_name, string followed, string follower)
{
    WriteToFile(file_name,followed,follower);

}