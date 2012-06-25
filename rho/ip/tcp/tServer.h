#ifndef __tcp_tServer_h__
#define __tcp_tServer_h__


#include "tSocket.h"

#include "rho/bNonCopyable.h"
#include "rho/types.h"

#include <arpa/inet.h>    //
#include <sys/socket.h>   // posix headers
#include <sys/types.h>    //


namespace rho
{
namespace ip
{
namespace tcp
{


class tServer : public bNonCopyable
{
    public:

        explicit tServer(u16 bindPort);

        ~tServer();

        tSocket* accept();

        void shutdown();

    private:

        int m_fd;    // posix file descriptor

        static const int kServerAcceptQueueLength = 100;
};


tServer::tServer(u16 bindPort)
    : m_fd(-1)
{

}

tServer::~tServer()
{

}

tSocket* tServer::accept()
{

}

void tServer::shutdown()
{

}


}   // namespace tcp
}   // namespace ip
}   // namespace rho


#endif     // __tcp_tServer_h__
