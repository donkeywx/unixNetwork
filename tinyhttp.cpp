#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
#define STDIN   0
#define STDOUT  1
#define STDERR  2

int setup(string& ip, uint16_t& port);
void* processRequest(void* arg);
int getLine(int sock, char* buf, int size);
void serveFile(int clientSock, const char* fileName);
void executeCgi(int client, const char *path, const char *method, const char *query_string);
void headers(int, const char *);
void cat(int, FILE *);
void bad_request(int client);
void cannot_execute(int client);

int main(int argc, char** argv)
{

    string ip = "127.0.0.1";
    uint16_t port = 0;

    int serverSock = setup(ip, port);
    if (-1 == serverSock)
    {
        cout << "faile to set up server" << endl;
        return 0;
    }

    cout << "server is running on: [ " << ip << ", " << port << " ]" << endl; 

    int clientSock = -1;
    struct sockaddr_in clientSockAddr;
    socklen_t clientSockAddrLen = sizeof(clientSockAddr);
    memset(&clientSockAddr, 0, sizeof(clientSockAddr));

    string clientIp = "";
    uint16_t clietnPort = -1;
    pthread_t thread;
    while (1)
    {
        
        int clientSock = accept(serverSock, (sockaddr*) &clientSockAddr, &clientSockAddrLen);
        if (-1 == clientSock)
        {
            cout << "failed to accept new connection, errno: " << errno << endl;
            return 0;
        }
        clientIp = inet_ntoa(clientSockAddr.sin_addr);
        clietnPort = ntohl(clientSockAddr.sin_port);
        cout << "received a new connection: [ " << clientIp << ", "  << clietnPort << 
            " ]. Fd is: " << clientSock << endl;

        
        if (0 != pthread_create(&thread, nullptr, processRequest, (void*)(intptr_t) clientSock))
        {
            cout << "failed to create thread, errno: " << errno << endl;
        }
    }

    close(serverSock);

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

    if (-1 == bind(serverSock, (const sockaddr*) &serverSockAddr, sizeof(serverSockAddr)))
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
    int clientSock = (intptr_t) arg;

    char buf[1024];
    int index = 0;
    int nchars = getLine(clientSock, buf, sizeof(buf));

    // 获取method
    char method[255];
    int i = 0;
    while (' ' !=  buf[index] && i < sizeof(method) - 1)
    {
        method[i++] = buf[index++];
    }
    method[i] = '\0';

    // 两种方法都不是，直接返回
    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
    {
        close(clientSock);
        return nullptr;
    }

    // cgi
    bool isCgi = false;
    if (0 == strcasecmp(method, "POST"))
    {
        isCgi = true;
    }

    // 去除空格
    while (' ' == buf[index] && index < sizeof(buf))
    {
        index ++ ;
    }

    // 获取url
    char url[255];
    i = 0;
    while(' ' != buf[index] && i < sizeof(url) -1  && index < sizeof(buf))
    {
        url[i++] = buf[index++];
    }
    url[i] = '\0';

    char* queryStr = nullptr; 
    if (0 == strcasecmp(method, "GET"))
    {
        queryStr = url;
        while ('?' != *queryStr && '\0' != *queryStr)
        {
            queryStr ++;
        }
        
        if ('?' == *queryStr)
        {
            isCgi = true;
            *queryStr = '\0';
            queryStr++;
        }
    }

    // 获取请求的文件的相对路径
    char path[255];
    sprintf(path, "htdocs%s", url);
    if ('/' == path[strlen(path) - 1])
    {
        strcat(path, "index.html");
    }

    // 验证路径
    struct stat st;
    cout << path << endl;
    if (-1 == stat(path, &st))
    {
        // 找不到，丢弃header信息
        while(nchars > 0 && strcmp("\n", buf))
        {
            nchars = getLine(clientSock, buf, sizeof(buf));
        }

        cout << "path is not found" << endl;    // not found
    }
    else
    {
        // 如果是目录
        if (S_IFDIR == (st.st_mode & S_IFMT))
        {
            strcat(path, "/index.html");
        }
        
        if ((st.st_mode & S_IXUSR) ||
                (st.st_mode & S_IXGRP) ||
                (st.st_mode & S_IXOTH)    )
        {    
            isCgi = true;
        }

        if (!isCgi)
        {
            serveFile(clientSock, path);
        }
        else
        {
            executeCgi(clientSock, path, method, queryStr);
        }
        
    }

    close(clientSock);
    return nullptr;
}

