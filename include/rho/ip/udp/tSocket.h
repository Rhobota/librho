#ifndef __rho_ip_udp_tSocket_h__
#define __rho_ip_udp_tSocket_h__


#include <rho/ppcheck.h>
#include <rho/ip/tAddrGroup.h>
#include <rho/bNonCopyable.h>
#include <rho/iClosable.h>
#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/types.h>
#include <rho/sync/tMutex.h>
#include <rho/sync/tAutoSync.h>

#include <string>


namespace rho
{
namespace ip
{
namespace udp
{


class tSocket : public bNonCopyable
{
    public:

        /**
         * Creates a UDP socket for sending datagrams.
         * Next, use the send() method to send a UDP datagram.
         */
        tSocket();

        /**
         * Creates a UDP socket for receiving datagrams to this port.
         * Next, use the receive() method to receive a UDP datagram on this port.
         */
        tSocket(u16 port);

        /**
         * Creates a UDP socket that joins this multicast group and receives datagrams to this port.
         * Next, use the receive() method to receive a UDP datagram on this port to this multicast group.
         */
        tSocket(const tAddr& addr, u16 port);

        /**
         * Closes the socket and cleans up stuff.
         */
        ~tSocket();

    private:

        int   m_fd;       // posix file descriptor
};


}  // namespace udp
}  // namespace ip
}  // namespace rho


#endif // __rho_ip_udp_tSocket_h__
