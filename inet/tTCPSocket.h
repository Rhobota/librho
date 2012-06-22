#ifndef __tTCPSocket_h__
#define __tTCPSocket_h__


#include "ebInet.h"

#include "rho/types.h"

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
namespace inet
{


class tTCPSocket
{
    public:

        /**
         * Connects to 'host' on 'port'.
         *
         * 'host' can be an ipv4 or ipv6 address in dot-notation, or
         * it can be a domain name.
         */
        tTCPSocket(std::string host, u16 port);

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
        ~tTCPSocket();

    private:

        explicit tTCPSocket(int fd);

        int m_fd;   // posix file descriptor

        friend class tTCPServer;

    private:

        // Disable default constructor and make sockets non-copyable.
        tTCPSocket();
        tTCPSocket(const tTCPSocket&);
        tTCPSocket& operator=(const tTCPSocket&);
};


tTCPSocket::tTCPSocket(std::string host, u16 port)
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

tTCPSocket::tTCPSocket(int fd)
{
    m_fd = fd;
}

int tTCPSocket::read(u8* buffer, int length)
{
    return ::read(m_fd, buffer, length);
}

int tTCPSocket::write(u8* buffer, int length)
{
    return ::write(m_fd, buffer, length);
}

void tTCPSocket::shutdown()
{
    if (::shutdown(m_fd, SHUT_RDWR) == -1)
    {
        // "Cannot shutdown socket"
        throw std::exception();
    }
}

void tTCPSocket::shutdownRead()
{
    if (::shutdown(m_fd, SHUT_RD) == -1)
    {
        // "Cannot shutdown socket"
        throw std::exception();
    }
}

void tTCPSocket::shutdownWrite()
{
    if (::shutdown(m_fd, SHUT_WR) == -1)
    {
        // "Cannot shutdown socket"
        throw std::exception();
    }
}

tTCPSocket::~tTCPSocket()
{
    close(m_fd);
    m_fd = -1;
}


} // namespace inet
} // namespace rho


#endif    // __tTCPSocket_h__
