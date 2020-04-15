#ifndef __DONKEY_TCPCONN_H__
#define __DONKEY_TCPCONN_H__

#include "socket.h"
namespace donkey
{
class TCPConn: public Noncopyable, public std::enable_shared_from_this<TCPConn>
{
public:
    TCPConn(Socket::Ptr sock);
    ~TCPConn();

    int read(void* buffer, size_t length);
    int write(void* buffer, size_t length);
    bool isConnected();
    void close();
private:
    Socket::Ptr m_sock;
};
}

#endif