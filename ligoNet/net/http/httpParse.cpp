#include  "httpParse.h"

namespace donkey
{

static uint64_t s_http_request_buffer_size = 4 * 1024;
static uint64_t s_http_request_max_body_size = 64 * 1024 * 1024;
static uint64_t s_http_response_buffer_size = 4 * 1024;
static uint64_t s_http_response_max_body_size = 64 * 1024 * 1024;

HttpMethod CharsToHttpMethod(const char* m)
{
#define XX(num, name, string) \
    if(strncmp(#string, m, strlen(#string)) == 0) { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

void on_request_method(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    HttpMethod m = CharsToHttpMethod(at);

    if(m == HttpMethod::INVALID_METHOD)
    {
        parser->setError(1000);
        return;
    }
    parser->getHttpRequest()->setMethod(m);
}

void on_request_uri(void *data, const char *at, size_t length)
{
}

void on_request_fragment(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getHttpRequest()->setFragment(std::string(at, length));
}

void on_request_path(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getHttpRequest()->setPath(std::string(at, length));
}

void on_request_query(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    parser->getHttpRequest()->setQuery(std::string(at, length));
}

void on_request_version(void *data, const char *at, size_t length)
{
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {
        parser->setError(1001);
        return;
    }
    parser->getHttpRequest()->setVersion(v);
}

void on_request_header_done(void *data, const char *at, size_t length)
{
    //HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
}

void on_request_http_field(void *data, const char *field, size_t flen
                           ,const char *value, size_t vlen) {
    HttpRequestParser* parser = static_cast<HttpRequestParser*>(data);
    if(flen == 0) {
        //parser->setError(1002);
        return;
    }
    parser->getHttpRequest()->setHeader(std::string(field, flen)
                                ,std::string(value, vlen));
}


HttpRequestParser::HttpRequestParser()
    : m_error(0)
{
    m_httpRequest.reset(new HttpRequest);
    http_parser_init(&m_parser);
    m_parser.request_method = on_request_method;
    m_parser.request_uri = on_request_uri;
    m_parser.fragment = on_request_fragment;
    m_parser.request_path = on_request_path;
    m_parser.query_string = on_request_query;
    m_parser.http_version = on_request_version;
    m_parser.header_done = on_request_header_done;
    m_parser.http_field = on_request_http_field;
    m_parser.data = this;
}
HttpRequestParser::~HttpRequestParser()
{

}

size_t HttpRequestParser::execute(char* data, size_t len)
{
    size_t offset = http_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + offset, (len - offset));
    return offset;
}

int HttpRequestParser::isFinished()
{
    return http_parser_finish(&m_parser);
}


int HttpRequestParser::hasError()
{
    return m_error || http_parser_has_error(&m_parser);
}

uint64_t HttpRequestParser::getContentLength()
{
    // return m_data->getHeaderAs<uint64_t>("content-length", 0);
    return 0;
}


uint64_t HttpRequestParser::GetHttpRequestBufferSize()
{
    return s_http_request_buffer_size;
}

uint64_t HttpRequestParser::GetHttpRequestMaxBodySize()
{
    return s_http_request_max_body_size;
}




void on_response_reason(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    parser->getHttpReponse()->setReason(std::string(at, length));
}

void on_response_status(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    HttpStatus status = (HttpStatus)(atoi(at));
    parser->getHttpReponse()->setStatus(status);
}

void on_response_chunk(void *data, const char *at, size_t length) {
}

void on_response_version(void *data, const char *at, size_t length) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    uint8_t v = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        v = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0) {
        v = 0x10;
    } else {

        parser->setError(1001);
        return;
    }

    parser->getHttpReponse()->setVersion(v);
}

void on_response_header_done(void *data, const char *at, size_t length) {
}

void on_response_last_chunk(void *data, const char *at, size_t length) {
}

void on_response_http_field(void *data, const char *field, size_t flen
                           ,const char *value, size_t vlen) {
    HttpResponseParser* parser = static_cast<HttpResponseParser*>(data);
    if(flen == 0) {

        //parser->setError(1002);
        return;
    }
    parser->getHttpReponse()->setHeader(std::string(field, flen)
                                ,std::string(value, vlen));
}

HttpResponseParser::HttpResponseParser()
    : m_error(0)
{
    m_httpReponse.reset(new HttpResponse);
    httpclient_parser_init(&m_parser);
    m_parser.reason_phrase = on_response_reason;
    m_parser.status_code = on_response_status;
    m_parser.chunk_size = on_response_chunk;
    m_parser.http_version = on_response_version;
    m_parser.header_done = on_response_header_done;
    m_parser.last_chunk = on_response_last_chunk;
    m_parser.http_field = on_response_http_field;
    m_parser.data = this;
}
HttpResponseParser::~HttpResponseParser()
{
}

size_t HttpResponseParser::execute(char* data, size_t len, bool chunck)
{
    if(chunck) {
        httpclient_parser_init(&m_parser);
    }
    size_t offset = httpclient_parser_execute(&m_parser, data, len, 0);

    memmove(data, data + offset, (len - offset));
    return offset;
}


int HttpResponseParser::isFinished()
{
    return httpclient_parser_finish(&m_parser);
}

int HttpResponseParser::hasError()
{
    return m_error || httpclient_parser_has_error(&m_parser);
}

uint64_t HttpResponseParser::getContentLength()
{
    // return m_data->getHeaderAs<uint64_t>("content-length", 0);
    return 0;
}

uint64_t HttpResponseParser::GetHttpResponseBufferSize()
{
    return s_http_response_buffer_size;
}


uint64_t HttpResponseParser::GetHttpResponseMaxBodySize()
{
    return s_http_response_max_body_size;
}

}