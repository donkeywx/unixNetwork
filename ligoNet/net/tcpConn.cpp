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
    if (!isConnected())
    {
        return -1;
    }

    return m_sock->recv(buffer, length, 0);
}
int TCPConn::write(const void* buffer, size_t length)
{
    if (!isConnected())
    {
        return -1;
    }

    return m_sock->send(buffer, length, 0);
}

int TCPConn::readFixSize(void* buffer, size_t length)
{
    size_t offset = 0;
    int64_t left = length;
    while(left > 0) {
        int64_t len = read((char*)buffer + offset, left);
        if(len <= 0) {
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
}
int TCPConn::writeFixSize(const void* buffer, size_t length)
{
    size_t offset = 0;
    int64_t left = length;
    while(left > 0) {
        int64_t len = write((const char*)buffer + offset, left);
        if(len <= 0) {
            return len;
        }
        offset += len;
        left -= len;
    }
    return length;
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