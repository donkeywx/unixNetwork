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

    int listen(int backlog);
    int bind(const struct sockaddr *addr, socklen_t addrlen);
    int accept(struct sockaddr *addr, socklen_t *addrlen);
    int connect(const struct sockaddr *addr, socklen_t addrlen);
    int send(const void *buf, size_t len, int flags);
    int recv(void *buf, size_t len, int flags);

    // todo sendto recvfrom
private:
    
private:
    // 句柄
    int m_sock;
    // 协议簇
    int m_domain;
    // 类型
    int m_type;
    // 协议
    int m_protocol; 
};
}

#endif