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
         * Creates a UDP socket for receiving datagrams to this port. The socket binds to this port.
         * Next, use the receive() method to receive a UDP datagram on this port.
         */
        tSocket(u16 port);

        /**
         * Creates a UDP socket that joins this multicast group and receives datagrams to this port. The socket binds to this port.
         * Next, use the receive() method to receive a UDP datagram on this port to this multicast group.
         */
        tSocket(const tAddr& addr, u16 port);

        /**
         * Closes the socket and cleans up stuff (e.g. leaves the multicast group if applicable).
         */
        ~tSocket();

        /**
         * Sends a UDP datagram to 'dest'.
         * Note: Don't try to send a huge buffer inside a single datagram! That's not safe
         * and the huge buffer will likely be dropped by an intermediary router, especially
         * if you're sending across the Internet at large. This reference recommends sending
         * at most 512 payload bytes inside a single datagram:
         *    http://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
         */
        void send(u8* buf, i32 bufSize, const tAddr& dest);

        /**
         * Receives a UDP datagram from the bound socket.
         * Use 'maxSize' to tell this method how much space you've allocated in the 'buf' buffer.
         * This method will truncate the datagram if it cannot fit inside 'buf'. The size of the
         * datagram is returned. The source of the datagram is returned via the 'src' parameter.
         */
        i32  receive(u8* buf, i32 maxSize, tAddr& src);

    private:

        void m_openSocket();

        void m_finalize();

    private:

        int   m_fd;       // posix file descriptor
};


}  // namespace udp
}  // namespace ip
}  // namespace rho


#endif // __rho_ip_udp_tSocket_h__
