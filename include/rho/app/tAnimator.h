#ifndef __rho_app_tAnimator_h__
#define __rho_app_tAnimator_h__


#include <rho/app/iAnimatable.h>
#include <rho/bNonCopyable.h>


namespace rho
{
namespace app
{


class tAnimator : public bNonCopyable
{
    public:

        tAnimator();

        ///////////////////////////////////////////////////////////////
        // Registering new animations:
        ///////////////////////////////////////////////////////////////

        void registerNewAnimation(iAnimatable* animatable, i32 animationId,
                                      refc<iPayload> payload);

        ///////////////////////////////////////////////////////////////
        // Cancelling existing animations:
        ///////////////////////////////////////////////////////////////

        void cancelAnimation(iAnimatable* animatable, i32 animationId);
        void cancelAllAnimationsOn(iAnimatable* animatable);

        void cancelAndRollbackAnimation(iAnimatable* animatable,i32 animationId);
        void cancelAndRollbackAllAnimationsOn(iAnimatable* animatable);

        ///////////////////////////////////////////////////////////////
        // Stepping all current animations...
        // Called by the application's main loop...
        // Causes stepAnimation() to be invoked for each
        // active animation...
        ///////////////////////////////////////////////////////////////

        void stepAllAnimations(u64 currtime);
};


}    // namespace app
}    // namespace rho


#endif   // __rho_app_tAnimator_h__
