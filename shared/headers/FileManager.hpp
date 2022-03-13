#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <sstream>
using namespace std;
using std::string;


class FileManager {
    public:
        // FileManager();
        map<string,list <string> >  CreateFile(string file_name);

        map<string,list <string> > ReturnUsers(string file_name);
        void WriteToFile(string file_name, string followed, string follower);
        void WriteFollowers(string file_name, string followed, string follower);
};

