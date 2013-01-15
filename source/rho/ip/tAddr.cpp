#include <rho/ip/tAddr.h>

#include <iostream>


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
        throw eLogicError(
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
        throw eLogicError(
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
        throw eLogicError(
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
        throw eLogicError(
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
        #if __linux__ || __APPLE__ || __CYGWIN__
        inet_ntop(m_sockaddr->sa_family, addr, addrStr, buffSize);
        #elif __MINGW32__
        DWORD lengthOfReturnedString = buffSize;
        WSAAddressToString(m_sockaddr, m_sockaddrlen, NULL, addrStr, &lengthOfReturnedString);
        #else
        #error What platform are you on!?
        #endif
        rep = addrStr;
        delete [] addrStr;
    }
    else
    {
        throw eLogicError(
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


#if __linux__ || __APPLE__ || __CYGWIN__

/*
 * On Linux, when you write() to a broken pipe or socket, the program gets
 * a SIGPIPE signal delivered to it. If not handled, that will kill the
 * program. The following code causes that signal to be ignored.
 */
static int setSigPipeHandler()
{
    signal(SIGPIPE, SIG_IGN);
    return 1;
}
const int kSigPipeIgnoreKickoff = setSigPipeHandler();

#elif __MINGW32__

/*
 * On Windows, you must call WSAStartup() before any other winsock call.
 */
static int initForWindows()
{
    WSADATA info;
    if (WSAStartup(MAKEWORD(2,2), &info) != 0)
    {
        std::cerr << "Couldn't start winsock!" << std::endl;
        std::exit(1);
    }
    return 1;
}
const int kInitForWindowsKickoff = initForWindows();

#else
#error What platform are you on!?
#endif


}   // namespace ip
}   // namespace rho
