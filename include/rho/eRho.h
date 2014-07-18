#ifndef __rho_eRho_h__
#define __rho_eRho_h__


#include <rho/ppcheck.h>
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


class eNotImplemented : public ebObject
{
    public:

        eNotImplemented(std::string reason) : ebObject(reason) { }

        ~eNotImplemented() throw() { }
};


class eInvalidArgument : public eLogicError
{
    public:

        eInvalidArgument(std::string reason) : eLogicError(reason) { }

        ~eInvalidArgument() throw() { }
};


class eImpossiblePath : public eLogicError
{
    public:

        eImpossiblePath()
            : eLogicError("This codepath should be unreachable.") { }

        ~eImpossiblePath() throw () { }
};


class eResourceAcquisitionError : public eRuntimeError
{
    public:

        eResourceAcquisitionError(std::string reason) : eRuntimeError(reason) {}

        ~eResourceAcquisitionError() throw() { }
};


class eNullPointer : public eLogicError
{
    public:

        eNullPointer(std::string reason) : eLogicError(reason) { }

        ~eNullPointer() throw() { }
};


class eBufferOverflow : public eLogicError
{
    public:

        eBufferOverflow(std::string reason) : eLogicError(reason) { }

        ~eBufferOverflow() throw() { }
};


class eBufferUnderflow : public eLogicError
{
    public:

        eBufferUnderflow(std::string reason) : eLogicError(reason) { }

        ~eBufferUnderflow() throw() { }
};


}     // namespace rho


#endif
