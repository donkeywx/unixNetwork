#ifndef __DONKEY_HTTP_SESSION_H__
#define __DONKEY_HTTP_SESSION_H__

#include "../tcpConn.h"
#include "httpParse.h"


namespace donkey
{
class HttpSession: public Noncopyable, public std::enable_shared_from_this<HttpSession>
{
public:
    typedef std::shared_ptr<HttpSession> Ptr;

    HttpSession(TCPConn::Ptr tcpConn);
    ~HttpSession();

    /**
     * @brief 接收HTTP请求
     */
    HttpRequest::Ptr recvRequest();

    /**
     * @brief 发送HTTP响应
     * @param[in] rsp HTTP响应
     * @return >0 发送成功
     *         =0 对方关闭
     *         <0 Socket异常
     */
    int sendResponse(HttpResponse::Ptr rsp);

    void close();
private:
    TCPConn::Ptr m_tcpConn;
};
}

#endif