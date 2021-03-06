#include "../_pre.h"
#include <rho/ip/tcp/tSocket.h>
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


tSocket::tSocket(const tAddr& addr, u16 port, u32 timeoutMS)
    : m_fd(kInvalidSocket), m_readEOF(false), m_writeEOF(false),
      m_internalReaderWriter(this),
      m_bufferedReadable(&m_internalReaderWriter),
      m_bufferedWritable(&m_internalReaderWriter)
{
    m_init(addr, port, timeoutMS);

    setNagles(false);
}

tSocket::tSocket(const tAddrGroup& addrGroup, u16 port, u32 timeoutMS)
    : m_fd(kInvalidSocket), m_readEOF(false), m_writeEOF(false),
      m_internalReaderWriter(this),
      m_bufferedReadable(&m_internalReaderWriter),
      m_bufferedWritable(&m_internalReaderWriter)
{
    m_init(addrGroup, port, timeoutMS);

    setNagles(false);
}

tSocket::tSocket(std::string hostStr, u16 port, u32 timeoutMS)
    : m_fd(kInvalidSocket), m_readEOF(false), m_writeEOF(false),
      m_internalReaderWriter(this),
      m_bufferedReadable(&m_internalReaderWriter),
      m_bufferedWritable(&m_internalReaderWriter)
{
    tAddrGroup addrGroup(hostStr);
    m_init(addrGroup, port, timeoutMS);

    setNagles(false);
}

tSocket::tSocket(int fd, const tAddr& addr)
    : m_fd(fd), m_addr(addr), m_readEOF(false), m_writeEOF(false),
      m_internalReaderWriter(this),
      m_bufferedReadable(&m_internalReaderWriter),
      m_bufferedWritable(&m_internalReaderWriter)
{
    setNagles(false);
}

void tSocket::m_init(const tAddr& addr, u16 port, u32 timeoutMS)
{
    m_finalize();

    m_addr = addr;
    m_addr.setUpperProtoPort(port);

    #if __linux__
    m_fd = (addr.getVersion() == kIPv4) ? ::socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_TCP)
                                        : ::socket(AF_INET6, SOCK_STREAM|SOCK_CLOEXEC, IPPROTO_TCP);
    #elif __APPLE__ || __CYGWIN__ || __MINGW32__
    m_fd = (addr.getVersion() == kIPv4) ? ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
                                        : ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    #else
    #error What platform are you on!?
    #endif
    if (m_fd == kInvalidSocket)
    {
        throw eSocketCreationError(
                std::string("Cannot create posix tcp socket. Error: ") +
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

    #if __linux__ || __APPLE__ || __CYGWIN__
    int currentFlags = ::fcntl(m_fd, F_GETFL);
    if (currentFlags < 0)
    {
        m_finalize();
        throw eSocketCreationError("Cannot get the current file status flags.");
    }
    if (::fcntl(m_fd, F_SETFL, (currentFlags | O_NONBLOCK)) != 0)
    {
        m_finalize();
        throw eSocketCreationError("Cannot set the socket to be non-blocking during the connect phase.");
    }
    #elif __MINGW32__
    unsigned long nonblockflag = 1;
    if (ioctlsocket(m_fd, FIONBIO, &nonblockflag) != 0)
    {
        m_finalize();
        throw eSocketCreationError("Cannot set the socket to be non-blocking during the connect phase.");
    }
    #else
    #error What platform are you on!?
    #endif

    int connectStatus = ::connect(m_fd, (struct sockaddr*)(m_addr.m_sockaddr), m_addr.m_sockaddrlen);
    #if __linux__ || __APPLE__ || __CYGWIN__
    if (connectStatus != -1 || errno != EINPROGRESS)
    #elif __MINGW32__
    if (connectStatus != SOCKET_ERROR || WSAGetLastError() != WSAEWOULDBLOCK)
    #else
    #error What platform are you on!?
    #endif
    {
        m_finalize();
        throw eSocketCreationError("Connect behaved weirdly. It should indicate the socket is nonblocking...");
    }

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
    int selectStatus = ::select(m_fd+1, NULL, &myfdset, NULL, &tv);

    if (selectStatus != 1)
    {
        m_finalize();
        std::ostringstream errorStr;
        errorStr << "Host (" << addr.toString() << ") unreachable, ";
        errorStr << "or host rejected connection on port (" << port << "). ";
        errorStr << "Error: " << "timeout expired on connect()";
        throw eHostUnreachableError(errorStr.str());
    }

    socklen_t argLen = sizeof(int);
    #if __linux__ || __APPLE__ || __CYGWIN__
    if (::getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (void*)(&connectStatus), &argLen) != 0)
    #elif __MINGW32__
    if (::getsockopt(m_fd, SOL_SOCKET, SO_ERROR, (char*)(&connectStatus), &argLen) != 0)
    #else
    #error What platform are you on!?
    #endif
    {
        m_finalize();
        throw eSocketCreationError("Cannot get socket error status after select.");
    }

    if (connectStatus != 0)
    {
        m_finalize();
        std::ostringstream errorStr;
        errorStr << "Host (" << addr.toString() << ") unreachable, ";
        errorStr << "or host rejected connection on port (" << port << "). ";
        errorStr << "Error: " << strerror(connectStatus);
        throw eHostUnreachableError(errorStr.str());
    }

    #if __linux__ || __APPLE__ || __CYGWIN__
    if (::fcntl(m_fd, F_SETFL, currentFlags) != 0)
    {
        m_finalize();
        throw eSocketCreationError("Cannot set the socket back to blocking mode after connect.");
    }
    #elif __MINGW32__
    nonblockflag = 0;
    if (ioctlsocket(m_fd, FIONBIO, &nonblockflag) != 0)
    {
        m_finalize();
        throw eSocketCreationError("Cannot set the socket back to blocking mode after connect.");
    }
    #else
    #error What platform are you on!?
    #endif
}

