#include <string>
#include <list>
#include <pthread.h>
#include <Session.hpp>
#include <map>
#include <notification.hpp>

using namespace std;
using std::string;
class User
{
private:

    /* data */
public:

    list<std::string> followers;

    map<string,Session> sessions;

    string username;
    pthread_t thread_id;
    //Aqui pode ser uma lista do tipo Notification tambem
    list<Notification> pend_notifications;

    User(/* args */);
    User(string username);
    User(string username,list<string>pend_notifications, list<string> followers);

    ~User();
};

