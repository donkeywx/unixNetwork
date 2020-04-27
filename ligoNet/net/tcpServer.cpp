#include "tcpServer.h"
#include <libgo/coroutine.h>
#include <iostream>
#include <thread>
namespace donkey
{
TCPServer::TCPServer()
    : m_listSock(std::make_shared<Socket>(Socket::Domain::IPv4, Socket::Type::TCP, 0))
    , m_stop(false)
{

}
TCPServer::~TCPServer()
{
    if (m_listSock)
    {
        m_listSock->close();
    }
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
    std::thread t2([]{co_sched.Start(2); });
    t2.detach();

    sched = co::Scheduler::Create();
    go co_scheduler(sched) []{
        printf("run in my scheduler.\n");
    };
    sched->Start(0, 10); 
    
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
                go co_scheduler(sched) std::bind(&TCPServer::handleClient, shared_from_this(), clientTcpConn);
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