#ifndef __rho_eRho_h__
#define __rho_eRho_h__


#include <rho/ebObject.h>


namespace rho
{


class eLogicError : public ebObject
{
    public:

        eLogicError(std::string reason) : ebObject(reason) { }

        ~eLogicError() throw() { }
};


class eRuntimeError : public ebObject
{
    public:

        eRuntimeError(std::string reason) : ebObject(reason) { }

        ~eRuntimeError() throw() { }
};


class eResourceAcquisitionError : public eRuntimeError
{
    public:

        eResourceAcquisitionError(std::string reason) : eRuntimeError(reason) { }

        ~eResourceAcquisitionError() throw() { }
};


class eNullPointer : public ebObject
{
    public:

        eNullPointer(std::string reason) : ebObject(reason) { }

        ~eNullPointer() throw() { }
};


class eBufferOverflow : public ebObject
{
    public:

        eBufferOverflow(std::string reason) : ebObject(reason) { }

        ~eBufferOverflow() throw() { }
};


class eBufferUnderflow : public ebObject
{
    public:

        eBufferUnderflow(std::string reason) : ebObject(reason) { }

        ~eBufferUnderflow() throw() { }
};


}     // namespace rho


#endif
