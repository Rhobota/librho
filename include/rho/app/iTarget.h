#ifndef __rho_app_iTarget_h__
#define __rho_app_iTarget_h__


#include <rho/app/iPayload.h>
#include <rho/app/iSource.h>


namespace rho
{
namespace app
{


typedef std::string tNotificationCode;


class iTarget
{
    public:

        /**
         * Called by tDispatcher when 'source' posts a new notification.
         * For this method to be called, the receiver must have already
         * registered for notifications from source for the specific code.
         * It is also possible that the receive registered for all
         * notification posted by 'source' (no matter what the code value),
         * or that the receiver registered for all notification with some
         * value for 'code' (no matter what 'source').
         *
         * 'source' is the object which posted the notification.
         * 'code' and 'payload' are constitute the notification content.
         *
         * Important: Do not hold a reference to 'source' or 'payload'.
         *            The lifetimes of 'source' and 'payload' are only
         *            guaranteed for the duration of this method's
         *            execution.
         */
        virtual void notify(iSource* source,
                            tNotificationCode code,
                            iPayload* payload) = 0;

        virtual ~iTarget() { }
};


}   // namespace app
}   // namespace rho


#endif   // __rho_app_iTarget_h__
