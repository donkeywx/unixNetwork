#ifndef __DONKEY_SERVLET_H__
#define __DONKEY_SERVLET_H__

#include "httpSession.h"
#include <string>

namespace donkey
{
class Servlet: public Noncopyable, public std::enable_shared_from_this<Servlet>
{
public:
    typedef std::shared_ptr<Servlet> Ptr;
    Servlet(std::string name);
    ~Servlet();

    virtual int32_t handle(HttpRequest::Ptr request
                   , HttpResponse::Ptr response
                   , HttpSession::Ptr session);

private:
    std::string m_name;
};
}

#endif