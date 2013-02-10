#include <rho/app/tAnimator.h>
#include <rho/eRho.h>


namespace rho
{
namespace app
{


tAnimator::tAnimator(u64 currtime)
    : m_currtime(currtime)
{
}

void tAnimator::registerNewAnimation(iAnimatable* animatable, i32 animationId,
                              refc<iPayload> payload)
{
    tAnimation animation(animatable, animationId);
    if (m_animationPayloadMap.find(animation) != m_animationPayloadMap.end())
    {
        throw eLogicError("Cannot create two animations with same animatable "
                "object and same animation id.");
    }

    tAnimationPayload animationPayload(payload, m_currtime);
    m_animationPayloadMap[animation] = animationPayload;

    m_activeAnimations[animatable].insert(animationId);
}

void tAnimator::cancelAnimation(iAnimatable* animatable, i32 animationId)
{
    tAnimation animation(animatable, animationId);

    if (m_animationPayloadMap.find(animation) != m_animationPayloadMap.end())
    {
        m_animationPayloadMap.erase(animation);

        m_activeAnimations[animatable].erase(animationId);
        if (m_activeAnimations[animatable].size() == 0)
            m_activeAnimations.erase(animatable);
    }
}

void tAnimator::cancelAllAnimationsOn(iAnimatable* animatable)
{
    if (m_activeAnimations.find(animatable) != m_activeAnimations.end())
    {
        std::set<i32> animationIds = m_activeAnimations[animatable];  // a copy
        std::set<i32>::iterator itr;
        for (itr = animationIds.begin(); itr != animationIds.end(); ++itr)
            cancelAnimation(animatable, *itr);
    }
}

void tAnimator::cancelAndRollbackAnimation(iAnimatable* animatable,
                                           i32 animationId)
{
    tAnimation animation(animatable, animationId);

    if (m_animationPayloadMap.find(animation) != m_animationPayloadMap.end())
    {
        tAnimationPayload animationPayload = m_animationPayloadMap[animation];
        refc<iPayload> payload = animationPayload.first;
        u64 starttime = animationPayload.second;

        animatable->stepAnimation(animationId, payload,
                starttime, m_currtime, starttime);       // <-- the rollback

        cancelAnimation(animatable, animationId);
    }
}

void tAnimator::cancelAndRollbackAllAnimationsOn(iAnimatable* animatable)
{
    if (m_activeAnimations.find(animatable) != m_activeAnimations.end())
    {
        std::set<i32> animationIds = m_activeAnimations[animatable];  // a copy
        std::set<i32>::iterator itr;
        for (itr = animationIds.begin(); itr != animationIds.end(); ++itr)
            cancelAndRollbackAnimation(animatable, *itr);
    }
}

void tAnimator::stepAllAnimations(u64 currtime)
{
    std::map<tAnimation,tAnimationPayload>::iterator itr;
    for (itr = m_animationPayloadMap.begin(); itr != m_animationPayloadMap.end();
            ++itr)
    {
        const tAnimation& animation = itr->first;
        tAnimationPayload& animationPayload = itr->second;

        iAnimatable* animatable = animation.first;
        i32 animationId = animation.second;
        refc<iPayload> payload = animationPayload.first;
        u64 starttime = animationPayload.second;

        animatable->stepAnimation(animationId, payload,
                starttime, m_currtime, currtime);
    }

    m_currtime = currtime;
}


}   // namespace app
}   // namespace rho
