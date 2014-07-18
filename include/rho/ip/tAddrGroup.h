#ifndef __rho_ip_tAddrGroup_h__
#define __rho_ip_tAddrGroup_h__


#include <rho/ppcheck.h>
#include <rho/ip/ebIP.h>
#include <rho/ip/tAddr.h>
#include <rho/bNonCopyable.h>

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
                           void* hints);

        void m_initLocalhostConnect();
        void m_initLocalhostBind();
        void m_initWildcardBind();
        void m_init(std::string host, bool resolve);

        void m_finalize();

    private:

        void* m_addrinfohead;
        std::vector<void*> m_valid_addrinfos;

        void m_debugprint(std::ostream& o, void* info);
};


}   // namespace ip
}   // namespace rho


#endif   // __rho_ip_tAddrGroup_h__
