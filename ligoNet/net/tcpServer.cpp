#include "tcpServer.h"
#include <libgo/coroutine.h>
#include <iostream>
namespace donkey
{
TCPServer::TCPServer()
    : m_listSock(std::make_shared<Socket>(Socket::Domain::IPv4, Socket::Type::TCP, 0))
    , m_stop(false)
{

}
TCPServer::~TCPServer()
{

}
int TCPServer::listen(const char* ip, uint16_t port)
{
    if (nullptr == m_listSock)
    {
        return -1;
    }

    return m_listSock->listen(ip, port);
}
void TCPServer::start()
{
    if (isStop())
    {
        return ;
    }
    m_stop = false;
    go std::bind(&TCPServer::startAccept, shared_from_this(), m_listSock);
    co_sched.Start();
}

void TCPServer::startAccept(Socket::Ptr sock)
{
    while (!isStop())
    {
        Socket::Ptr clientSock = sock->accept();
        if (clientSock)
        {
            TCPConn::Ptr clientTcpConn = std::make_shared<TCPConn>(clientSock);
            
            if (clientTcpConn)
            {
                // std::cout << "accept a  new connection" << std::endl;
                go std::bind(&TCPServer::handleClient, shared_from_this(), clientTcpConn);
            }
            else
            {
                std::cout << "accpet a bad tcp connection" << std::endl;   
            }
            
        }
        else
        {
            std::cout << "accpet a bad socket" << std::endl;
        }
        

    }
    
}

void TCPServer::handleClient(TCPConn::Ptr clientTcpConn)
{
    std::cout << "hello handle" << std::endl;
}


void TCPServer::stop()
{
    m_stop = false;
}
}