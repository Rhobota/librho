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

        tAnimator(u64 currtime);

        ///////////////////////////////////////////////////////////////
        // Registering new animations:
        ///////////////////////////////////////////////////////////////

        void registerNewAnimation(iAnimatable* animatable, i32 animationId,
                                      refc<iPayload> payload);

        bool isRegistered(iAnimatable* animatable, i32 animationId) const;

        ///////////////////////////////////////////////////////////////
        // Cancelling existing animations:
        ///////////////////////////////////////////////////////////////

        void cancelAnimation(iAnimatable* animatable, i32 animationId);
        void cancelAllAnimationsOn(iAnimatable* animatable);

        void cancelAndRollbackAnimation(iAnimatable* animatable, i32 animationId);
        void cancelAndRollbackAllAnimationsOn(iAnimatable* animatable);

        ///////////////////////////////////////////////////////////////
        // Stepping all current animations...
        // Called by the application's main loop...
        // Causes stepAnimation() to be invoked for each
        // active animation...
        ///////////////////////////////////////////////////////////////

        void stepAllAnimations(u64 currtime);

    private:

        typedef std::pair<iAnimatable*,i32>   tAnimation;
        typedef std::pair<refc<iPayload>,u64> tAnimationPayload;

        u64 m_currtime;
        std::map< iAnimatable*, std::set<i32> >   m_activeAnimations;
        std::map< tAnimation, tAnimationPayload > m_animationPayloadMap;

    public:

        ///////////////////////////////////////////////////////////////
        // Debugging methods (used only for testing):
        ///////////////////////////////////////////////////////////////

        bool isConsistent() const;
        i32 numRegistrations() const;
};


}    // namespace app
}    // namespace rho


#endif   // __rho_app_tAnimator_h__
