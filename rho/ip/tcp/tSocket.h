#ifndef __tcp_tSocket_h__
#define __tcp_tSocket_h__


#include "../ebIP.h"
#include "../tAddrGroup.h"

#include "rho/bNonCopyable.h"
#include "rho/types.h"

#include <arpa/inet.h>     //
#include <sys/socket.h>    // posix header files
#include <sys/types.h>     //
#include <errno.h>         //

#include <sstream>
#include <string>


namespace rho
{
namespace ip
{
namespace tcp
{


class tSocket : public bNonCopyable
{
    public:

        /**
         * Tries to connects to the host described by 'addr'.
         * Tries to connect on 'port'.
         */
        tSocket(const tAddr& addr, u16 port);

        /**
         * Tries to connect to each host described in 'addrGroup' until
         * one connects successfully. Tries each connection on 'port'.
         */
        tSocket(const tAddrGroup& addrGroup, u16 port);

        /**
         * Tries to connect to the host described by 'hostStr'.
         * Tries to connect on 'port'.
         */
        tSocket(std::string hostStr, u16 port);

        /**
         * Closes and destroys the socket.
         */
        ~tSocket();

        /**
         * Returns the address of the other side of the socket.
         */
        const tAddr& getForeignAddress() const;

        /**
         * Returns the port of the other side of the socket.
         */
        u16 getForeignPort() const;

        /**
         * Reads up to 'length' bytes from the socket into 'buffer'.
         *
         * Returns the number of bytes read, or -1 if the stream is closed.
         */
        int read(u8* buffer, int length);

        /**
         * Writes up to 'length' bytes from the socket into 'buffer'.
         *
         * Returns the number of bytes written, or -1 if the stream is closed.
         */
        int write(const u8* buffer, int length);

        /**
         * Shuts down the socket's input and output streams.
         */
        void close();

        /**
         * Shuts down the input stream of the socket.
         */
        void closeRead();

        /**
         * Shuts down the output stream of the socket.
         */
        void closeWrite();

    private:

        tSocket(int fd, const tAddr& addr);

        void m_init(const tAddr& addr, u16 port);
        void m_init(const tAddrGroup& addrGroup, u16 port);
        void m_finalize();

        friend class tServer;

    private:

        int   m_fd;       // posix file descriptor
        tAddr m_addr;
};


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

int tSocket::read(u8* buffer, int length)
{
    return ::read(m_fd, buffer, length);
}

int tSocket::write(const u8* buffer, int length)
{
    return ::write(m_fd, buffer, length);
}

void tSocket::close()
{
    if (::shutdown(m_fd, SHUT_RDWR) == -1)
    {
        throw std::logic_error(strerror(errno));
    }
}

void tSocket::closeRead()
{
    if (::shutdown(m_fd, SHUT_RD) == -1)
    {
        throw std::logic_error(strerror(errno));
    }
}

void tSocket::closeWrite()
{
    if (::shutdown(m_fd, SHUT_WR) == -1)
    {
        throw std::logic_error(strerror(errno));
    }
}


} // namespace tcp
} // namespace ip
} // namespace rho


#endif    // __tcp_tSocket_h__
