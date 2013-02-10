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
    u64 prevtime = m_currtime;
    m_currtime = currtime;

    size_t numCalls = m_animationPayloadMap.size();
    std::map<tAnimation,tAnimationPayload>::iterator itr;
    size_t currIndex = 0;

    std::vector<iAnimatable*>     ans(numCalls);
    std::vector<i32>              ids(numCalls);
    std::vector< refc<iPayload> > pays(numCalls);
    std::vector<u64>              starts(numCalls);

    for (itr = m_animationPayloadMap.begin(); itr != m_animationPayloadMap.end();
            ++itr)
    {
        const tAnimation& animation = itr->first;
        const tAnimationPayload& animationPayload = itr->second;

        iAnimatable* animatable = animation.first;
        i32 animationId = animation.second;
        refc<iPayload> payload = animationPayload.first;
        u64 starttime = animationPayload.second;

        ans[currIndex] = animatable;
        ids[currIndex] = animationId;
        pays[currIndex] = payload;
        starts[currIndex] = starttime;
        ++currIndex;
    }

    for (size_t i = 0; i < numCalls; i++)
    {
        bool keepGoing = ans[i]->stepAnimation(ids[i], pays[i],
                            starts[i], prevtime, currtime);
        if (!keepGoing)
            cancelAnimation(ans[i], ids[i]);
    }
}


}   // namespace app
}   // namespace rho
