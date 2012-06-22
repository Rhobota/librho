#ifndef __TCPServer_h__
#define __TCPServer_h__


#include "TCPSocket.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>


class TCPServer
{
    public:

        explicit TCPServer(int bindPort);

        TCPSocket accept();

        void shutdown();

        ~TCPServer();

    private:

        int fd;    // posix file descriptor

        static const int kServerAcceptQueueLength = 100;

    private:

        TCPServer();
        TCPServer(const TCPServer&);
        TCPServer& operator=(const TCPServer&);
};


TCPServer::TCPServer(int bindPort)
{
    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1)
    {
        // "Cannot create tcp socket"
        throw std::exception();
    }

    struct sockaddr_in stSockAddr;
    memset(&stSockAddr, 0, sizeof(stSockAddr));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(bindPort);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)) == -1)
    {
        // "Cannot bind socket to port %d", bindPort
        close(fd);
        throw std::exception();
    }

    if (listen(fd, kServerAcceptQueueLength) == -1)
    {
        // "Cannot put the socket into listening mode"
        close(fd);
        throw std::exception();
    }
}

TCPSocket TCPServer::accept()
{
    if (fd == -1)
    {
        // "Cannot accept (the server socket is already shutdown)"
        throw std::exception();
    }

    int socketFd = ::accept(fd, NULL, NULL);
    return TCPSocket(socketFd);
}

void TCPServer::shutdown()
{
    if (fd == -1)
    {
        // "Cannot shutdown (the server socket is already shutdown)"
        throw std::exception();
    }

    close(fd);
    fd = -1;
}

TCPServer::~TCPServer()
{
    if (fd != -1)
    {
        shutdown();
    }
}


#endif     // __TCPServer_h__
