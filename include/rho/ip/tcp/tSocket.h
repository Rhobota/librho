#ifndef __rho_ip_tcp_tSocket_h__
#define __rho_ip_tcp_tSocket_h__


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
namespace tcp
{


class tSocket :
    public bNonCopyable, public iClosable,
    public iReadable, public iWritable,
    public iFlushable
{
    public:

        /**
         * Tries to connects to the host described by 'addr'.
         * Tries to connect on 'port'.
         * You can set the timeout if you'd like, but the default
         * is 5 seconds.
         */
        tSocket(const tAddr& addr, u16 port, u32 timeoutMS=5000);

        /**
         * Tries to connect to each host described in 'addrGroup' until
         * one connects successfully. Tries each connection on 'port'.
         * You can set the timeout if you'd like, but the default
         * is 5 seconds.
         */
        tSocket(const tAddrGroup& addrGroup, u16 port, u32 timeoutMS=5000);

        /**
         * Tries to connect to the host described by 'hostStr'.
         * Tries to connect on 'port'.
         * You can set the timeout if you'd like, but the default
         * is 5 seconds.
         */
        tSocket(std::string hostStr, u16 port, u32 timeoutMS=5000);

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
         * Returns the address of the local side of the socket.
         */
        tAddr getLocalAddress() const;

        /**
         * Returns the port of the local side of the socket.
         */
        u16 getLocalPort() const;

        /**
         * Enables/disables Nagle's algorithm. (It is enabled by default.)
         */
        void setNagles(bool on);

        /**
         * Set read/write timeout. If cannot read or write in the given
         * time, give up.
         *
         * Pass 0 for infinity (which is also the default).
         */
        void setTimeout(u16 seconds);

        /**
         * See iReadable interface.
         */
        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        /**
         * See iWritable interface.
         */
        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        /**
         * Flushes the socket write stream.
         * This is not a guaranteed flush, but we'll
         * do the best we can...
         */
        bool flush();

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

        /**
         * Returns the underlying unix socket file descriptor.
         */
        int getFileDescriptor() { return m_fd; }

    private:

        tSocket(int fd, const tAddr& addr);

        void m_init(const tAddr& addr, u16 port, u32 timeoutMS);
        void m_init(const tAddrGroup& addrGroup, u16 port, u32 timeoutMS);
        void m_finalize();

        friend class tServer;

    private:

        int   m_fd;       // posix file descriptor
        tAddr m_addr;
        bool  m_readEOF;
        bool  m_writeEOF;
        bool  m_nodelay;
        sync::tMutex m_closeMux;
};


} // namespace tcp
} // namespace ip
} // namespace rho


#endif    // __rho_ip_tcp_tSocket_h__
