#include "tcpConn.h"

namespace donkey
{
TCPConn::TCPConn(Socket::Ptr sock)
    : m_sock(sock)
{

}

TCPConn::~TCPConn()
{

}
int TCPConn::read(void* buffer, size_t length)
{
    if (isConnected())
    {
        return -1;
    }

    return m_sock->recv(buffer, length, 0);
}
int TCPConn::write(void* buffer, size_t length)
{
    if (isConnected())
    {
        return -1;
    }

    return m_sock->send(buffer, length, 0);
}
bool TCPConn::isConnected()
{
    return m_sock && m_sock->isConnected();
}
void TCPConn::close()
{
    if (m_sock)
    {
        m_sock->close();
    }
}
};