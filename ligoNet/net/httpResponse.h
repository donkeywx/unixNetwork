#ifndef __DONKEY_HTTP_RESPONSE_H__
#define __DONKEY_HTTP_RESPONSE_H__

#include "nonCopyable.h"
#include <memory>

namespace donkey
{
class HttpReponse: public Noncopyable, public std::enable_shared_from_this<HttpReponse>
{
public:
    typedef std::shared_ptr<HttpReponse> Ptr;

    HttpReponse();
    ~HttpReponse();
};
}

#endif