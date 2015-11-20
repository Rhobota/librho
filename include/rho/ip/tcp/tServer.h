#ifndef __rho_ip_tcp_tServer_h__
#define __rho_ip_tcp_tServer_h__


#include <rho/ppcheck.h>
#include <rho/ip/tcp/tSocket.h>
#include <rho/bNonCopyable.h>
#include <rho/refc.h>
#include <rho/types.h>


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
         * Blocks waiting for a connection. When one arrives it is returned.
         * This method can block indefinitely if no connection ever arrives.
         */
        refc<tSocket> accept();

        /**
         * Same as accept() above, except only blocks for a maximum of 'timeoutMS'
         * milliseconds. A timeout occurring is not an error--null is returned.
         */
        refc<tSocket> accept(u32 timeoutMS);

        ~tServer();

    private:

        void m_init(const tAddrGroup& addrGroup, u16 bindPort);
        void m_finalize();

        void m_initClientConnection(int fd, socklen_t sockAddrLen, socklen_t returnedLen);

    private:

        int   m_fd;    // posix file descriptor
        tAddr m_addr;

        static const int kServerAcceptQueueLength = 100;
};


}   // namespace tcp
}   // namespace ip
}   // namespace rho


#endif     // __rho_ip_tcp_tServer_h__
