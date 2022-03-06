#include <string>
#include <list>
#include <pthread.h>
#include <Session.hpp>
#include <map>

using namespace std;
using std::string;
class User
{
private:

    list<std::string> followers;

    map<string,Session> sessions;

    /* data */
public:


    string username;
    pthread_t thread_id;
    list<string> pend_notifications;

    User(/* args */);
    User(string username);
    User(string username,list<string>pend_notifications, list<string> followers);

    ~User();

    list<string> getFollowers();
    list<string> getPend_Notifications();

    //ou void sla
    bool addNewNotifications(int notificationId);
    void getNotifications();
    
    bool hasSessionwithId(string SessionId);
    Session getSessionWithId(string SessionId);
    //bool UpdateSession(string SessionId);
    void addNewSession(string SessionId, Session session);
    void RemoveSession(string SessionId);
    map<string,Session> getSessions();




};

