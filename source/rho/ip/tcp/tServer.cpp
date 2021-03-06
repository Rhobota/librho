#include "../_pre.h"
#include <rho/ip/tcp/tServer.h>
#include <rho/ip/ebIP.h>

#include <sstream>


namespace rho
{
namespace ip
{
namespace tcp
{


#if __linux__ || __APPLE__ || __CYGWIN__
static const int kInvalidSocket = -1;
#elif __MINGW32__
static const int kInvalidSocket = INVALID_SOCKET;
#else
#error What platform are you on!?
#endif


tServer::tServer(u16 bindPort)
    : m_fd(kInvalidSocket)
{
    tAddrGroup addrGroup(tAddrGroup::kLocalhostBind);
    m_init(addrGroup, bindPort);
}

tServer::tServer(const tAddrGroup& addrGroup, u16 bindPort)
    : m_fd(kInvalidSocket)
{
    m_init(addrGroup, bindPort);
}

void tServer::m_init(const tAddrGroup& addrGroup, u16 bindPort)
{
    m_finalize();

    if (addrGroup.size() != 1)
        throw eLogicError("A tcp server can only bind to one address.");

    m_addr = addrGroup[0];
    m_addr.setUpperProtoPort(bindPort);

    #if __linux__
    m_fd = ::socket(AF_INET6, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_TCP);
    #elif __APPLE__ || __CYGWIN__ || __MINGW32__
    m_fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    #else
    #error What platform are you on!?
    #endif
    if (m_fd == kInvalidSocket)
    {
        throw eSocketCreationError(
                std::string("Cannot create posix server socket. Error: ") +
                strerror(errno));
        // If you get an "Address family not supported by protocol" error here,
        // it's probably because the kernel doesn't have the IPv6 module enabled.
        // For Raspberry PIs, that is the default, so you'll have to enable IPv6
        // as described here:
        //     http://www.raspbian.org/RaspbianFAQ#How_do_I_enable_or_use_IPv6.3F
    }

    #if __APPLE__ || __CYGWIN__ || __MINGW32__
    #if __APPLE__ || __CYGWIN__
    if (fcntl(m_fd, F_SETFD, fcntl(m_fd, F_GETFD, 0) | FD_CLOEXEC) != 0)
    #else
    if (!SetHandleInformation((HANDLE)m_fd, HANDLE_FLAG_INHERIT, 0))
    #endif
    {
        m_finalize();
        throw eSocketCreationError("Cannot set close-on-exec on the new server socket.");
    }
    #endif

    int off = 0;
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off))
            != 0)
    {
        #if __linux__ || __APPLE__
        m_finalize();
        throw eSocketCreationError("Cannot turn off ipv6-only on server socket.");
        #else
        std::cerr << "Cannot turn off ipv6-only on server socket." << std::endl;
        #endif
    }

    int on  = 1;
    #if __linux__ || __APPLE__ || __CYGWIN__
    if (::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) != 0)
    #elif __MINGW32__
    if (::setsockopt(m_fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&on, sizeof(on)) != 0)
    #else
    #error What platform are you on!?
    #endif
    {
        m_finalize();
        throw eSocketCreationError("Cannot enable reuse-addr on server socket.");
    }

    if (::bind(m_fd, (struct sockaddr*)(m_addr.m_sockaddr), m_addr.m_sockaddrlen) != 0)
    {
        m_finalize();
        std::ostringstream o;
        o << "Cannot bind tcp server socket to port (" << bindPort << ").";
        throw eSocketBindError(o.str());
    }

    if (::listen(m_fd, kServerAcceptQueueLength) != 0)
    {
        m_finalize();
        throw eSocketBindError("Cannot put server socket into the listening state.");
    }
}

void tServer::m_finalize()
{
    if (m_fd != kInvalidSocket)
    {
        #if __linux__ || __APPLE__ || __CYGWIN__
        ::close(m_fd);
        #elif __MINGW32__
        ::closesocket(m_fd);
        #else
        #error What platform are you on!?
        #endif
        m_fd = kInvalidSocket;
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

    #if __linux__
    int fd = ::accept4(m_fd, (struct sockaddr*)&sockAddr, &returnedLen, SOCK_CLOEXEC);
    #elif __APPLE__ || __CYGWIN__ || __MINGW32__
    int fd = ::accept(m_fd, (struct sockaddr*)&sockAddr, &returnedLen);
    #else
    #error What platform are you on!?
    #endif

    if (fd == kInvalidSocket)
    {
        std::ostringstream o;
        o << "Server socket failed to accept() a new connection, error: "
          << strerror(errno);
        throw eSocketCreationError(o.str());
    }

    #if __APPLE__ || __CYGWIN__ || __MINGW32__
    #if __APPLE__ || __CYGWIN__
    if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | FD_CLOEXEC) != 0)
    #else
    if (!SetHandleInformation((HANDLE)fd, HANDLE_FLAG_INHERIT, 0))
    #endif
    {
        int err = errno;
        #if __linux__ || __APPLE__ || __CYGWIN__
        ::close(fd);
        #elif __MINGW32__
        ::closesocket(fd);
        #else
        #error What platform are you on!?
        #endif
        fd = kInvalidSocket;
        std::ostringstream o;
        o << "Cannot set close-on-exec on the new accept()ed connection, error: "
          << strerror(err);
        throw eSocketCreationError(o.str());
    }
    #endif

    if (returnedLen > sockAddrLen)
    {
        #if __linux__ || __APPLE__ || __CYGWIN__
        ::close(fd);
        #elif __MINGW32__
        ::closesocket(fd);
        #else
        #error What platform are you on!?
        #endif
        fd = kInvalidSocket;
        throw eLogicError("Something is crazy wrong.");
    }

    tAddr addr((struct sockaddr*)&sockAddr, (int)returnedLen);

    return refc<tSocket>(new tSocket(fd, addr));
}

refc<tSocket> tServer::accept(u32 timeoutMS)
{
    fd_set myfdset;
    FD_ZERO(&myfdset);
    #if __linux__ || __APPLE__ || __CYGWIN__
    FD_SET(m_fd, &myfdset);
    #elif __MINGW32__
    FD_SET((SOCKET)m_fd, &myfdset);
    #else
    #error What platform are you on!?
    #endif
    struct timeval tv;
    tv.tv_sec = (timeoutMS / 1000);
    tv.tv_usec = ((timeoutMS % 1000) * 1000);
    int selectStatus = ::select(m_fd+1, &myfdset, NULL, NULL, &tv);

    // This is the timeout case:
    if (selectStatus != 1)
        return refc<tSocket>(NULL);

    // This is the case when we found that the socket is readable!
    // This means we should be able to call accept() and have it return
    // immediately.
    return this->accept();
}


}   // namespace tcp
}   // namespace ip
}   // namespace rho
