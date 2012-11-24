#include <rho/ip/tcp/tSocket.h>
#include <rho/ip/ebIP.h>

#include <signal.h>    // posix header
#include <sstream>


namespace rho
{
namespace ip
{
namespace tcp
{


tSocket::tSocket(const tAddr& addr, u16 port)
    : m_fd(-1)
{
    m_init(addr, port);
}

tSocket::tSocket(const tAddrGroup& addrGroup, u16 port)
    : m_fd(-1)
{
    m_init(addrGroup, port);
}

tSocket::tSocket(std::string hostStr, u16 port)
    : m_fd(-1)
{
    tAddrGroup addrGroup(hostStr);
    m_init(addrGroup, port);
}

tSocket::tSocket(int fd, const tAddr& addr)
    : m_fd(fd),
      m_addr(addr)
{
}

void tSocket::m_init(const tAddr& addr, u16 port)
{
    m_finalize();

    m_addr = addr;
    m_addr.setUpperProtoPort(port);

    if (addr.getVersion() == kIPv4)
        m_fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    else if (addr.getVersion() == kIPv6)
        m_fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (m_fd == -1)
    {
        throw eSocketCreationError("Cannot create posix tcp socket.");
    }

    if (::connect(m_fd, m_addr.m_sockaddr, m_addr.m_sockaddrlen) == -1)
    {
        m_finalize();
        std::ostringstream errorStr;
        errorStr << "Host (" << addr.toString() << ") unreachable, ";
        errorStr << "or host rejected connection on port (" << port << ").";
        throw eHostUnreachableError(errorStr.str());
    }
}

void tSocket::m_init(const tAddrGroup& addrGroup, u16 port)
{
    for (int i = 0; i < addrGroup.size(); i++)
    {
        try
        {
            m_init(addrGroup[i], port);
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
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
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

i32 tSocket::read(u8* buffer, i32 length)
{
    return ::read(m_fd, buffer, length);
}

i32 tSocket::readAll(u8* buffer, i32 length)
{
    i32 amountRead = 0;
    while (amountRead < length)
    {
        i32 n = ::read(m_fd, buffer+amountRead, length-amountRead);
        if (n <= 0)
            return (amountRead>0) ? amountRead : n;
        amountRead += n;
    }
    return amountRead;
}

i32 tSocket::write(const u8* buffer, i32 length)
{
    return ::write(m_fd, buffer, length);
}

i32 tSocket::writeAll(const u8* buffer, i32 length)
{
    i32 amountWritten = 0;
    while (amountWritten < length)
    {
        i32 n = ::write(m_fd, buffer+amountWritten, length-amountWritten);
        if (n <= 0)
            return (amountWritten>0) ? amountWritten : n;
        amountWritten += n;
    }
    return amountWritten;
}

void tSocket::close()
{
    if (::shutdown(m_fd, SHUT_RDWR) == -1)
    {
        throw eRuntimeError(strerror(errno));
    }
}

void tSocket::closeRead()
{
    if (::shutdown(m_fd, SHUT_RD) == -1)
    {
        throw eRuntimeError(strerror(errno));
    }
}

void tSocket::closeWrite()
{
    if (::shutdown(m_fd, SHUT_WR) == -1)
    {
        throw eRuntimeError(strerror(errno));
    }
}


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


} // namespace tcp
} // namespace ip
} // namespace rho
