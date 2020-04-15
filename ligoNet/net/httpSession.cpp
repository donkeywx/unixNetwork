#include "httpSession.h"

namespace donkey
{
HttpSession::HttpSession(Socket::Ptr sock)
    : TCPConn(sock)
{

}
HttpSession::~HttpSession()
{

}

HttpRequest::Ptr HttpSession::recvRequest()
{
    HttpRequest::Ptr httpRequest(new HttpRequest());

    return httpRequest;
}

int HttpSession::sendReponse(HttpReponse::Ptr)
{
    return 0;
}
}