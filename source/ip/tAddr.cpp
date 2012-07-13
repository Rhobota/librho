#include "rho/ip/tAddr.h"
#include "rho/ip/ebIP.h"


namespace rho
{
namespace ip
{


tAddr::tAddr(const tAddr& other)
    : m_sockaddr(NULL),
      m_sockaddrlen(0)
{
    m_init(other.m_sockaddr, other.m_sockaddrlen);
}

tAddr& tAddr::operator= (const tAddr& other)
{
    m_init(other.m_sockaddr, other.m_sockaddrlen);
    return *this;
}

nVersion tAddr::getVersion() const
{
    if (m_sockaddr->sa_family == AF_INET)
        return kIPv4;
    else if (m_sockaddr->sa_family == AF_INET6)
        return kIPv6;
    else
        throw std::logic_error(
                "Internal IP address structure has an unknown version.");
}

std::vector<u8> tAddr::getAddress() const
{
    std::vector<u8> rep;

    if (m_sockaddr->sa_family == AF_INET)
    {
        struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) m_sockaddr;
        struct in_addr* ip4addr = &(ip4sockAddr->sin_addr);
        u32 ipVal = ip4addr->s_addr;
        rep.push_back((ipVal >> 0) & 0xFF);
        rep.push_back((ipVal >> 8) & 0xFF);
        rep.push_back((ipVal >> 16) & 0xFF);
        rep.push_back((ipVal >> 24) & 0xFF);
    }
    else if (m_sockaddr->sa_family == AF_INET6)
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) m_sockaddr;
        struct in6_addr* ip6addr = &(ip6sockAddr->sin6_addr);
        u8* ipVal = ip6addr->s6_addr;
        for (int i = 0; i < 16; i++)
            rep.push_back(ipVal[i]);
    }
    else
    {
        throw std::logic_error(
                "Internal IP address structure has an unknown version.");
    }

    return rep;
}

u16 tAddr::getUpperProtoPort() const
{
    u16 port = 0;

    if (m_sockaddr->sa_family == AF_INET)
    {
        struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) m_sockaddr;
        port = ntohs(ip4sockAddr->sin_port);
    }
    else if (m_sockaddr->sa_family == AF_INET6)
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) m_sockaddr;
        port = ntohs(ip6sockAddr->sin6_port);
    }
    else
    {
        throw std::logic_error(
                "Internal IP address structure has an unknown version.");
    }

    return port;
}

void tAddr::setUpperProtoPort(u16 port)
{
    if (m_sockaddr->sa_family == AF_INET)
    {
        struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) m_sockaddr;
        ip4sockAddr->sin_port = htons(port);
    }
    else if (m_sockaddr->sa_family == AF_INET6)
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) m_sockaddr;
        ip6sockAddr->sin6_port = htons(port);
    }
    else
    {
        throw std::logic_error(
                "Internal IP address structure has an unknown version.");
    }
}

std::string tAddr::toString() const
{
    void* addr = NULL;
    int   buffSize = 0;

    std::string rep;

    if (m_sockaddr->sa_family == AF_INET)
    {
        struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) m_sockaddr;
        struct in_addr* ip4addr = &(ip4sockAddr->sin_addr);
        addr = ip4addr;
        buffSize = INET_ADDRSTRLEN;
    }
    else if (m_sockaddr->sa_family == AF_INET6)
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) m_sockaddr;
        struct in6_addr* ip6addr = &(ip6sockAddr->sin6_addr);
        addr = ip6addr;
        buffSize = INET6_ADDRSTRLEN;
    }

    if (addr)
    {
        char* addrStr = new char[buffSize];
        inet_ntop(m_sockaddr->sa_family, addr, addrStr, buffSize);
        rep = addrStr;
        delete addrStr;
    }
    else
    {
        throw std::logic_error(
                "Internal IP address structure has an unknown version.");
    }

    return rep;
}

tAddr::tAddr()
    : m_sockaddr(NULL),
      m_sockaddrlen(0)
{
}

tAddr::tAddr(struct sockaddr* sockAddr, int length)
    : m_sockaddr(NULL),
      m_sockaddrlen(0)
{
    m_init(sockAddr, length);
}

void tAddr::m_init(struct sockaddr* sockAddr, int length)
{
    if (m_sockaddr)
        m_finalize();

    m_sockaddr = (struct sockaddr*) malloc(length);
    m_sockaddrlen = length;
    memcpy(m_sockaddr, sockAddr, length);
}

void tAddr::m_finalize()
{
    if (m_sockaddr)
    {
        free(m_sockaddr);
        m_sockaddr = NULL;
        m_sockaddrlen = 0;
    }
}

tAddr::~tAddr()
{
    m_finalize();
}


}   // namespace ip
}   // namespace rho