void tSocket::m_init(const tAddrGroup& addrGroup, u16 port, u32 timeoutMS)
{
    for (int i = 0; i < addrGroup.size(); i++)
    {
        try
        {
            m_init(addrGroup[i], port, timeoutMS);
            break;
        }
        catch (ebIP& e)
        {
            if (i == addrGroup.size()-1)
                throw;
        }
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

tSocket::~tSocket()
{
    m_finalize();
}

const tAddr& tSocket::getForeignAddress() const
{
    return m_addr;
}

u16 tSocket::getForeignPort() const
{
    return m_addr.getUpperProtoPort();
}

tAddr tSocket::getLocalAddress() const
{
    struct sockaddr_in6 sockAddr;
    socklen_t sockAddrLen = sizeof(sockAddr);
    socklen_t returnedLen = sockAddrLen;

    if (::getsockname(m_fd, (struct sockaddr*)&sockAddr, &returnedLen) != 0)
    {
        throw eResourceAcquisitionError(strerror(errno));
    }

    if (returnedLen > sockAddrLen)
    {
        throw eLogicError("Something is crazy wrong.");
    }

    tAddr addr((struct sockaddr*)&sockAddr, (int)returnedLen);
    return addr;
}

u16 tSocket::getLocalPort() const
{
    tAddr addr = getLocalAddress();
    return addr.getUpperProtoPort();
}

void tSocket::setNagles(bool on)
{
    int flag = on ? 0 : 1;    // Nagle's on == TCP_NODELAY off
    if(::setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int))
            != 0)
    {
        throw eRuntimeError(strerror(errno));
    }
}

void tSocket::setTimeout(u16 seconds)
{
#if __linux__ || __APPLE__ || __CYGWIN__

    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    if (::setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                sizeof(timeout)) != 0)
    {
        throw eRuntimeError(strerror(errno));
    }

    if (::setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout,
                sizeof(timeout)) != 0)
    {
        throw eRuntimeError(strerror(errno));
    }

#elif __MINGW32__

    // Windows sucks. For setsockopt() with params SO_*TIMEO, it expects
    // integer arguments which are in milliseconds. Why would they deviate
    // from the standard!!!!????

    DWORD milliseconds = seconds * 1000;

    if (::setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&milliseconds,
                sizeof(milliseconds)) != 0)
    {
        throw eRuntimeError(strerror(errno));
    }

    if (::setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&milliseconds,
                sizeof(milliseconds)) != 0)
    {
        throw eRuntimeError(strerror(errno));
    }

