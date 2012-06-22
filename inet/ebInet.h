#ifndef __ebInet_h__
#define __ebInet_h__


#include "rho/ebObject.h"


namespace rho
{
namespace inet
{


class ebInet : public ebObject
{
    public:

        ebInet(std::string reason)
            : ebObject(reason)
        {
        }

        ~ebInet() throw() { }
};


class eTCPSocketCreationError : public ebInet
{
    public:

        eTCPSocketCreationError(std::string reason)
            : ebInet(reason)
        {
        }

        ~eTCPSocketCreationError() throw() { }
};


class eTCPSocketUnrecognizedHostStringError : public ebInet
{
    public:

        eTCPSocketUnrecognizedHostStringError(std::string reason)
            : ebInet(reason)
        {
        }

        ~eTCPSocketUnrecognizedHostStringError() throw() { }
};


class eTCPSocketHostNotFoundError : public ebInet
{
    public:

        eTCPSocketHostNotFoundError(std::string reason)
            : ebInet(reason)
        {
        }

        ~eTCPSocketHostNotFoundError() throw() { }
};


class eTCPSocketHostUnreachableError : public ebInet
{
    public:

        eTCPSocketHostUnreachableError(std::string reason)
            : ebInet(reason)
        {
        }

        ~eTCPSocketHostUnreachableError() throw() { }
};


}    // namespace rho
}    // namespace inet


#endif    // __ebInet_h__
