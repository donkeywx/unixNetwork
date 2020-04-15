#ifndef __DONKEY_HTTP_SESSION_H__
#define __DONKEY_HTTP_SESSION_H__

#include "tcpConn.h"
#include "httpRequest.h"
#include "httpResponse.h"

namespace donkey
{
class HttpSession: public TCPConn
{
public:
    HttpSession(Socket::Ptr sock);
    ~HttpSession();

    HttpRequest::Ptr recvRequest();
    int sendReponse(HttpReponse::Ptr);
private:

};
}

#endif