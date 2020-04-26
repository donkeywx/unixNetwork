#ifndef __DONKEY_HTTP_SERVER_H__
#define __DONKEY_HTTP_SERVER_H__

#include "http/servlet.h"
#include "tcpServer.h"
#include "http/httpSession.h"
namespace donkey
{
class HttpServer: public TCPServer
{
public:
    typedef std::shared_ptr<HttpServer> Ptr;
    HttpServer();
    virtual ~HttpServer();
    virtual void handleClient(TCPConn::Ptr clientTcpConn);
private:
    Servlet::Ptr servlet;
};
}

#endif