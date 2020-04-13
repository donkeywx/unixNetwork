#include "socket.h"
#include <libgo/netio/unix/hook.h>
namespace donkey
{
Socket::Socket(Domain domain, Type type, int protocol)
    : m_sock(-1), m_domain(domain)
    , m_type(type), m_protocol(protocol)
{

}
Socket::~Socket()
{

}
}