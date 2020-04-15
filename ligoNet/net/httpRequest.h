#ifndef __DONKEY_HTTP_REQUEST_H__
#define __DONKEY_HTTP_REQUEST_H__

#include "nonCopyable.h"
#include <memory>

namespace donkey
{
class HttpRequest: public Noncopyable, public std::enable_shared_from_this<HttpRequest>
{
public:
    typedef std::shared_ptr<HttpRequest> Ptr;
    HttpRequest();
    ~HttpRequest();
private:
    
};
}

#endif