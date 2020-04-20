#include "servlet.h"

namespace donkey
{
Servlet::Servlet(std::string name)
    : m_name(name)
{

}
Servlet::~Servlet()
{

}

int32_t Servlet::handle(HttpRequest::Ptr request
                   , HttpResponse::Ptr response
                   , HttpSession::Ptr session)
{
    response->setStatus(donkey::HttpStatus::NOT_FOUND);
    response->setHeader("Server", "sylar/1.0.0");
    response->setHeader("Content-Type", "text/html");
    std::string m_content = "<html><head><title>404 Not Found"
    "</title></head><body><center><h1>404 Not Found</h1></center>"
    "<hr><center>" + m_name + 
    "If you see this page, the nginx web server is successfully installed and working. Further configuration is required. For online documentation and support please refer to nginx.org. Commercial support is available at nginx.com.Thank you for using nginx." 
    + 
    "</center></body></html>";
    response->setBody(m_content);

    return 0;
}
}