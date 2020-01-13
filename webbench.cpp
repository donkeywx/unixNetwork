#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h> // for sigaction

using namespace std;

enum Method
{
    GET,
    POST,
    HEAD
};

string buildRequest(string url, Method method, string& host, int& port);
void bench(int nClients, const char* host, const int port, const char* request);
void benchCore(int nClients, const char* host, const int port, const char* request, 
    int& nRequests, int& nFailures, int& nReadBytes);
int connect2Server(const char* host, const int port);
void alarmHandler(int signal);


int main(int argc, char** argv)
{
    if (2 != argc)
    {
        cout << "require para" << endl;
        return 0;
    }
    string host = "";
    int port = 0;
    string request = buildRequest(argv[1], GET, host, port);

    bench(1, host.c_str(), port, request.c_str());

    return 0;
}

string buildRequest(string url, Method method, string& host, int& port)
{
    if (0 != url.find("http://"))
    {
        cout << "invalid url: " << url << endl;
    }
    if (url.length() > 1500)
    {
        cout << "url is too long, length: " << url.length() << endl;
        return "";
    }

    string request = "";
    switch(method)
    {
        case GET:
            request += "GET";
        break;
        case POST:
        break;
        case HEAD:
        break;
    }
    request += " ";

    string uri = url.substr(url.find("://") + 3, url.length() - 7);
    cout << uri << endl;

    if (uri.npos == uri.find("/"))
    {
        cout << "invalid uri that should be end with '/'" << endl;
        return "";
    }
    string portStr = "";
    // 默认不使用代理
    // path and port
    if (uri.npos != uri.find(":") && uri.find(":") < uri.find("/"))
    {
        host = uri.substr(0, uri.find(":"));
        portStr = uri.substr(uri.find(":") + 1, uri.find("/") - uri.find(":") - 1);
    }
    // path
    else
    {
        host = uri.substr(0, uri.find("/"));
    }
    port = atoi(portStr.c_str());

    cout << host << " " << port << endl;

    request += uri.substr(uri.find("/"), uri.length() - uri.find("/"));
    request += " ";
    request += "HTTP/1.0";
    request += "\r\n";
    
    request += "User-Agent: WebBench\r\n";
    request += "Host: " + host + "\r\n";
    request += "\r\n";
    cout << request << endl;

    return request;
}

void bench(int nClients, const char* host, const int port, const char* request)
{
    // test server is available
    int client = connect2Server(host, port);
    if (-1 == client)
    {
        return;
    }

    int myPipe[2];
    if (pipe(myPipe))
    {
        cout << "failed to create pipe, errno: " << errno << endl;
        return;
    }

    pid_t pid = -1;
    for (int i = 0; i < nClients; i++)
    {
        pid = fork();
        if (pid <= 0)
        {
            sleep(1);
            // printf("child the pid is: %d, getpid is: %d, getppid is: %d\n", pid, getpid(), getppid());
            break;
        }
    }

    if (pid < 0)
    {
        printf("fork failed, errno: %d", errno);
        return;
    }


    if (0 == pid)
    {
        // i am a child
        int nRequests = 0;
        int nFailures = 0;
        int nReadBytes = 0; 
        benchCore(nClients, host, port, request, nRequests, nFailures, nReadBytes);

        FILE* f = fdopen(myPipe[1], "w");
        if (nullptr == f)
        {
            cout << "failed to open pipe, errno: " << errno << endl;
            return;
        }
        fprintf(f, "%d %d %d\n", nRequests, nFailures, nReadBytes);
        fclose(f);

        return;
    }
    else
    {
        // i am father

        FILE* f = fdopen(myPipe[0], "r");
        if (nullptr == f)
        {
            cout << "failed to open pipe, errno: " << errno << endl;
            return;
        }

        setvbuf(f, nullptr, _IONBF, 0);

        int nRequests = 0;
        int nFailures = 0;
        int nReadBytes = 0; 

        int i, j, k;
        while (1)
        {
            pid = fscanf(f, "%d %d %d", &i, &j, &k);
            if (pid < 2)
            {
                cout << "some of our childrens died" << endl;
                break;
            }

            nRequests += i;
            nFailures += j;
            nReadBytes += k;

            if (0 == --nClients)
            {
                break;
            }
        }
        
        fclose(f);
        

        printf("\nSpeed=%d pages/min, %d bytes/sec.\nRequest: %d susceed, %d failed.\n",
            (int)((nRequests + nFailures) / (10.0f/60.0f)),
            (int)(nReadBytes / (float)10),
            nRequests,
            nFailures);
    }
    
}

static int timerExpired = 0;
void alarmHandler(int signal)
{
    timerExpired = 1;
}

void benchCore(int nClients, const char* host, const int port, const char* request,
    int& nRequests, int& nFailures, int& nReadBytes)
{
    // 设置定时器
    struct sigaction sa;
    sa.sa_handler = alarmHandler;
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, nullptr))
    {
        return;
    }
    alarm(10);

    int requestLen = strlen(request);
    char buf[1500];
    while(1)
    {
        if (1 == timerExpired)
        {
            return;
        }

        int clientSock = connect2Server(host, port);
        if(clientSock < 0)
        {
            nFailures++;
            continue;
        }

        if (requestLen != write(clientSock, request, requestLen))
        {
            nFailures++;
            close(clientSock);
            continue;
        }

        while (1)
        {
            if (1 == timerExpired)
            {
                break;
            }

            int nBytes = read(clientSock, buf, sizeof(buf));
            if (nBytes < 0)
            {
                nFailures++;
                break;
            }
            else if (0 == nBytes)
            {
                break;
            }
            else
            {
                nReadBytes += nBytes;
            }
        }

        if (close(clientSock))
        {
            nFailures++;
            continue;
        }
        nRequests ++;
    }
}



int connect2Server(const char* host, const int port)
{
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    if (!inet_aton(host, &serverAddr.sin_addr))
    {
        cout << "cannot convert " << host << " to in_addr, errno" << errno << endl;
        return -1;
    }
    serverAddr.sin_port = htons(port);
    
    if (-1 == connect(clientSock, (const sockaddr*) &serverAddr, sizeof(serverAddr)))
    {
        cout << "can not connet to server" << endl;
        return -1;
    }
    return clientSock;
}