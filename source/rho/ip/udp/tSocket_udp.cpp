#include "../_pre.h"
#include <rho/ip/udp/tSocket.h>
#include <rho/ip/ebIP.h>

#include <sstream>


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
}


tSocket::tSocket(u16 port)
    : m_fd(kInvalidSocket)
{
    tAddrGroup addrGroup(tAddrGroup::kWildcardBind);
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


tSocket::tSocket(tAddr addr, u16 port)
    : m_fd(kInvalidSocket)
{
    m_openSocket();

    addr.setUpperProtoPort(port);

    if (addr.getVersion() == kIPv4)
    {
        struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) addr.m_sockaddr;

        struct ip_mreqn joinRequest;
        memset(&joinRequest, 0, sizeof(joinRequest));
        joinRequest.imr_multiaddr = ip4sockAddr->sin_addr;
        joinRequest.imr_address.s_addr = INADDR_ANY;
        joinRequest.imr_ifindex = 0;

        if (::setsockopt(m_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) != 0)
        {
            m_finalize();
            throw eRuntimeError("Cannot join ipv4 multicast group!");
        }
    }

    else
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) addr.m_sockaddr;

        struct ipv6_mreq joinRequest;
        memset(&joinRequest, 0, sizeof(joinRequest));
        joinRequest.ipv6mr_multiaddr = ip6sockAddr->sin6_addr;
        joinRequest.ipv6mr_interface = 0;

        if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) != 0)
        {
            m_finalize();
            throw eRuntimeError("Cannot join ipv6 multicast group!");
        }
    }
}


tSocket::~tSocket()
{
    m_finalize();
}


void tSocket::send(const u8* buf, i32 bufSize, tAddr dest, u16 port)
{
    if (bufSize <= 0)
        throw eInvalidArgument("bufSize must be positive");
    dest.setUpperProtoPort(port);
    int flags = 0;
    ssize_t ret = ::sendto(m_fd, buf, (size_t)bufSize, flags,
                           (struct sockaddr*)(dest.m_sockaddr),
                           dest.m_sockaddrlen);
    if (ret == -1)
    {
        throw eRuntimeError(
                std::string("Cannot sendto udp socket. Error: ") +
                strerror(errno));
    }
}


tAddr tSocket::receive(u8* buf, i32 maxSize, i32& bufSize, u16& port)
{
    if (maxSize <= 0)
        throw eInvalidArgument("maxSize must be positive");

    struct sockaddr_in6 sockAddr;
    socklen_t sockAddrLen = sizeof(sockAddr);
    socklen_t returnedLen = sockAddrLen;

    int flags = MSG_TRUNC;

    ssize_t ret = ::recvfrom(m_fd, buf, (size_t)maxSize, flags,
                             (struct sockaddr*)&sockAddr,
                             &returnedLen);

    if (ret == -1)
    {
        throw eRuntimeError(
                std::string("Cannot recvfrom udp socket. Error: ") +
                strerror(errno));
    }

    if (returnedLen > sockAddrLen)
    {
        throw eLogicError("Something is crazy wrong (2).");
    }

    bufSize = (i32) ret;

    tAddr addr((struct sockaddr*)&sockAddr, (int)returnedLen);
    port = addr.getUpperProtoPort();
    return addr;
}


void tSocket::setMulticastSendHops(int allowedHops)
{
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &allowedHops, sizeof(int)) != 0)
    {
        throw eRuntimeError("Cannot setsockopt() with IPV6_MULTICAST_HOPS on this socket.");
    }
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
