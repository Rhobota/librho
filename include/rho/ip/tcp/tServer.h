#ifndef __rho_ip_tcp_tServer_h__
#define __rho_ip_tcp_tServer_h__


#include "tSocket.h"

#include "rho/bNonCopyable.h"
#include "rho/refc.h"
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

        /**
         * Binds to localhost on port 'bindPort'.
         */
        explicit tServer(u16 bindPort);

        /**
         * Binds to 'addrGroup[0]' on port 'bindPort'.
         *
         * Note: 'addrGroup.size()' must equal one, or else
         *       an exception will be thrown.
         *
         * Hint: Construct the tAddrGroup using the constructor which takes
         *       an 'ip::tAddrGroup::nAddrGroupSpecialType'.
         */
        tServer(const tAddrGroup& addrGroup, u16 bindPort);

        /**
         * Returns the address on which the server is bound.
         */
        tAddr getBindAddress() const;

        /**
         * Returns the port on which the server is bound.
         */
        u16   getBindPort() const;

        /**
         * Blocks, waiting for a connection. When one arrives it is returned.
         */
        refc<tSocket> accept();

        ~tServer();

    private:

        void m_init(const tAddrGroup& addrGroup, u16 bindPort);
        void m_finalize();

    private:

        int   m_fd;    // posix file descriptor
        tAddr m_addr;

        static const int kServerAcceptQueueLength = 100;
};


}   // namespace tcp
}   // namespace ip
}   // namespace rho


#endif     // __rho_ip_tcp_tServer_h__