#else
#error What platform are you on!?
#endif
}

i32 tSocket::read(u8* buffer, i32 length)
{
    if (m_readEOF)
        return -1;
    return m_bufferedReadable.read(buffer, length);
}

i32 tSocket::readAll(u8* buffer, i32 length)
{
    if (m_readEOF)
        return -1;
    return m_bufferedReadable.readAll(buffer, length);
}

i32 tSocket::write(const u8* buffer, i32 length)
{
    if (m_writeEOF)
        return 0;
    return m_bufferedWritable.write(buffer, length);
}

i32 tSocket::writeAll(const u8* buffer, i32 length)
{
    if (m_writeEOF)
        return 0;
    return m_bufferedWritable.writeAll(buffer, length);
}

bool tSocket::flush()
{
    return m_bufferedWritable.flush();
}

void tSocket::close()
{
    flush();
    closeRead();
    closeWrite();
}

void tSocket::closeRead()
{
    sync::tAutoSync as(m_closeMux);
    if (m_readEOF)
        return;
    m_readEOF = true;
    #if __linux__ || __APPLE__ || __CYGWIN__
    if (::shutdown(m_fd, SHUT_RD) != 0)
    #elif __MINGW32__
    if (::shutdown(m_fd, SD_RECEIVE) != 0)
    #else
    #error What platform are you on!?
    #endif
    {
        // We are not throwing below, because it is causing problems
        // when this method is called from read().

        //throw eRuntimeError(strerror(errno));
    }
}

void tSocket::closeWrite()
{
    sync::tAutoSync as(m_closeMux);
    if (m_writeEOF)
        return;
    m_writeEOF = true;
    #if __linux__ || __APPLE__ || __CYGWIN__
    if (::shutdown(m_fd, SHUT_WR) != 0)
    #elif __MINGW32__
    if (::shutdown(m_fd, SD_SEND) != 0)
    #else
    #error What platform are you on!?
    #endif
    {
        //throw eRuntimeError(strerror(errno));
    }
}

i32 tSocket::m_read(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_readEOF)
        return -1;

    #if __linux__ || __APPLE__ || __CYGWIN__
    i32 val = (i32) ::read(m_fd, buffer, length);
    #elif __MINGW32__
    i32 val = (i32) ::recv(m_fd, (char*)buffer, length, 0);
    #else
    #error What platform are you on!?
    #endif

    if (val <= 0)
    {
        closeRead();
        return 0;
    }
    else
    {
        return val;
    }
}

i32 tSocket::m_write(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_writeEOF)
        return 0;

    #if __linux__ || __APPLE__ || __CYGWIN__
    i32 val = (i32) ::write(m_fd, buffer, length);
    #elif __MINGW32__
    i32 val = (i32) ::send(m_fd, (const char*)buffer, length, 0);
    #else
    #error What platform are you on!?
    #endif

    if (val <= 0)
    {
        closeWrite();
        return 0;
    }
    else
    {
        return val;
    }
}


i32 tSocket::tInternalReaderWriter::read(u8* buffer, i32 length)
{
    return m_socket->m_read(buffer, length);
}

i32 tSocket::tInternalReaderWriter::readAll(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    i32 amountRead = 0;
    while (amountRead < length)
    {
        i32 n = read(buffer+amountRead, length-amountRead);
        if (n <= 0)
            return (amountRead>0) ? amountRead : n;
        amountRead += n;
    }
    return amountRead;
}

i32 tSocket::tInternalReaderWriter::write(const u8* buffer, i32 length)
{
    return m_socket->m_write(buffer, length);
}

i32 tSocket::tInternalReaderWriter::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    i32 amountWritten = 0;
    while (amountWritten < length)
    {
        i32 n = write(buffer+amountWritten, length-amountWritten);
        if (n <= 0)
            return (amountWritten>0) ? amountWritten : n;
        amountWritten += n;
    }
    return amountWritten;
}


} // namespace tcp
} // namespace ip
} // namespace rho
