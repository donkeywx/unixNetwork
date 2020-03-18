#include "ttcp_blocking.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <sys/time.h>

namespace net
{
int acceptOrDie(uint16_t port)
{
    int listenfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
    {
        printErr("failed to create listen socket", errno);
    }

    
    // 设置地址可以重用
    // 即使tcp连接处于time_wait状态，也可以绑定成功
    int yes = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        printErr("failed to set sockt", errno);
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (::bind(listenfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)))
    {
        printErr("failed to bind", errno);
        exit(1);
    }

    if (listen(listenfd, 5))
    {
        printErr("failed to listen", errno);
        exit(1);
    }

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t addrlen = 0;
    int sockfd = ::accept(listenfd, reinterpret_cast<sockaddr*>(&clientAddr), &addrlen);
    if (sockfd < 0)
    {
        printErr("failed to accept", errno);
        exit(1);
    }

    std::cout << "accept a new connection: " << sockfd;
    ::close(listenfd);
    return sockfd;
}

struct sockaddr_in resolveOrDie(const char* host, uint16_t port)
{
    struct hostent* he = ::gethostbyname(host); // netdb
    if (!he)
    {
        perror("gethostbyname");
        exit(1);
    }
    assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return addr;
}

int write_n(int sockfd, const void* buf, int length)
{
    int written = 0;
    
    while(written < length)
    {
        ssize_t nw = ::write(sockfd, static_cast<const char*> (buf) + written, length - written);
        if (nw > 0)
        {
            written += static_cast<int>(nw);
        }
        else if (0 == nw)
        {
            break;
        }
        else if (errno != EINTR)
        {
            printErr("failed to write", errno);
            break;
        }
    }
    return written;
}

int read_n(int sockfd, void* buf, int length)
{
    int nread = 0;

    while(nread < length)
    {
        ssize_t nr = :: read(sockfd, static_cast<char*>(buf) + nread, length - nread);
        if (nr > 0)
        {
            nread += static_cast<int> (nr);
        }
        else if (0 == nr)
        {
            break;
        }
        else if (EINTR != errno)
        {
            printErr("failed to read", errno);
            break;
        }
        
    }
    return nread;
}

void transmit(const Options& opt)
{
    struct sockaddr_in addr = resolveOrDie(opt.host.c_str(), opt.port);
    std::cout << "connecting to " << inet_ntoa(addr.sin_addr) << ":" << opt.port << std::endl;

    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        printErr("failed to create socket", errno);
        exit(1);
    }

    int ret = ::connect(sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
    if (ret)
    {
        printErr("failed to  connect", errno);
        exit(1);   
    }
    std::cout << "connected" << std::endl;

    clock_t startTime = clock();
    // 告知对方将要发送的长度和个数
    struct SessionMessage sessionMessage = {0, 0};
    sessionMessage.number = htonl(opt.number);
    sessionMessage.length = htonl(opt.length);
    if (sizeof(SessionMessage) != write_n(sockfd, &sessionMessage, sizeof(SessionMessage)))
    {
        printErr("failed to wirte session message", 0);
        exit(1);
    }

    const int totalLen = static_cast<int>(sizeof(int32_t) + opt.length);
    PayloadMessage* payload = static_cast<PayloadMessage*>(::malloc(totalLen));
    assert(payload);
    payload->length = htonl(opt.length);
    for (int i = 0; i < opt.length; ++i)
    {
        payload->data[i] = "0123456789ABCDEF"[i % 16];
    }

    double total_mb = 1.0 * opt.length * opt.number / 1024 / 1024;
    std::cout << total_mb << " MiB in total" << std::endl;

    for (int i = 0; i < opt.number; ++i)
    {
        int nw = write_n(sockfd, payload, totalLen);
        assert(nw == totalLen);

        int ack = 0;
        int nr = read_n(sockfd, &ack, sizeof(ack));
        assert(nr == sizeof(ack));
        ack = ntohl(ack);
        assert(ack == opt.length);
    }

    ::free(payload);
    ::close(sockfd);

    clock_t endTime = clock();
    double elapsed = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    std::cout << elapsed << "s, " << total_mb / elapsed << "Mib/s" << std::endl;
}

void receive(const Options& opt)
{
    int sockfd = acceptOrDie(opt.port);

    struct SessionMessage sessionMessage = { 0, 0 };
    if (read_n(sockfd, &sessionMessage, sizeof(sessionMessage)) != sizeof(sessionMessage))
    {
        perror("read SessionMessage");
        exit(1);
    }

    sessionMessage.number = ntohl(sessionMessage.number);
    sessionMessage.length = ntohl(sessionMessage.length);
    printf("receive number = %d\nreceive length = %d\n",
            sessionMessage.number, sessionMessage.length);
    const int total_len = static_cast<int>(sizeof(int32_t) + sessionMessage.length);
    PayloadMessage* payload = static_cast<PayloadMessage*>(::malloc(total_len));
    assert(payload);

    for (int i = 0; i < sessionMessage.number; ++i)
    {
        payload->length = 0;
        if (read_n(sockfd, &payload->length, sizeof(payload->length)) != sizeof(payload->length))
        {
            perror("read length");
            exit(1);
        }
        payload->length = ntohl(payload->length);
        assert(payload->length == sessionMessage.length);
        if (read_n(sockfd, payload->data, payload->length) != payload->length)
        {
            perror("read payload data");
            exit(1);
        }
        int32_t ack = htonl(payload->length);
        if (write_n(sockfd, &ack, sizeof(ack)) != sizeof(ack))
        {
            perror("write ack");
            exit(1);
        }
    }
    ::free(payload);
    ::close(sockfd);
}

}

