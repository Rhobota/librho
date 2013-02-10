#include <rho/app/tAnimator.h>


namespace rho
{
namespace app
{


tAnimator::tAnimator()
{
}

void tAnimator::registerNewAnimation(iAnimatable* animatable, i32 animationId,
                              refc<iPayload> payload)
{
}

void tAnimator::cancelAnimation(iAnimatable* animatable, i32 animationId)
{
}

void tAnimator::cancelAllAnimationsOn(iAnimatable* animatable)
{
}

void tAnimator::cancelAndRollbackAnimation(iAnimatable* animatable,
                                           i32 animationId)
{
}

void tAnimator::cancelAndRollbackAllAnimationsOn(iAnimatable* animatable)
{
}

void tAnimator::stepAllAnimations(u64 currtime)
{
}


}   // namespace app
}   // namespace rho
