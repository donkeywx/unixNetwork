#include <iostream>
#include <stdio.h>   // for perror
#include <stdlib.h>
#include <sys/socket.h> // for sockaddr_in
#include <netinet/in.h>
#include <string.h>     // for memset
#include <unistd.h>     // for close
using namespace std;

int main()
{
    
    int serverFd = 0;
    if (-1 == (serverFd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        
        perror("failed to create socket");
        exit(1);
    }

    int port = 8088;    
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (-1 == bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        
        perror("failed to bind");
        exit(1);
    }

    if (-1 == listen(serverFd, 10))
    {
        
        perror("failed to listen");
        exit(1);
    }
    printf("start to listen\n");

    char buff[4096];
    int n = 0;

    struct sockaddr_in clientAddr;
    socklen_t clientAddrlen = sizeof(clientAddr);

    int clientFd = 0;
    if (-1 == (clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrlen)))
    {
        perror("failed to accept client socket");
            exit(1);
    }

    // while(1)
    // {

    //     printf("received a connection from client\n");

    //     // 
        n = recv(clientFd, buff, 1, 0);
    //     if (n <= 0)
    //     {
    //         std::cout << "recv result: " << n << std::endl;
    //         std::cout << "errorno is: " << errno << ", "<< strerror(errno) << std::endl;
    //         break;
    //     }
    //     buff[n] = '\0';
    //     printf("recv message from client: %s\n", buff);
    //     // close(clientFd);
    // }
close(clientFd);
    return 0;
}
