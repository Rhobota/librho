#ifndef __rho_app_iAnimatable_h__
#define __rho_app_iAnimatable_h__


#include <rho/app/iPayload.h>
#include <rho/refc.h>
#include <rho/types.h>


namespace rho
{
namespace app
{


class iAnimatable
{
    public:

        /**
         * This method defines the interface for animatable objects.
         * A tAnimator object is used to drive the animation of many
         * animatable objects. Animation is performed by the animatable
         * object when the tAnimator object calls this method. For this
         * method to be called, the animatable object must be registered
         * with a tAnimator object. The same animatable object can be
         * registered many times with the same tAnimator object, each
         * using a different animationId. Doing so represents concurrent
         * animations on the same object, each operating independently
         * as if they were on separate objects.
         *
         * This method should return true when the animation has not ended.
         * Doing so indicates to the tAnimator that it should continue calling.
         *
         * This method should return false when the animation has ended.
         * Doing so causes the tAnimator to remove it from its list of active
         * animations, meaning that this method will not be called again for
         * the current animationId until it is re-registered.
         */
        virtual bool stepAnimation(i32 animationId, refc<iPayload> payload,
                           u64 starttime, u64 prevtime, u64 currtime) = 0;

        virtual ~iAnimatable() { }
};


}   // namespace app
}   // namespace rho


#endif   // __rho_app_iAnimatable_h__
