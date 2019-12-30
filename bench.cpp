/*
* reactor框架做的bench测试
*/
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <functional>
#include <unordered_map>
#include <sys/epoll.h>
#include <sys/time.h>
#include <vector>

using namespace std;

class Pipe;
class Event;
class EpollController;
class EventController;


bool createPipe(Pipe& pipe);
Pipe* createPipes(int num);
static struct timeval* runOnce(EventController& eventController, Pipe* pipes, int nPipes, int nActive);
void callback(int fd, void* arg);

class Pipe
{
public:
    Pipe(): connector(-1), acceptor(-1) {}
    virtual ~Pipe(){}

    int getConnector(){return connector;}
    int getAcceptor(){return acceptor;}

    void setConnector(int connector){this->connector = connector;}
    void setAcceptor(int acceptor){this->acceptor = acceptor;}
private:
    int connector;
    int acceptor;
};

class Event
{
public:
    Event(){}
    Event(int fd, function<void(int fd, void* arg)> callback, void* arg)
        :fd(fd), callback(callback), arg(arg){}
    virtual ~Event(){}

    void handle(){callback(this->fd, arg);}
    int getHandle(){return this->fd;}
private:
    int fd;
    function<void(int fd, void* arg)> callback;
    void* arg;

};

class EpollController
{
public:
    EpollController(){}
    virtual ~EpollController(){}

    void initialize(int nEvents)
    {
        this->nEvents = nEvents;
        epollFd = epoll_create1(0);
        if (-1 == epollFd)
        {
            cout << "failed to create epoll fd, errno: " << errno << endl;
            return;
        }
        epollEvents = new epoll_event[nEvents];
    }

    void addFd(int fd)
    {
        struct epoll_event event;
        event.events = EPOLLET | EPOLLIN;
        event.data.fd = fd; 
        epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    }

    void delFd(int fd)
    {
        struct epoll_event event;
        event.events = EPOLLET | EPOLLIN;
        event.data.fd = fd;
        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &event);
    }

    void wait(vector<int>& activeFd)
    {
        int nfds = epoll_wait(epollFd, epollEvents, nEvents, -1);
        for (int i = 0; i < nfds; i++)
        {
            uint32_t what = epollEvents[i].events;
            uint32_t fd = epollEvents[i].data.fd;
            if (epollEvents[i].events & what)
            {
                activeFd.push_back(fd);
            }
        }
    }

private:
    int epollFd;
    int nEvents;
    struct epoll_event* epollEvents;
};

class EventController
{
public:
    EventController(int nEvents)
    {
        epollController.initialize(nEvents);
    }
    virtual ~EventController(){}
    void addEvent(Event event)
    {
        if (!events.count(event.getHandle()))
        {
            events.insert(make_pair(event.getHandle(), event));
            epollController.addFd(event.getHandle());
        }
    }
    void delEvent(Event event)
    {
        if (events.count(event.getHandle()))
        {
            events.erase(event.getHandle());
            epollController.delFd(event.getHandle());
        }
    }

    void loop()
    {
        vector<int> activeFd;
        
        int done = 0;
        while(!done)
        {
            activeFd.clear();
            epollController.wait(activeFd);
            for (int i = 0; i < activeFd.size(); i++)
            {
                Event activeEvent = events[activeFd[i]];
                activeEvent.handle();
            }
            done = 1;
        }
    }
private:
    unordered_map<int, Event> events;
    EpollController epollController;
};


int main(int argc, char** argv)
{
    int numPipes = 4100; // ulimit -n 最多打开的文件描述符
    int numActive = 4100;
    Pipe* pipes = createPipes(numPipes);
    if (nullptr == pipes)
    {
        cout << "failed to create pipes" << endl; 
        return 0;
    }

    EventController eventController(numActive);
    for (int i = 0 ; i < 5; i ++)
    {
        timeval* tv = runOnce(eventController, pipes, numPipes, numActive);
        cout << "used time: " << tv->tv_sec * 1000000L + tv->tv_usec << "us" <<endl;;
    }
    return 0;
}

