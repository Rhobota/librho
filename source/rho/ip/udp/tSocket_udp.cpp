#include "../_pre.h"
#include <rho/ip/udp/tSocket.h>
#include <rho/ip/ebIP.h>


namespace rho
{
namespace ip
{
namespace udp
{


#if __linux__ || __APPLE__ || __CYGWIN__
static const int kInvalidSocket = -1;
#elif __MINGW32__
static const int kInvalidSocket = INVALID_SOCKET;
#else
#error What platform are you on!?
#endif


tSocket::tSocket()
    : m_fd(kInvalidSocket)
{
    m_openSocket();

    // TODO
}


tSocket::tSocket(u16 port)
    : m_fd(kInvalidSocket)
{
    tAddrGroup addrGroup(kWildcardBind);
    if (addrGroup.size() != 1)
        throw eLogicError("A udp socket can only bind to one address.");

    tAddr addr = addrGroup[0];
    addr.setUpperProtoPort(port);

    m_openSocket();

    if (::bind(m_fd, (struct sockaddr*)(addr.m_sockaddr), addr.m_sockaddrlen) != 0)
    {
        m_finalize();
        std::ostringstream o;
        o << "Cannot bind udp socket to port (" << port << ").";
        throw eSocketBindError(o.str());
    }
}


tSocket::tSocket(const tAddr& addr, u16 port)
    : m_fd(kInvalidSocket)
{
    m_openSocket();

    // TODO
}


tSocket::~tSocket()
{
    m_finalize();
}


void tSocket::m_openSocket()
{
    m_finalize();

    #if __linux__
    m_fd = ::socket(AF_INET6, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_UDP);
    #elif __APPLE__ || __CYGWIN__ || __MINGW32__
    m_fd = ::socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    #else
    #error What platform are you on!?
    #endif
    if (m_fd == kInvalidSocket)
    {
        throw eSocketCreationError(
                std::string("Cannot create posix udp socket. Error: ") +
                strerror(errno));
        // If you get an "Address family not supported by protocol" error here,
        // it's probably because the kernel doesn't have the IPv6 module enabled.
        // For Raspberry PIs, that is the default, so you'll have to enable IPv6
        // as described here:
        //     http://www.raspbian.org/RaspbianFAQ#How_do_I_enable_or_use_IPv6.3F
    }

    #if __APPLE__ || __CYGWIN__ || __MINGW32__
    #if __APPLE__ || __CYGWIN__
    if (::fcntl(m_fd, F_SETFD, ::fcntl(m_fd, F_GETFD, 0) | FD_CLOEXEC) != 0)
    #else
    if (!SetHandleInformation((HANDLE)m_fd, HANDLE_FLAG_INHERIT, 0))
    #endif
    {
        m_finalize();
        throw eSocketCreationError("Cannot set close-on-exec on the new socket.");
    }
    #endif
}


void tSocket::m_finalize()
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


}  // namespace udp
}  // namespace ip
}  // namespace rho
