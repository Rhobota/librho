#ifndef __rho_sync_iStoppable_h__
#define __rho_sync_iStoppable_h__


#include <rho/ppcheck.h>


namespace rho
{
namespace sync
{


/**
 * This interface is often implemented along side iRunnable,
 * so that a runnable can be told to stop before it would
 * naturally stop by itself.
 */
class iStoppable
{
    public:

        /**
         * Tells this object to stop.
         *
         * This method is typically used with a runnable
         * so you can tell it explicitly to stop running.
         */
        virtual void stop() = 0;

        virtual ~iStoppable() { }
};


}   // namespace sync
}   // namespace rho


#endif   // __rho_sync_iStoppable_h__
