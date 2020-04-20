#include "httpSession.h"
#include <sstream>
#include <iostream>

namespace donkey
{

HttpSession::HttpSession(TCPConn::Ptr tcpConn)
    : m_tcpConn(tcpConn)
{

}

HttpSession::~HttpSession()
{

}

HttpRequest::Ptr HttpSession::recvRequest()
{
    HttpRequestParser::ptr parser(new HttpRequestParser);
    uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();

    std::shared_ptr<char> buffer(new char[buff_size], 
        [](char* ptr)
        {
            delete[] ptr;
        }
    );

    char* data = buffer.get();
    int offset = 0;
    // std::cout << "in recv" << std::endl;
    do
    {
        int len = m_tcpConn->read(data + offset, buff_size - offset);
        if(len <= 0)
        {
            m_tcpConn->close();
            return nullptr;
        }

        len += offset;
        size_t nparse = parser->execute(data, len);
        if(parser->hasError())
        {
            m_tcpConn->close();
            return nullptr;
        }
        offset = len - nparse;
        if(offset == (int)buff_size)
        {
            m_tcpConn->close();
            return nullptr;
        }
        if(parser->isFinished())
        {
            break;
        }
    } while(true);

    // std::cout << "end while in recv" << std::endl;
    int64_t length = parser->getContentLength();
    if(length > 0)
    {
        std::string body;
        body.resize(length);

        int len = 0;
        if(length >= offset)
        {
            memcpy(&body[0], data, offset);
            len = offset;
        }
        else
        {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        if(length > 0)
        {
            if(m_tcpConn->readFixSize(&body[len], length) <= 0)
            {
                m_tcpConn->close();
                return nullptr;
            }
        }
        parser->getHttpRequest()->setBody(body);
    }

    parser->getHttpRequest()->init();
    // std::cout << "end in recv" << std::endl;
    return parser->getHttpRequest();
}

int HttpSession::sendResponse(HttpResponse::Ptr rsp)
{
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return m_tcpConn->writeFixSize(data.c_str(), data.size());
}

void HttpSession::close()
{
    if (m_tcpConn)
    {
        m_tcpConn->close();
    }
}
}