Pipe* createPipes(int num)
{
    if (num <= 0)
    {
        return nullptr;
    }

    Pipe* pipes = new Pipe[num];
    for (int i = 0; i < num; i++)
    {
        cout << "npipes: " << i << endl;
        createPipe(pipes[i]);
    }

    return pipes;
}

bool createPipe(Pipe& pipe)
{
    int listener = -1;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listener)
    {
        cout << "failed to create socket, errno: " << errno << endl;
        return false;
    }
    sockaddr_in listenerAddr;
    memset(&listenerAddr, 0, sizeof(listenerAddr));
    listenerAddr.sin_family = AF_INET;
    listenerAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    listenerAddr.sin_port = htons(0);

    if ( -1 == bind(listener, (const sockaddr*)& listenerAddr, sizeof(listenerAddr)))
    {
        cout << "failed to bind socket, errno: " << errno << endl;
        return false;
    }

    if (-1 == listen(listener, 1))
    {
        cout << "faile to listen, errno: " << errno << endl;
        return false;
    }

    int connector = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == connector)
    {
        cout << "failed to create socket, errno: " << errno << endl;
        return false;
    }
    struct sockaddr_in connectorAddr;
    memset(&connectorAddr, 0, sizeof(connectorAddr));
    socklen_t size =  sizeof(connectorAddr);
    if (-1 == getsockname(listener, (sockaddr*)& connectorAddr, &size))
    {
        cout << "failed to get socket name, errno: " << errno << endl;
        return false;
    }
    if (-1 == connect(connector, (const sockaddr*)& connectorAddr, sizeof(connectorAddr)))
    {
        cout << "failed to connect, errno: " << errno <<endl;
        return false;
    }

    size = sizeof(listenerAddr);
    int acceptor =  accept(listener, (sockaddr*)& listenerAddr, &size);
    if (-1 == acceptor)
    {
        cout << "failed to accept, errno: " << errno << endl;
        return false;
    }

    cout << "connector: " << connector << ", acceptor: " << acceptor << endl;
    pipe.setConnector(connector);    // 0 write
    pipe.setAcceptor(acceptor); // 1 read

    close(listener);

    return true;
}


int received = 0;
int done = 0;
int toSend = 0;
int failed = 0;
void callback(int fd, void* arg)
{
    int* connectFd = (int*) arg;

    char ch;
    int n = recv(fd, &ch, sizeof(ch), 0);
    if (n >= 0)
    {
        received += n;
    }
    else
    {
        failed ++;
    }
    
    if (toSend > 0)
    {
        n = send(*connectFd, "e", 1, 0);
        if (1 != n)
        {
            // todo fail 
        }
        toSend --;
        done ++;
    }
    
}

static struct timeval*
runOnce(EventController& eventController, Pipe* pipes, int nPipes, int nActive)
{
    static struct timeval ts, te;

    for (int i = 0; i < nPipes; i++)
    {
        int connector = pipes[i].getConnector();
        Event event(pipes[i].getAcceptor(), callback, (void*)&connector);
        eventController.delEvent(event);    // 先清除之前添加的
        eventController.addEvent(event);
    }

    // eventController.loop();
    
    received = 0;
    toSend = 0;
    done = 0;
    failed = 0;

    int sapce = nPipes / nActive;
    for (int i = 0; i < nActive; i++)
    {
        send(pipes[i * sapce].getConnector(), "e", 1, 0);
        toSend ++;
    }

    int cnt= 0;
    gettimeofday(&ts, nullptr);
    do
    {
        eventController.loop();
        cnt ++;
    }while(received != done);
    gettimeofday(&te, nullptr);

    cout << "received: " << received << endl;
    cout << "done: " << done << endl;
    cout << "cnt: " << cnt << endl;
    cout << "failed: " << failed << endl;

    timersub(&te, &ts, &te);

    return (&te);
}
