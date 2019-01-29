/*************************************************************************
	> File Name: client.cpp
	> Author:donkeywx 
	> Mail:wangkang 
	> Created Time: Tue 29 Jan 2019 09:10:23 PM CST
 ************************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // for inet_pton
using namespace std;

int main(int argc, char** argv)
{
    
    int clientFd = 0;
    if (-1 == (clientFd = socket(AF_INET, SOCK_STREAM, 0)))
    {

        perror("failed to create socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    int serverPort = 8888;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    // inet_pton(int family, const char* strptr, void* addrptr)
    // 将字符串转换为s_addr，存储在sin_addr结构体中
    // 转换成功返回0，失败返回-1
    // 与之相反的函数：net_ntop(int family, const void* addrptr, char* strptr, size_t len)
    // 从数值格式(addrptr)转换到表达式(strptr)，该两个参数不能为空
    // 成功返回指向in_addr的指针(s_addr对应的类型)，错误返回NULL
    if (-1 == inet_pton(AF_INET, argv[1], &serverAddr.sin_addr))
    {
        
        perror("failed to transfor char* to s_addr");
        exit(1);
    }

    if (-1 == connect(clientFd, (struct sockaddr*)&serverAddr, sizeof(sockaddr)))
    {
        
        perror("failed to connect to server");
        exit(1);
    }
    printf("connect to server successfully\n");

    char sendLine[1024];
    printf("send message to server:\n");
    fgets(sendLine, 1024, stdin);
    if (-1 == send(clientFd, sendLine, strlen(sendLine), 0))
    {
        
        perror ("failed to send message");
        exit(1);
    }

    close(clientFd);

    return 0;
}
