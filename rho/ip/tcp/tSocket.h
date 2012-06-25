#ifndef __tSocket_h__
#define __tSocket_h__


#include "../ebIP.h"

#include "../rho/types.h"

#include <arpa/inet.h>     //
#include <sys/socket.h>    // posix header files
#include <sys/types.h>     //

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <string>


namespace rho
{
namespace ip
{
namespace tcp
{


class tSocket
{
    public:

        /**
         * Connects to 'host' on 'port'.
         */
        tSocket(tAddr host, u16 port);

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
        int write(u8* buffer, int length);

        /**
         * Shuts down the socket's input and output streams.
         */
        void shutdown();

        /**
         * Shuts down the input stream of the socket.
         */
        void shutdownRead();

        /**
         * Shuts down the output stream of the socket.
         */
        void shutdownWrite();

        /**
         * The destructor will also call shutdown(), so don't stress about
         * doing it yourself unless you need to shutdown early for some reason.
         */
        ~tSocket();

    private:

        explicit tSocket(int fd);

        int m_fd;   // posix file descriptor

        friend class tTCPServer;

    private:

        // Disable default constructor and make sockets non-copyable.
        tSocket();
        tSocket(const tSocket&);
        tSocket& operator=(const tSocket&);
};


tSocket::tSocket(tAddr host, u16 port)
{
    m_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_fd == -1)
    {
        throw eTCPSocketCreationError("Cannot create posix tcp socket.");
    }

    struct sockaddr_in stSockAddr;
    memset(&stSockAddr, 0, sizeof(stSockAddr));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(port);
    int res = inet_pton(AF_INET, host.c_str(), &stSockAddr.sin_addr);
    if (res != 1)
    {
        printf("Cannot convert host to inet address\n");
        close(m_fd);
        throw std::exception();
    }

    if (connect(m_fd, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)) == -1)
    {
        close(m_fd);
        std::ostringstream errorStr;
        errorStr << "Host (" << host << ") unreachable, ";
        errorStr << "or host rejected connection on port (" << port << ").";
        throw eTCPSocketHostUnreachableError(errorStr.str());
    }
}

tSocket::tSocket(int fd)
{
    m_fd = fd;
}

int tSocket::read(u8* buffer, int length)
{
    return ::read(m_fd, buffer, length);
}

int tSocket::write(u8* buffer, int length)
{
    return ::write(m_fd, buffer, length);
}

void tSocket::shutdown()
{
    if (::shutdown(m_fd, SHUT_RDWR) == -1)
    {
        // "Cannot shutdown socket"
        throw std::exception();
    }
}

void tSocket::shutdownRead()
{
    if (::shutdown(m_fd, SHUT_RD) == -1)
    {
        // "Cannot shutdown socket"
        throw std::exception();
    }
}

void tSocket::shutdownWrite()
{
    if (::shutdown(m_fd, SHUT_WR) == -1)
    {
        // "Cannot shutdown socket"
        throw std::exception();
    }
}

tSocket::~tSocket()
{
    close(m_fd);
    m_fd = -1;
}


} // namespace tcp
} // namespace ip
} // namespace rho


#endif    // __tSocket_h__
