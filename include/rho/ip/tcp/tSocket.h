#ifndef __rho_ip_tcp_tSocket_h__
#define __rho_ip_tcp_tSocket_h__


#include <rho/bNonCopyable.h>
#include <rho/iClosable.h>
#include <rho/iInputStream.h>
#include <rho/iOutputStream.h>
#include <rho/types.h>

#include <rho/ip/tAddrGroup.h>

#include <arpa/inet.h>     //
#include <sys/socket.h>    // posix header files
#include <sys/types.h>     //
#include <errno.h>         //

#include <string>


namespace rho
{
namespace ip
{
namespace tcp
{


class tSocket :
    public bNonCopyable, public iClosable,
    public iInputStream, public iOutputStream
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
         * Returns the number of bytes read, or 0 when eof is reached,
         * or -1 if the stream is closed.
         *
         * The 'all' version will read exactly 'length' bytes (unless
         * eof is reached or the stream is closed).
         */
        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        /**
         * Writes up to 'length' bytes from the socket into 'buffer'.
         *
         * Returns the number of bytes written, or -1 if the stream is closed.
         *
         * The 'all' version will write exactly 'length' bytes (unless
         * eof is reached or the stream is closed).
         */
        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

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


} // namespace tcp
} // namespace ip
} // namespace rho


#endif    // __rho_ip_tcp_tSocket_h__
