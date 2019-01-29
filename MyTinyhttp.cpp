/*************************************************************************
	> File Name: MyTinyhttp.cpp
	> Author:donkeywx 
	> Mail:wangkang 
	> Created Time: Thu 24 Jan 2019 10:30:28 PM CST
    > tinyhttp
 ************************************************************************/

#include <stdio.h>   // for perror
#include <stdlib.h>  // for exit
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> // for sockaddr_in
#include <string.h>  // for memset
#include <pthread.h>
using namespace std;

int startup(unsigned short&);   // 
void error(const char *sc);     // 输出错误信息，并退出
void* acceptRequest(void*);   // 处理请求 
int main(int argc, char** argv)
{
    int serverSock = -1;
    unsigned short port = 0;
    
    serverSock = startup(port);

    int clientSock = -1;
    struct sockaddr_in clientName;
    socklen_t clientNameLen = sizeof(clientName);

    while(1)
    {

        // 失败返回-1
        clientSock = accept(serverSock, (struct sockaddr*)&clientName, &clientNameLen);

        if (-1 == clientSock)
        {
            error("failed to accept client socket");
        }

        // pthread_create(pthread_t* restrict tidp,指向线程标识符的指针
        // const pthread_attr_t* restrict attr, 设置线程属性
        // void* (*start_run)(void), 运行函数的起始地址
        // void* restrict arg); 运行函数的参数
        // 返回0
        pthread_t newThread;
        if (0 != pthread_create(&newThread, NULL, acceptRequest, &clientSock))
        {
            
            error("failed to create thread");
        }
    }
    return 0;
}
void error(const char* sc)
{

    perror(sc); // 用来将上一个函数发生错误的原因输出到标准错误(stderr)。
                // 参数string所指的字符串会先打印出，后面再加上错误原因字符串，
                // 此错误原因依照全局变量errno 的值来决定要输出的字符串。
    exit(1);
}
int startup(unsigned short& port)
{
    
    int serverSock = 0;

    // 创建套接
    // 原型: socket(int family, int type, int protocol)
    // family: 指定一个协议簇，AF_INET为IPV4协议，AF_INET6位IPV6协议，AF_LOCAL──指定为UNIX 协议域
    // type SOCK_STREAM, SOCK_DGRAM等，分别表示字节流和数据报
    // protocol 系统针对每一个协议簇与类型提供了一个默认的协议
        // 我们通过把protocol设置为0来使用这个默认的值.
        // 比如IPv4可以用来实现TCP或UDP等等传输层协议，所以称为协议簇。
        // 相应的传输层的协议就简单地称为协议。
        // 常见的协议有TCP、UDP、SCTP，要指定它们分别使用宏IPPROTO_TCP、IPPROTO_UPD、IPPROTO_SCTP来指定。
    // 如果出现错误，它返回-1，并设置errno为相应的值，用户应该检测以判断出现什么错误
    serverSock = socket(PF_INET, SOCK_STREAM, 0);                                           
    if (-1 == serverSock)
    {
        error("failed to create server socket!");
    }

    // 设置服务器套接字
    struct sockaddr_in serverName;
    memset(&serverName, 0, sizeof(serverName));
    serverName.sin_family = AF_INET;    // 协议簇
    serverName.sin_port = htons(port);  // 指定端口 htons: 将主机无符号短整形数转换成网络
    serverName.sin_addr.s_addr = htonl(INADDR_ANY); // 地址 htonl: 将主机数转换成无符号长整型的网络字节顺序, 
   
    // 将服务器套接字绑定到指定端口和地址
    // bind函数将serverName中的服务器套接字地址和套接字描述符serverSock联系起来
    // 成功返回0,失败返回-1
    if (bind(serverSock, (struct sockaddr*)&serverName, sizeof(serverName)) < 0)
    {
        
        error("failed to bind!");
    }

    // 如果port为0，则自动分配一个端口
    if (0 == port)
    {
        socklen_t nameLen = sizeof(serverName);

        // 在调用bin函数后，getsockname返回内核分配的的本地端号
        if (-1 == getsockname(serverSock, (struct sockaddr*)&serverName, &nameLen))
        {

            error("failed to get socke name ");
        }

        // linux 2.2 版本以后
        // 使用两个队列分别存放：不完全连接请求和完全建立的连接请求
        // 不完全连接的长度可以使用/proc/sys/net/ipv4/tcp_max_syn_backlog设置
        // 完全连接请求就是第二个参数设置

        if (listen(serverSock, 5) < 0)
        {
            
            error("failed to listen");
        }
    }
}
void* acceptRequest(void* clientSock)
{

}

