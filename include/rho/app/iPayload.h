#ifndef __rho_app_iPayload_h__
#define __rho_app_iPayload_h__


#include <map>
#include <set>
#include <string>
#include <vector>


namespace rho
{
namespace app
{


class iPayload
{
    public:

        /**
         * iPayload is only used as a polymorphic type for the many
         * types of payloads. It has no interface of its own.
         */

        virtual ~iPayload() { }
};


class tStringPayload : public iPayload
{
    public:

        tStringPayload(std::string str)
            : m_str(str) { }

        std::string getString() const        { return m_str; }

    private:

        std::string m_str;
};


template <class T>
class tVectorPayload : public iPayload
{
    public:

        tVectorPayload(std::vector<T> vect)
            : m_vect(vect) { }

        std::vector<T> getVector() const     { return m_vect; }

    private:

        std::vector<T> m_vect;
};


template <class T>
class tSetPayload : public iPayload
{
    public:

        tSetPayload(std::set<T> s)
            : m_s(s) { }

        std::set<T> getSet() const            { return m_s; }

    private:

        std::set<T> m_s;
};


template <class T, class U>
class tMapPayload : public iPayload
{
    public:

        tMapPayload(std::map<T,U> m)
            : m_m(m) { }

        std::map<T,U> getMap() const         { return m_m; }

    private:

        std::map<T,U> m_m;
};


}    // namespace app
}    // namespace rho


#endif    // __rho_app_iPayload_h__
