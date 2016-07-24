#ifndef __rho_ip_tAddr_h__
#define __rho_ip_tAddr_h__


#include <rho/ppcheck.h>
#include <rho/ip/tcp/_pre.h>
#include <rho/ip/udp/_pre.h>
#include <rho/ip/ebIP.h>
#include <rho/types.h>

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

        std::string     toString(bool reverseLookup = false) const;

    private:

        tAddr();
        tAddr(void* sockAddr, int length);

        void m_init(void* sockAddr, int length);
        void m_finalize();

        u16  getUpperProtoPort() const;
        void setUpperProtoPort(u16 port);

    private:

        void* m_sockaddr;
        int m_sockaddrlen;

        friend class tAddrGroup;
        friend class tcp::tSocket;
        friend class tcp::tServer;
        friend class udp::tSocket;
};


}   // namespace ip
}   // namespace rho


#endif   // __rho_ip_tAddr_h__