int getLine(int sock, char* buf, int size)
{
    int cnt = 0;
    char c = '\0';
    int n = 0;
    while((cnt < size -1) && ('\n' != c))
    {
        n = recv(sock, &c, 1, 0);   // 单个字符接受
        
        if (n > 0)
        {
            if ('\r' ==  c) // 如果说是回车，则继续读取
            {
                n = recv(sock, &c, 1, MSG_PEEK); // 预读取一个字符，滑动窗口不变

                if ((n > 0) && ('\n' == c)) // 如果是换行
                {
                    // 继续接受单个字符，实际上和上面那个标志位MSG_PEEK读取同样的字符
                    // 读取完删除输入队列的数据，即滑动窗口，c == '\n'
                    recv(sock, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }

            buf[cnt ++] = c;
        }
        else
        {
            c = '\n';
        }

    }
    buf[cnt] = '\0';

    printf("%s\n", buf);
    // cout << "read a line: " << buf << endl;
    return cnt;
}

void serveFile(int clientSock, const char* fileName)
{
    char buf[1024];

    int nchars = 1;

    /* read & discard headers */
    buf[0] = 'A';
    buf[1] = '\0';
    while(nchars > 0 && strcmp("\n", buf))
    {
        nchars = getLine(clientSock, buf, sizeof(buf));
    }

    FILE* resource = fopen(fileName, "r");
    if (resource == NULL)
    {
        // not_found(client);
    }
    else
    {
        headers(clientSock, fileName);
        cat(clientSock, resource);
    }
    fclose(resource);
}

void executeCgi(int client, const char *path, const char *method, const char *query_string)
{
   char buf[1024];
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    int i;
    char c;
    int numchars = 1;
    int content_length = -1;

    buf[0] = 'A'; buf[1] = '\0';
    if (strcasecmp(method, "GET") == 0)
        while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
            numchars = getLine(client, buf, sizeof(buf));
    else if (strcasecmp(method, "POST") == 0) /*POST*/
    {
        numchars = getLine(client, buf, sizeof(buf));
        while ((numchars > 0) && strcmp("\n", buf))
        {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0)
                content_length = atoi(&(buf[16]));
            numchars = getLine(client, buf, sizeof(buf));
        }
        if (content_length == -1) {
            bad_request(client);
            return;
        }
    }
    else/*HEAD or other*/
    {
    }


    if (pipe(cgi_output) < 0) {
        cannot_execute(client);
        return;
    }
    if (pipe(cgi_input) < 0) {
        cannot_execute(client);
        return;
    }

    if ( (pid = fork()) < 0 ) {
        cannot_execute(client);
        return;
    }
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    if (pid == 0)  /* child: CGI script */
    {
        char meth_env[255];
        char query_env[255];
        char length_env[255];

        dup2(cgi_output[1], STDOUT);
        dup2(cgi_input[0], STDIN);
        close(cgi_output[0]);
        close(cgi_input[1]);
        sprintf(meth_env, "REQUEST_METHOD=%s", method);
        putenv(meth_env);
        if (strcasecmp(method, "GET") == 0) {
            sprintf(query_env, "QUERY_STRING=%s", query_string);
            putenv(query_env);
        }
        else {   /* POST */
            sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
            putenv(length_env);
        }
        execl(path, NULL);
        exit(0);
    } else {    /* parent */
        close(cgi_output[1]);
        close(cgi_input[0]);
        if (strcasecmp(method, "POST") == 0)
        {
            for (i = 0; i < content_length; i++) {
                recv(client, &c, 1, 0);
                write(cgi_input[1], &c, 1);
                cout << (char)c;
            }
            cout << endl;
        }
        while (read(cgi_output[0], &c, 1) > 0)
        {
            send(client, &c, 1, 0);
            cout << (char)c;
        }
        cout << endl;
        close(cgi_output[0]);
        close(cgi_input[1]);
        waitpid(pid, &status, 0);
    }
}

void headers(int client, const char *filename)
{
    char buf[1024];
    (void)filename;  /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

void cat(int client, FILE *resource)
{
    char buf[1024];

    fgets(buf, sizeof(buf), resource);
    while (!feof(resource))
    {
        send(client, buf, strlen(buf), 0);
        fgets(buf, sizeof(buf), resource);
    }
}

void bad_request(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "<P>Your browser sent a bad request, ");
    send(client, buf, sizeof(buf), 0);
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    send(client, buf, sizeof(buf), 0);
}

void cannot_execute(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
    send(client, buf, strlen(buf), 0);
}
