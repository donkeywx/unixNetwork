#include "httpServer.h"
#include <iostream>

namespace donkey
{
HttpServer::HttpServer()
{
    servlet = std::make_shared<Servlet>("not_found");
}
HttpServer::~HttpServer()
{

}

void HttpServer::handleClient(TCPConn::Ptr clientTcpConn)
{
    HttpSession::Ptr session = std::make_shared<HttpSession>(clientTcpConn);
    do
    {
        auto req = session->recvRequest();
        // std::cout << "end" << std::endl;
        if(!req)
        {
            break;
        }

        // req->dump(std::cout);

        HttpResponse::Ptr rsp(new HttpResponse(req->getVersion(), req->isClose()));
        servlet->handle(req, rsp, session);
        
        // m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);
    } while(0);
    // std::cout << "session close" << std::endl;
    session->close();
}
}