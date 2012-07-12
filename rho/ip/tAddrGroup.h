#ifndef __rho_ip_tAddrGroup_h__
#define __rho_ip_tAddrGroup_h__


#include "ebIP.h"
#include "tAddr.h"

#include "rho/bNonCopyable.h"
#include "rho/types.h"

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
            kWildcardBind,
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


void tAddrGroup::m_debugprint(std::ostream& o, struct addrinfo* info)
{
    switch (info->ai_family)
    {
        case AF_INET:
            o << "IPv4";
            break;
        case AF_INET6:
            o << "IPv6";
            break;
        default:
            o << "????";
            break;
    }
    o << "   ";
    switch (info->ai_socktype)
    {
        case SOCK_STREAM:
            o << "STREAM";
            break;
        case SOCK_DGRAM:
            o << "DGRAM ";
            break;
        default:
            o << "??????";
            break;
    }
    o << "   ";
    switch (info->ai_protocol)
    {
        case IPPROTO_TCP:
            o << "TCP";
            break;
        case IPPROTO_UDP:
            o << "UDP";
            break;
        default:
            o << "???";
            break;
    }
    o << "\n";
    o << tAddr(info->ai_addr, info->ai_addrlen).toString();
    o << "\n";
}

tAddrGroup::tAddrGroup(nAddrGroupSpecialType type)
    : m_addrinfohead(NULL)
{
    switch (type)
    {
        case kLocalhostConnect:
            m_initLocalhostConnect();
            break;
        case kLocalhostBind:
            m_initLocalhostBind();
            break;
        case kWildcardBind:
            m_initWildcardBind();
            break;
        default:
            throw std::logic_error("Unknown nAddrGroupSpecialType");
            break;
    }
}

tAddrGroup::tAddrGroup(std::string host)
    : m_addrinfohead(NULL)
{
    m_init(host, true);
}

tAddrGroup::tAddrGroup(std::string host, bool resolve)
    : m_addrinfohead(NULL)
{
    m_init(host, resolve);
}

void tAddrGroup::m_init_helper(const char* hostStr, const char* serviceStr,
                               struct addrinfo* hints)
{
    int a;
    for (int i = 0; i < 10; i++)
    {
        a = getaddrinfo(hostStr, serviceStr, hints, &m_addrinfohead);
        if (a != EAI_AGAIN)
            break;
    }
    if (a != 0)
    {
        if (a == EAI_NONAME || a == EAI_NODATA)
            throw eHostNotFoundError("Cannot resolve host to address.");
        else
            throw std::logic_error(gai_strerror(a));
    }

    struct addrinfo* curr = NULL;
    for (curr = m_addrinfohead; curr != NULL; curr = curr->ai_next)
    {
        if (curr->ai_family == AF_INET || curr->ai_family == AF_INET6)
            m_valid_addrinfos.push_back(curr);
    }
}

void tAddrGroup::m_initLocalhostConnect()
{
    m_init("::1", false);
}

void tAddrGroup::m_initLocalhostBind()
{
    if (m_addrinfohead)
        m_finalize();

    struct addrinfo hints;            // see `man getaddrinfo'
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;  // This code assumes that if SOCK_STREAM
    hints.ai_protocol = IPPROTO_TCP;  // and IPPROTO_TCP work then UDP
                                      // will work as well.
                                      // Specifying TCP here prevents duplicate
                                      // addresses from being returned by
                                      // getaddrinfo().
    hints.ai_flags   |= AI_ALL;

    m_init_helper(NULL, "80", &hints);   // the port is bogus; we just need
                                         // something here that is not NULL.
}

void tAddrGroup::m_initWildcardBind()
{
    if (m_addrinfohead)
        m_finalize();

    struct addrinfo hints;            // see `man getaddrinfo'
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;  // This code assumes that if SOCK_STREAM
    hints.ai_protocol = IPPROTO_TCP;  // and IPPROTO_TCP work then UDP
                                      // will work as well.
                                      // Specifying TCP here prevents duplicate
                                      // addresses from being returned by
                                      // getaddrinfo().
    hints.ai_flags   |= AI_ALL;
    hints.ai_flags   |= AI_PASSIVE;

    m_init_helper(NULL, "80", &hints);   // the port is bogus; we just need
                                         // something here that is not NULL.
}

void tAddrGroup::m_init(std::string host, bool resolve)
{
    if (m_addrinfohead)
        m_finalize();

    struct addrinfo hints;            // see `man getaddrinfo'
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  // This code assumes that if SOCK_STREAM
    hints.ai_protocol = IPPROTO_TCP;  // and IPPROTO_TCP work then UDP
                                      // will work as well.
                                      // Specifying TCP here prevents duplicate
                                      // addresses from being returned by
                                      // getaddrinfo().
    hints.ai_flags   |= AI_ALL;

    if (!resolve)
        hints.ai_flags   |= AI_NUMERICHOST;

    m_init_helper(host.c_str(), NULL, &hints);
}

void tAddrGroup::m_finalize()
{
    if (m_addrinfohead)
    {
        freeaddrinfo(m_addrinfohead);
        m_addrinfohead = NULL;
        m_valid_addrinfos.clear();
    }
}

tAddrGroup::~tAddrGroup()
{
    m_finalize();
}

int tAddrGroup::size() const
{
    return m_valid_addrinfos.size();
}

tAddr tAddrGroup::operator[](int i) const
{
    if (i < 0 || i >= size())
        throw std::logic_error("No address with that index in group.");

    struct addrinfo* curr = m_valid_addrinfos[i];
    return tAddr(curr->ai_addr, curr->ai_addrlen);
}


}   // namespace ip
}   // namespace rho


#endif   // __rho_ip_tAddrGroup_h__
