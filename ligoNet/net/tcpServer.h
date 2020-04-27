#ifndef __DONKEY_TCPSERVER_H__
#define __DONKEY_TCPSERVER_H__

#include <libgo/coroutine.h>
#include "tcpConn.h"
namespace donkey
{


class TCPServer: public Noncopyable, public std::enable_shared_from_this<TCPServer>
{
public:
    typedef std::shared_ptr<TCPServer> Ptr;

    TCPServer();
    virtual ~TCPServer();
    int listen(const char* ip, uint16_t port);
    void start();
    void stop();
    void startAccept(Socket::Ptr sock);
    bool isStop(){return m_stop;}

    virtual void handleClient(TCPConn::Ptr clientTcpConn);
private:
    // 负责监听的sokcet
    Socket::Ptr m_listSock;
    bool m_stop;
    co::Scheduler* sched;
};
}

#endif