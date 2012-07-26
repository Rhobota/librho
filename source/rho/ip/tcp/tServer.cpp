#include "rho/ip/tcp/tServer.h"

#include "rho/ip/ebIP.h"

#include <sstream>


namespace rho
{
namespace ip
{
namespace tcp
{


tServer::tServer(u16 bindPort)
    : m_fd(-1)
{
    tAddrGroup addrGroup(tAddrGroup::kLocalhostBind);
    m_init(addrGroup, bindPort);
}

tServer::tServer(const tAddrGroup& addrGroup, u16 bindPort)
    : m_fd(-1)
{
    m_init(addrGroup, bindPort);
}

void tServer::m_init(const tAddrGroup& addrGroup, u16 bindPort)
{
    if (addrGroup.size() != 1)
        throw eLogicError("A tcp server can only bind to one address.");

    m_addr = addrGroup[0];
    m_addr.setUpperProtoPort(bindPort);

    m_fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (m_fd == -1)
    {
        throw eSocketCreationError("Cannot create posix socket.");
    }

    int off = 0;
    int on  = 1;
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&off, sizeof(off))
            == -1)
    {
        m_finalize();
        throw eRuntimeError("Cannot set server to ipv6-only.");
    }

    if (::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on))
            == -1)
    {
        m_finalize();
        throw eRuntimeError("Cannot enable reuse-addr on server.");
    }

    if (::bind(m_fd, m_addr.m_sockaddr, m_addr.m_sockaddrlen) == -1)
    {
        m_finalize();
        std::ostringstream o;
        o << "Cannot bind tcp server to port (" << bindPort << ").";
        throw eSocketBindError(o.str());
    }

    if (::listen(m_fd, kServerAcceptQueueLength) == -1)
    {
        m_finalize();
        throw eRuntimeError("Cannot put server in listening state.");
    }
}

void tServer::m_finalize()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

tAddr tServer::getBindAddress() const
{
    return m_addr;
}

u16 tServer::getBindPort() const
{
    return m_addr.getUpperProtoPort();
}

tServer::~tServer()
{
    m_finalize();
}

refc<tSocket> tServer::accept()
{
    struct sockaddr_in6 sockAddr;
    socklen_t sockAddrLen = sizeof(sockAddr);
    socklen_t returnedLen = sockAddrLen;

    int fd = ::accept(m_fd, (struct sockaddr*)&sockAddr, &returnedLen);

    if (fd == -1)
    {
        throw eResourceAcquisitionError(strerror(errno));
    }

    if (returnedLen > sockAddrLen)
    {
        throw eLogicError("Something is crazy wrong.");
    }

    tAddr addr((struct sockaddr*)&sockAddr, (int)returnedLen);

    return new tSocket(fd, addr);
}


}   // namespace tcp
}   // namespace ip
}   // namespace rho
