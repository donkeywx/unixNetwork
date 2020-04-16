#ifndef __DONKEY_HTTP_COMMON_H__
#define __DONKEY_HTTP_COMMON_H__

#include <string>
#include <string.h>

namespace donkey
{
enum HTTP_VERSION
{
    HTTP_10 = 0X10,
    HTTP_11 = 0X11
};
enum HTTP_PARSE_REQUEST_ERRORCODE
{
    /// 1000: invalid method
    INVALID_REQ_METHOD = 2001,
    /// 1001: invalid version
    INVALID_REQ_VERSION = 2002,
    /// 1002: invalid field
    INVALID_REQ_FIELD = 2003,
    // 
    NO_REQ_ERROR = 0
};
enum HTTP_PARSE_RESPONSE_ERRORCODE
{
    /// 1001: invalid version
    INVALID_RES_VERSION = 2001,
    /// 1002: invalid field
    INVALID_RES_FIELD = 2002,
    //
    NO_RES_ERROR = 0
};

struct CaseInsensitiveLess
{
    /**
     * @brief 忽略大小写比较字符串
     */
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

static const char xdigit_chars[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
}

#endif