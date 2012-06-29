#ifndef __rho_ip_tcp_tServer_h__
#define __rho_ip_tcp_tServer_h__


#include "tSocket.h"

#include "rho/bNonCopyable.h"
#include "rho/refc.h"
#include "rho/types.h"

#include <arpa/inet.h>    //
#include <sys/socket.h>   // posix headers
#include <sys/types.h>    //

#include <sstream>


namespace rho
{
namespace ip
{
namespace tcp
{


class tServer : public bNonCopyable
{
    public:

        explicit tServer(u16 bindPort);

        tServer(const tAddrGroup& addrGroup, u16 bindPort);

        ~tServer();

        refc<tSocket> accept();

    private:

        void m_init(const tAddrGroup& addrGroup, u16 bindPort);
        void m_finalize();

    private:

        int   m_fd;    // posix file descriptor
        tAddr m_addr;

        static const int kServerAcceptQueueLength = 100;
};


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
        throw std::logic_error("A tcp server can only bind to one address.");

    m_addr = addrGroup[0];
    m_addr.setUpperProtoPort(bindPort);

    m_fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (m_fd == -1)
    {
        throw eSocketCreationError("Cannot create posix socket.");
    }

    int off = 0;
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&off, sizeof(off))
            == -1)
    {
        m_finalize();
        throw std::runtime_error("Cannot set socket option which is vital.");
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
        throw std::runtime_error("Cannot put server in listening state.");
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
        throw std::runtime_error(strerror(errno));
    }

    if (returnedLen > sockAddrLen)
    {
        throw std::logic_error("Something is crazy wrong.");
    }

    tAddr addr((struct sockaddr*)&sockAddr, (int)returnedLen);

    return new tSocket(fd, addr);
}


}   // namespace tcp
}   // namespace ip
}   // namespace rho


#endif     // __rho_ip_tcp_tServer_h__
