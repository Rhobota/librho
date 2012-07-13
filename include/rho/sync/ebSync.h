#ifndef __rho_sync_ebSync_h__
#define __rho_sync_ebSync_h__


#include "rho/ebObject.h"

#include <string>


namespace rho
{
namespace sync
{


class ebSync : public ebObject
{
    public:

        ebSync(std::string reason) : ebObject(reason) { }

        ~ebSync() throw() { }
};


class eThreadCreationError : public ebSync
{
    public:

        eThreadCreationError()
            : ebSync("Thread creation failed because either there are not enough resources or because there is a system-imposed limit on the number of threads allowed per process.")
        {
        }

        ~eThreadCreationError() throw() { }
};


class eThreadCannotBeJoinedError : public ebSync
{
    public:

        eThreadCannotBeJoinedError(std::string reason) : ebSync(reason) { }

        ~eThreadCannotBeJoinedError() throw() { }
};


class eThreadCannotBeDetachedError : public ebSync
{
    public:

        eThreadCannotBeDetachedError(std::string reason) : ebSync(reason) { }

        ~eThreadCannotBeDetachedError() throw() { }
};


class eMutexCreationError : public ebSync
{
    public:

        eMutexCreationError()
            : ebSync("The system lacks the resources to create another mutex.")
        {
        }

        ~eMutexCreationError() throw() { }
};


class eDeadlockDetected : public ebSync
{
    public:

        eDeadlockDetected() : ebSync("A deadlock has occurred.") { }

        ~eDeadlockDetected() throw() { }
};


}   // namespace sync
}   // namespace rho


#endif   // __rho_sync_ebSync_h__