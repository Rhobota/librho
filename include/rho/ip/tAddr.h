#ifndef __rho_ip_tAddr_h__
#define __rho_ip_tAddr_h__


#include <rho/types.h>

#include <rho/ip/ebIP.h>
#include <rho/ip/tcp/_pre.h>

#include <arpa/inet.h>    //
#include <sys/socket.h>   //
#include <sys/types.h>    // posix headers
#include <netdb.h>        //
#include <string.h>       //

#include <string>
#include <vector>


namespace rho
{
namespace ip
{


enum nVersion
{
    kIPv4 = 4,
    kIPv6 = 6
};


class tAddr
{
    public:

        tAddr(const tAddr& other);
        ~tAddr();

        tAddr& operator= (const tAddr& other);

        nVersion        getVersion()        const;
        std::vector<u8> getAddress()        const;

        std::string     toString()          const;

    private:

        tAddr();
        tAddr(struct sockaddr* sockAddr, int length);

        void m_init(struct sockaddr* sockAddr, int length);
        void m_finalize();

        u16  getUpperProtoPort() const;
        void setUpperProtoPort(u16 port);

    private:

        struct sockaddr* m_sockaddr;
        int m_sockaddrlen;

        friend class tAddrGroup;
        friend class tcp::tSocket;
        friend class tcp::tServer;
};


}   // namespace ip
}   // namespace rho


#endif   // __rho_ip_tAddr_h__
