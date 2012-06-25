#ifndef __ebIP_h__
#define __ebIP_h__


#include "rho/ebObject.h"


namespace rho
{
namespace ip
{


class ebIP : public ebObject
{
    public:

        ebIP(std::string reason)
            : ebObject(reason)
        {
        }

        ~ebIP() throw() { }
};


class eSocketCreationError : public ebIP
{
    public:

        eSocketCreationError(std::string reason)
            : ebIP(reason)
        {
        }

        ~eSocketCreationError() throw() { }
};


class eHostNotFoundError : public ebIP
{
    public:

        eHostNotFoundError(std::string reason)
            : ebIP(reason)
        {
        }

        ~eHostNotFoundError() throw() { }
};


class eHostUnreachableError : public ebIP
{
    public:

        eHostUnreachableError(std::string reason)
            : ebIP(reason)
        {
        }

        ~eHostUnreachableError() throw() { }
};


}    // namespace ip
}    // namespace rho


#endif    // __ebIP_h__
