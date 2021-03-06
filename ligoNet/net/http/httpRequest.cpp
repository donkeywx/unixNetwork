#include "httpRequest.h"
#include "string.h"
#include <sstream>

namespace donkey
{
HttpRequest::HttpRequest(uint8_t version, bool close)
    :m_method(HttpMethod::GET)
    , m_path("/")
    , m_version(version)
    , m_close(close)
    , m_parserParamFlag(0)
{

}
HttpRequest::~HttpRequest()
{
    
}
std::string HttpRequest::getHeader(const std::string& key, const std::string& def) const
{
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(const std::string& key, const std::string& def)
{
    initQueryParam();
    initBodyParam();
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

std::string HttpRequest::getCookie(const std::string& key, const std::string& def)
{
    initCookies();
    auto it = m_cookies.find(key);
    return it == m_cookies.end() ? def : it->second;
}

void HttpRequest::setHeader(const std::string& key, const std::string& val)
{
    m_headers[key] = val;
}

void HttpRequest::setParam(const std::string& key, const std::string& val)
{
    m_params[key] = val;
}

void HttpRequest::setCookie(const std::string& key, const std::string& val)
{
    m_cookies[key] = val;
}

void HttpRequest::delHeader(const std::string& key)
{
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string& key)
{
    m_params.erase(key);
}

void HttpRequest::delCookie(const std::string& key)
{   
    m_cookies.erase(key);
}

bool HttpRequest::hasHeader(const std::string& key, std::string* val)
{
    auto it = m_headers.find(key);
    if(it == m_headers.end())
    {
        return false;
    }
    if(val)
    {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasParam(const std::string& key, std::string* val)
{
    initQueryParam();
    initBodyParam();
    auto it = m_params.find(key);
    if(it == m_params.end())
    {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasCookie(const std::string& key, std::string* val)
{
    initCookies();
    auto it = m_cookies.find(key);
    if(it == m_cookies.end())
    {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

void HttpRequest::init()
{
    std::string conn = getHeader("connection");
    if(!conn.empty())
    {
        if(0 == strcasecmp(conn.c_str(), "keep-alive"))
        {
            m_close = false;
        }
        else
        {
            m_close = true;
        }
    }
}
void HttpRequest::initParam()
{
    initQueryParam();
    initBodyParam();
    initCookies();
}

std::string urlDecode(const std::string& str, bool space_as_plus = true) {
    std::string* ss = nullptr;
    const char* end = str.c_str() + str.length();
    for(const char* c = str.c_str(); c < end; ++c) {
        if(*c == '+' && space_as_plus) {
            if(!ss) {
                ss = new std::string;
                ss->append(str.c_str(), c - str.c_str());
            }
            ss->append(1, ' ');
        } else if(*c == '%' && (c + 2) < end
                    && isxdigit(*(c + 1)) && isxdigit(*(c + 2))){
            if(!ss) {
                ss = new std::string;
                ss->append(str.c_str(), c - str.c_str());
            }
            ss->append(1, (char)(xdigit_chars[(int)*(c + 1)] << 4 | xdigit_chars[(int)*(c + 2)]));
            c += 2;
        } else if(ss) {
            ss->append(1, *c);
        }
    }
    if(!ss) {
        return str;
    } else {
        std::string rt = *ss;
        delete ss;
        return rt;
    }
}

std::string trim(const std::string& str, const std::string& delimit = " \t\r\n")
{
    auto begin = str.find_first_not_of(delimit);
    if(begin == std::string::npos) {
        return "";
    }
    auto end = str.find_last_not_of(delimit);
    return str.substr(begin, end - begin + 1);
}

void HttpRequest::initQueryParam()
{
    if(m_parserParamFlag & 0x1)
    {
        return;
    }

#define PARSE_PARAM(str, m, flag, trim) \
    size_t pos = 0; \
    do { \
        size_t last = pos; \
        pos = str.find('=', pos); \
        if(pos == std::string::npos) { \
            break; \
        } \
        size_t key = pos; \
        pos = str.find(flag, pos); \
        m.insert(std::make_pair(trim(str.substr(last, key - last)), \
                    urlDecode(str.substr(key + 1, pos - key - 1)))); \
        if(pos == std::string::npos) { \
            break; \
        } \
        ++pos; \
    } while(true);

    PARSE_PARAM(m_query, m_params, '&',);
    m_parserParamFlag |= 0x1;
}
void HttpRequest::initBodyParam()
{
    if(m_parserParamFlag & 0x2)
    {
        return;
    }
    std::string content_type = getHeader("content-type");
    if(strcasestr(content_type.c_str(), "application/x-www-form-urlencoded") == nullptr)
    {
        m_parserParamFlag |= 0x2;
        return;
    }
    PARSE_PARAM(m_body, m_params, '&',);
    m_parserParamFlag |= 0x2;
}


void HttpRequest::initCookies()
{
    if(m_parserParamFlag & 0x4)
    {
        return;
    }
    std::string cookie = getHeader("cookie");
    if(cookie.empty())
    {
        m_parserParamFlag |= 0x4;
        return;
    }
    PARSE_PARAM(cookie, m_cookies, ';', trim);
    m_parserParamFlag |= 0x4;
}

static const char* s_method_string[] = {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

const char* HttpMethodToString(const HttpMethod& m) {
    uint32_t idx = (uint32_t)m;
    if(idx >= (sizeof(s_method_string) / sizeof(s_method_string[0]))) {
        return "<unknown>";
    }
    return s_method_string[idx];
}

std::string HttpRequest::toString() const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

std::ostream& HttpRequest::dump(std::ostream& os) const
{
    //GET /uri HTTP/1.1
    //Host: wwww.sylar.top
    //
    //
    os << HttpMethodToString(m_method) << " "
       << m_path
       << (m_query.empty() ? "" : "?")
       << m_query
       << (m_fragment.empty() ? "" : "#")
       << m_fragment
       << " HTTP/"
       << ((uint32_t)(m_version >> 4))
       << "."
       << ((uint32_t)(m_version & 0x0F))
       << "\r\n";

    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    for(auto& i : m_headers) {
        if(strcasecmp(i.first.c_str(), "connection") == 0) {
            continue;
        }
        os << i.first << ": " << i.second << "\r\n";
    }

    if(!m_body.empty()) {
        os << "content-length: " << m_body.size() << "\r\n\r\n"
           << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& req)
{
    return req.dump(os);
}
}