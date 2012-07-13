#ifndef __rho_ip_tAddrGroup_h__
#define __rho_ip_tAddrGroup_h__


#include "tAddr.h"
#include "ebIP.h"

#include "rho/bNonCopyable.h"

#include <arpa/inet.h>    //
#include <sys/socket.h>   // posix headers
#include <sys/types.h>    //
#include <netdb.h>        //

#include <ostream>
#include <string>
#include <vector>


namespace rho
{
namespace ip
{


class tAddrGroup : public bNonCopyable
{
    public:

        enum nAddrGroupSpecialType
        {
            kLocalhostConnect,
            kLocalhostBind,
            kWildcardBind
        };

        tAddrGroup(nAddrGroupSpecialType type);

        tAddrGroup(std::string host);
        tAddrGroup(std::string host, bool resolve);

        ~tAddrGroup();

        int   size() const;
        tAddr operator[](int i) const;

    private:

        void m_init_helper(const char* hostStr, const char* serviceStr,
                           struct addrinfo* hints);

        void m_initLocalhostConnect();
        void m_initLocalhostBind();
        void m_initWildcardBind();
        void m_init(std::string host, bool resolve);

        void m_finalize();

    private:

        struct addrinfo* m_addrinfohead;
        std::vector<struct addrinfo*> m_valid_addrinfos;

        void m_debugprint(std::ostream& o, struct addrinfo* info);
};


}   // namespace ip
}   // namespace rho


#endif   // __rho_ip_tAddrGroup_h__