#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

int setup(string& ip, uint16_t& port);
void* processRequest(void* arg);

int main(int argc, char** argv)
{

    string ip = "127.0.0.1";
    uint16_t port = 8088;

    int serverSock = setup(ip, port);
    if (-1 == serverSock)
    {
        cout << "faile to set up server" << endl;
        return 0;
    }

    cout << "server is running on: [ " << ip << " ," << port << " ]" << endl; 

    int clientSock = -1;
    struct sockaddr_in clientSockAddr;
    socklen_t clientSockAddrLen = sizeof(clientSockAddr);
    memset(&clientSockAddr, 0, sizeof(clientSockAddr));

    pthread_t thread;
    while (1)
    {
        
        clientSock = accept(serverSock, (sockaddr*) &clientSockAddr, &clientSockAddrLen);
        if (-1 == clientSock)
        {
            cout << "failed to accept new connection, errno: " << errno << endl;
            return 0;
        }

        if (0 != pthread_create(&thread, nullptr, processRequest, (void*)&clientSock));
    }
    return 0;
}

int setup(string& ip, uint16_t& port)
{
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == serverSock)
    {
        cout << "failed to create server socket, errno: " << errno << endl;
        return -1;
    }

    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr, 0 , sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    if (!inet_aton(ip.c_str(), &serverSockAddr.sin_addr)) // 失败返回0
    {
        cout << "cannot convert " << ip << " to in_addr, errno" << errno << endl;
        return -1;
    }
    serverSockAddr.sin_port = htons(port);

    if (-1 == bind(serverSock, (const sockaddr*) &serverSockAddr, sizeof(serverSock)))
    {
        cout << "cannot bind server socket with server addr, errno: " << errno << endl;
        return -1;
    }

    // 端口号为0，系统就会随机设定一个端口号
    if (0 == port)
    {
        socklen_t serverSockLen = sizeof(serverSock);
        if (-1 == getsockname(serverSock, (sockaddr*) &serverSockAddr, &serverSockLen))
        {
            cout << "failed to get server info, errno: " << errno << endl;
        }

        port = ntohs(serverSockAddr.sin_port);
    }

    if (-1 == listen(serverSock, 5))
    {
        cout << "failed to listen, errno: " << errno << endl;
        return -1;
    }

    return serverSock;
}

void* processRequest(void* arg)
{
    
}