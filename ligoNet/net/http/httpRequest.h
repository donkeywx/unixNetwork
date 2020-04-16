#ifndef __DONKEY_HTTP_REQUEST_H__
#define __DONKEY_HTTP_REQUEST_H__

#include "../nonCopyable.h"
#include "httpCommon.h"
#include <memory>
#include <map>

namespace donkey
{

#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  HEAD,        HEAD)         \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  /* pathological */                \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  /* WebDAV */                      \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  XX(16, BIND,        BIND)         \
  XX(17, REBIND,      REBIND)       \
  XX(18, UNBIND,      UNBIND)       \
  XX(19, ACL,         ACL)          \
  /* subversion */                  \
  XX(20, REPORT,      REPORT)       \
  XX(21, MKACTIVITY,  MKACTIVITY)   \
  XX(22, CHECKOUT,    CHECKOUT)     \
  XX(23, MERGE,       MERGE)        \
  /* upnp */                        \
  XX(24, MSEARCH,     M-SEARCH)     \
  XX(25, NOTIFY,      NOTIFY)       \
  XX(26, SUBSCRIBE,   SUBSCRIBE)    \
  XX(27, UNSUBSCRIBE, UNSUBSCRIBE)  \
  /* RFC-5789 */                    \
  XX(28, PATCH,       PATCH)        \
  XX(29, PURGE,       PURGE)        \
  /* CalDAV */                      \
  XX(30, MKCALENDAR,  MKCALENDAR)   \
  /* RFC-2068, section 19.6.1.2 */  \
  XX(31, LINK,        LINK)         \
  XX(32, UNLINK,      UNLINK)       \
  /* icecast */                     \
  XX(33, SOURCE,      SOURCE)       \

enum class HttpMethod
{
#define XX(num, name, string) name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    INVALID_METHOD
};



class HttpRequest: public Noncopyable, public std::enable_shared_from_this<HttpRequest>
{
public:
    typedef std::shared_ptr<HttpRequest> Ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

    HttpRequest(uint8_t version = HTTP_VERSION::HTTP_11, bool close = true);
    ~HttpRequest();


    HttpMethod getMethod() const { return m_method;}

    const std::string& getPath() const { return m_path;}

    const std::string& getQuery() const { return m_query;}

    const MapType& getParams() const { return m_params;}

    uint8_t getVersion() const { return m_version;}

    const std::string& getFragment() const { return m_fragment;}

    const MapType& getHeaders() const { return m_headers;}

    const std::string& getBody() const { return m_body;}

    const MapType& getCookies() const { return m_cookies;}

    std::string getHeader(const std::string& key, const std::string& def = "") const;

    std::string getParam(const std::string& key, const std::string& def = "");

    std::string getCookie(const std::string& key, const std::string& def = "");


    void setMethod(HttpMethod v) { m_method = v;}

    void setPath(const std::string& v) { m_path = v;}

    void setQuery(const std::string& v) { m_query = v;}

    void setParams(const MapType& v) { m_params = v;}

    void setVersion(uint8_t v) { m_version = v;}

    void setFragment(const std::string& v) { m_fragment = v;}

    void setHeaders(const MapType& v) { m_headers = v;}

    void setBody(const std::string& v) { m_body = v;}

    void setCookies(const MapType& v) { m_cookies = v;}

    void setHeader(const std::string& key, const std::string& val);

    void setParam(const std::string& key, const std::string& val);

    void setCookie(const std::string& key, const std::string& val);

    bool isClose() const { return m_close;}

    void setClose(bool v) { m_close = v;}


    void delHeader(const std::string& key);

    void delParam(const std::string& key);

    void delCookie(const std::string& key);

    bool hasHeader(const std::string& key, std::string* val = nullptr);

    bool hasParam(const std::string& key, std::string* val = nullptr);

    bool hasCookie(const std::string& key, std::string* val = nullptr);


    void init();
    void initParam();
    void initQueryParam();
    void initBodyParam();
    void initCookies();


    std::ostream& dump(std::ostream& os) const;

    std::string toString() const;
private:
    /// HTTP方法
    HttpMethod m_method;
    /// 请求路径
    std::string m_path;
    /// 请求参数
    std::string m_query;
    /// 请求参数MAP
    MapType m_params;
    /// 请求fragment
    std::string m_fragment;
    /// HTTP版本
    uint8_t m_version;
    /// 请求头部MAP
    MapType m_headers;
    /// 请求消息体
    std::string m_body;
    /// 请求Cookie MAP
    MapType m_cookies;

    /// 是否自动关闭
    bool m_close;
    uint8_t m_parserParamFlag;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& req);
}

#endif