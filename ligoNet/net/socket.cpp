#include "socket.h"
#include <arpa/inet.h>
#include <iostream>


namespace donkey
{
Socket::Socket(Domain domain, Type type, int protocol)
    : m_sock(-1), m_domain(domain)
    , m_type(type), m_protocol(protocol)
    , m_isConnected(false)
{

}
Socket::~Socket()
{
    close();
}

int Socket::listen(const char* serverIp, uint16_t serverPort, int backlog)
{
    m_sock = ::socket(m_domain, m_type, m_protocol);
    int val = 1;
    setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &val, (socklen_t)sizeof(int));
    if (INVALID_SOCKET == m_sock)
    {
        std::cout << "failed to create socket, " << strerror(errno) << std::endl;
        return -1;
    }

    sockaddr_in listenAddr;
    setAddr(serverIp, serverPort, listenAddr);
    int ret = bind(&listenAddr);
    if (ret)
    {
        std::cout << "failed to bind, " << strerror(errno) << std::endl;
        return ret;
    }

    ret = ::listen(m_sock, backlog);
    if (ret)
    {
        std::cout << "failed to listen, [" 
            << serverIp << ":" << serverPort << "]" << std::endl;
    }

    return ret;
}

void Socket::setAddr(const char* ip, uint16_t port, sockaddr_in& addr)
{
    memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		hostent* host = gethostbyname(ip);
		if (nullptr == host)
		{
            std::cout << "failed to get host by name, ip=%s " << ip << std::endl;
			return;
		}

		addr.sin_addr.s_addr = *(uint32_t*)host->h_addr;
	}
}

int Socket::bind(const struct sockaddr_in *addr)
{
    return ::bind(m_sock, (const struct sockaddr*)addr, sizeof(sockaddr));
}

Socket::Ptr Socket::accept()
{
    sockaddr_in client;
    socklen_t len = sizeof(sockaddr_in);
    int acceptedSock = ::accept(m_sock, (sockaddr*)& client, &len);
    if (-1 == acceptedSock)
    {
        std::cout << "accept error: " << m_sock << " " << strerror(errno) << std::endl;
        return nullptr;
    }
    else
    {
        std::cout << "accept a new socket: " << acceptedSock << std::endl;
    }
    
    return createAcceptedSocket(acceptedSock, m_domain,
        this->m_type, this->m_protocol);
}

Socket::Ptr Socket::createAcceptedSocket(int acceptedSock, Domain domain, Type type, int protocol)
{
    Socket::Ptr sock(new Socket(domain, type, protocol));


    int val = 1;
    setsockopt(sock->m_sock, SOL_SOCKET, SO_REUSEADDR, &val, (socklen_t)sizeof(int));
    sock->setSock(acceptedSock);
    sock->setConnected(true);
    return sock;
}

int Socket::connect(const char* remoteIp, uint16_t remotePort)
{
    struct sockaddr_in remoteAddr;
    setAddr(remoteIp, remotePort, remoteAddr);

    int ret = ::connect(m_sock, (const sockaddr*)&remoteAddr, sizeof(sockaddr));
    if (ret)
    {
        std::cout << "failed to connect remote server, [" 
            << remoteIp << ":" << remotePort << "]" << std::endl;
    }
    else
    {
        setConnected(true);
    }
    
    return ret;
}
int Socket::send(const void *buf, size_t len, int flags)
{
    if (isConnected())
    {
        return ::send(m_sock, buf, len, flags);
    }
    return -1;
}   
int Socket::recv(void *buf, size_t len, int flags)
{
    if (isConnected())
    {
        return ::recv(m_sock, buf, len, flags);
    }
    return -1;
}
int Socket::close()
{
    if (INVALID_SOCKET == m_sock && !isConnected())
    {
        return 0;
    }
    setConnected(false);
    if (INVALID_SOCKET != m_sock)
    {
        ::close(m_sock);
        // std::cout << "close socket" << std::endl;
        m_sock = INVALID_SOCKET;
    }
    return 0;
}
}