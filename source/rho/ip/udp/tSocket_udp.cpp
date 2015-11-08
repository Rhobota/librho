#include "../_pre.h"
#include <rho/ip/udp/tSocket.h>
#include <rho/ip/ebIP.h>

#include <sstream>
#include <iostream>


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
    m_openSocket();
    m_bind(port);
}


tSocket::tSocket(tAddr addr, u16 port)
    : m_fd(kInvalidSocket)
{
    m_openSocket();

    if (addr.getVersion() == kIPv4)
    {
        throw eRuntimeError("Cannot join IPv4 multicast group. Not supported!");

//      struct sockaddr_in* ip4sockAddr = (struct sockaddr_in*) addr.m_sockaddr;

//      struct ip_mreqn joinRequest;
//      memset(&joinRequest, 0, sizeof(joinRequest));
//      joinRequest.imr_multiaddr = ip4sockAddr->sin_addr;
//      joinRequest.imr_address.s_addr = INADDR_ANY;
//      joinRequest.imr_ifindex = 0;

//      if (::setsockopt(m_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) != 0)
//      {
//          m_finalize();
//          throw eRuntimeError("Cannot join ipv4 multicast group!");
//      }
    }

    else  // kIPv6
    {
        struct sockaddr_in6* ip6sockAddr = (struct sockaddr_in6*) addr.m_sockaddr;

        struct ipv6_mreq joinRequest;
        memset(&joinRequest, 0, sizeof(joinRequest));
        joinRequest.ipv6mr_multiaddr = ip6sockAddr->sin6_addr;
        joinRequest.ipv6mr_interface = 0;

        #if __linux__ || __APPLE__ || __CYGWIN__
        if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &joinRequest, sizeof(joinRequest)) != 0)
        #elif __MINGW32__
        if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)(&joinRequest), sizeof(joinRequest)) != 0)
        #else
        #error What platform are you on!?
        #endif
        {
            int err = errno;
            m_finalize();
            throw eRuntimeError(std::string("Cannot join ipv6 multicast group! Error: ") + strerror(err));
        }
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
        throw eSocketCreationError("Cannot enable reuse-addr on udp multicast socket.");
    }

    m_bind(port);
}


tSocket::~tSocket()
{
    m_finalize();
}


void tSocket::send(const u8* buf, i32 bufSize, tAddr dest, u16 port)
{
    if (dest.getVersion() == kIPv4)
    {
        tAddrGroup addrGroup(std::string("::ffff:") + dest.toString());
        dest = addrGroup[0];
    }

    if (bufSize <= 0)
        throw eInvalidArgument("bufSize must be positive");
    dest.setUpperProtoPort(port);
    int flags = 0;
    ssize_t ret = ::sendto(m_fd, (const char*)buf, (size_t)bufSize, flags,
                           (struct sockaddr*)(dest.m_sockaddr),
                           dest.m_sockaddrlen);
    if (ret == -1)
    {
        throw eRuntimeError(
                std::string("Cannot sendto udp socket. Error: ") +
                strerror(errno));
    }
}


void tSocket::send(const u8* buf, i32 bufSize, std::string dest, u16 port)
{
    tAddrGroup addrGroup(dest);
    if (addrGroup.size() == 0)
        throw eRuntimeError(std::string("Cannot resolve IP address of host: ") + dest);
    send(buf, bufSize, addrGroup[0], port);
}


tAddr tSocket::receive(u8* buf, i32 maxSize, i32& bufSize, u16& port)
{
    if (maxSize <= 0)
        throw eInvalidArgument("maxSize must be positive");

    struct sockaddr_in6 sockAddr;
    socklen_t sockAddrLen = sizeof(sockAddr);
    socklen_t returnedLen = sockAddrLen;

    #if __linux__ || __APPLE__ || __CYGWIN__
    int flags = MSG_TRUNC;
    #elif __MINGW32__
    int flags = 0;
    #else
    #error What platform are you on!?
    #endif

    ssize_t ret = ::recvfrom(m_fd, (char*)buf, (size_t)maxSize, flags,
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


tAddr tSocket::receive(u8* buf, i32 maxSize, i32& bufSize, u16& port, u32 timeoutMS)
{
    // Block until the socket is readable.
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
    if (selectStatus != 1)
        throw eReceiveTimeoutError("UDP receive timeout!");

    // The socket is readable... it seems... so let's read from it.
    return receive(buf, maxSize, bufSize, port);
}


void tSocket::setMulticastSendHops(int allowedHops)
{
    #if __linux__ || __APPLE__ || __CYGWIN__
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &allowedHops, sizeof(int)) != 0)
    #elif __MINGW32__
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)(&allowedHops), sizeof(int)) != 0)
    #else
    #error What platform are you on!?
    #endif
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

    int off = 0;
    if (::setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off))
            != 0)
    {
        #if __linux__ || __APPLE__
        m_finalize();
        throw eSocketCreationError("Cannot turn off ipv6-only on UDP socket.");
        #else
        std::cerr << "Cannot turn off ipv6-only on UDP socket." << std::endl;
        #endif
    }
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


void tSocket::m_bind(u16 port)
{
    tAddrGroup addrGroup(tAddrGroup::kWildcardBind);
    if (addrGroup.size() != 1)
    {
        m_finalize();
        throw eLogicError("A udp socket must bind to exactly one address.");
    }

    tAddr addr = addrGroup[0];
    addr.setUpperProtoPort(port);

    if (::bind(m_fd, (struct sockaddr*)(addr.m_sockaddr), addr.m_sockaddrlen) != 0)
    {
        m_finalize();
        std::ostringstream o;
        o << "Cannot bind udp socket to port (" << port << ").";
        throw eSocketBindError(o.str());
    }
}


}  // namespace udp
}  // namespace ip
}  // namespace rho
