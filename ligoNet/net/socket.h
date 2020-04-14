#ifndef __DONKEY_SOCKET_H__
#define __DONKEY_SOCKET_H__

#include "nonCopyable.h"
#include <memory>
#include <libgo/netio/unix/hook.h>


namespace donkey
{
class Socket;

class Socket: public Noncopyable, public std::enable_shared_from_this<Socket> 
{
public:
    static const int INVALID_SOCKET = -1;

    enum Domain
    {
        /// IPv4 socket
        IPv4 = PF_INET,
        /// IPv6 socket
        IPv6 = PF_INET6,
        /// Unix socket
        UNIX = PF_UNIX
    };
    enum Type
    {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };
public:
    typedef std::shared_ptr<Socket> Ptr;
    Socket(Domain domain, Type type, int protocol = 0);
    ~Socket();

    int listen(const char* serverIp, uint16_t serverPort, int backlog = 10);
    Socket::Ptr accept(struct sockaddr *addr, socklen_t *addrlen);
    int connect(const char* remoteIp, uint16_t remotePort);
    int send(const void *buf, size_t len, int flags);
    int recv(void *buf, size_t len, int flags);
    int close();
    bool isConnected(){return m_isConnected;}
    // todo sendto recvfrom

    void setSock(int sock){m_sock = sock;}
    void setConnected(bool connected){m_isConnected = connected;}
private:
    static Socket::Ptr createAcceptedSocket(int acceptedSock, Domain domain, Type type, int protocol);
    void setAddr(const char* ip, uint16_t port, sockaddr_in& addr);
    int bind(const struct sockaddr_in *addr);
private:
    // 句柄
    int m_sock;
    // 协议簇
    Domain m_domain;
    // 类型
    Type m_type;
    // 协议
    int m_protocol; 
    // 状态
    bool m_isConnected;
};
}

#endif