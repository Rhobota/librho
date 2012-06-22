#ifndef __tAddr_h__
#define __tAddr_h__


#include "ebInet.h"

#include "rho/bNonCopyable.h"
#include "rho/types.h"

#include <arpa/inet.h>    //
#include <sys/socket.h>   // posix headers
#include <sys/types.h>    //
#include <netdb.h>        //

#include <sstream>
#include <string>


namespace rho
{
namespace inet
{


class tAddr : public bNonCopyable
{
    public:

        enum nAddrSpecialType
        {
            kLocalhostConnect,
            kLocalhostBind,
            kWildcardBind,
        };

        tAddr(nAddrSpecialType type);

        tAddr(std::string host);

        tAddr(std::string host, bool resolve);

        ~tAddr();

        std::string toString();

    private:

        tAddr();

        void initLocalhostConnect();
        void initLocalhostBind();
        void initWildcardBind();

        void init(std::string host, bool resolve);

    private:

        struct addrinfo* m_addrinfohead;
};


tAddr::tAddr(nAddrSpecialType type)
{
    switch (type)
    {
        case kLocalhostConnect:
            initLocalhostConnect();
            break;
        case kLocalhostBind:
            initLocalhostBind();
            break;
        case kWildcardBind:
            initWildcardBind();
            break;
        default:
            throw std::runtime_error("Unknown nAddrSpecialType");
            break;
    }
}

tAddr::tAddr(std::string host)
{
    init(host, true);
}

tAddr::tAddr(std::string host, bool resolve)
{
    init(host, resolve);
}

static void debugprint(std::ostream& o, struct sockaddr* sockAddr)
{
    void* addr = NULL;
    int   buffSize = 0;
    u16   port = 0;

    if (sockAddr->sa_family == AF_INET)
    {
        struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) sockAddr;
        struct in_addr* ip4addr = &ip4sockAddr->sin_addr;
        addr = ip4addr;
        buffSize = INET_ADDRSTRLEN;
        port = ip4sockAddr->sin_port;
    }
    else if (sockAddr->sa_family == AF_INET6)
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) sockAddr;
        struct in6_addr* ip6addr = &ip6sockAddr->sin6_addr;
        addr = ip6addr;
        buffSize = INET6_ADDRSTRLEN;
        port = ip6sockAddr->sin6_port;
    }

    if (addr)
    {
        char* addrStr = new char[buffSize];
        inet_ntop(sockAddr->sa_family, addr, addrStr, buffSize);
        o << addrStr << ", port=" << port << "\n";
        delete addrStr;
    }
    else
    {
        o << "<unsupported address family>" << "\n";
    }
}

static void debugprint(std::ostream& o, struct addrinfo* info)
{
    switch (info->ai_family)
    {
        case AF_INET:
            o << "IPv4";
            break;
        case AF_INET6:
            o << "IPv6";
            break;
        default:
            o << "????";
            break;
    }
    o << "   ";
    switch (info->ai_socktype)
    {
        case SOCK_STREAM:
            o << "STREAM";
            break;
        case SOCK_DGRAM:
            o << "DGRAM ";
            break;
        default:
            o << "??????";
            break;
    }
    o << "   ";
    switch (info->ai_protocol)
    {
        case IPPROTO_TCP:
            o << "TCP";
            break;
        case IPPROTO_UDP:
            o << "UDP";
            break;
        default:
            o << "???";
            break;
    }
    o << "\n";
    debugprint(o, info->ai_addr);
    // Only thing we didn't use is info->ai_addrlen
    o << "\n";
}

void tAddr::initLocalhostConnect()
{

}

void tAddr::initLocalhostBind()
{

}

void tAddr::initWildcardBind()
{

}

void tAddr::init(std::string host, bool resolve)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;     // could be AF_INET or AF_INET6
    hints.ai_socktype = 0;             // any (SOCK_STREAM or SOCK_DGRAM)
    hints.ai_protocol = 0;             // any (IPPROTO_TCP or IPPROTO_UDP)
    hints.ai_flags   |= AI_ADDRCONFIG; // only return addresses types
                                       // which have some interface
                                       // configured for them
                                       // (set to AI_ALL to get all possible
                                       // address types, regardless of what
                                       // is configured on the local system)

    if (!resolve)
        hints.ai_flags   |= AI_NUMERICHOST;

    m_addrinfohead = NULL;
    int a;
    while (true)
    {
        a = getaddrinfo(host.c_str(), NULL, &hints, &m_addrinfohead);
        if (a != EAI_AGAIN)
            break;
    }
    if (a != 0)
    {
        throw std::runtime_error(gai_strerror(a));
    }
}

tAddr::~tAddr()
{
    freeaddrinfo(m_addrinfohead);
    m_addrinfohead = NULL;
}

std::string tAddr::toString()
{
    std::ostringstream o;
    struct addrinfo* curr;
    for (curr = m_addrinfohead; curr != NULL; curr = curr->ai_next)
    {
        debugprint(o, curr);
    }
    return o.str();
}


}   // namespace inet
}   // namespace rho


#endif   // __tAddr_h__
