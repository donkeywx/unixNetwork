#ifndef __DONKEY_HTTP_SESSION_H__
#define __DONKEY_HTTP_SESSION_H__

#include "tcpConn.h"
#include "http/httpRequest.h"
#include "http/httpResponse.h"

namespace donkey
{
class HttpSession: public TCPConn
{
public:
    HttpSession(Socket::Ptr sock);
    ~HttpSession();

    HttpRequest::Ptr recvRequest();
    int sendReponse(HttpResponse::Ptr);
private:

};
}

#endif