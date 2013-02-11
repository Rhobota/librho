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

bool tAnimator::isRegistered(iAnimatable* animatable, i32 animationId) const
{
    tAnimation animation(animatable, animationId);
    return m_animationPayloadMap.find(animation) != m_animationPayloadMap.end();
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

    for (itr = m_animationPayloadMap.begin(); itr != m_animationPayloadMap.end(); ++itr)
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

template <class T>
bool areSetsEqual(const std::set<T>& a, const std::set<T>& b)
{
    if (a.size() != b.size())
        return false;

    typename std::set<T>::const_iterator itr, itr2;

    for (itr = a.begin(), itr2 = b.begin(); itr != a.end(); ++itr, ++itr2)
    {
        if (*itr != *itr2)
            return false;
    }

    return true;
}

template <class T, class U>
bool areMapsEqual(const std::map< T, std::set<U> >& a, const std::map< T, std::set<U> >& b)
{
    typename std::map< T, std::set<U> >::const_iterator itr, itr2;

    std::set<T> aKeys;
    for (itr = a.begin(); itr != a.end(); itr++)
        aKeys.insert(itr->first);

    std::set<T> bKeys;
    for (itr = b.begin(); itr != b.end(); itr++)
        bKeys.insert(itr->first);

    if (! areSetsEqual(aKeys, bKeys))
        return false;

    for (itr = a.begin(), itr2 = b.begin(); itr != a.end(); ++itr, ++itr2)
    {
        if (! areSetsEqual(itr->second, itr2->second))
            return false;
    }

    return true;
}

bool tAnimator::isConsistent() const
{
    std::map< iAnimatable*, std::set<i32> > derived;

    std::map<tAnimation,tAnimationPayload>::const_iterator itr;
    for (itr = m_animationPayloadMap.begin(); itr != m_animationPayloadMap.end(); ++itr)
    {
        const tAnimation& animation = itr->first;
        iAnimatable* animatable = animation.first;
        i32 animationId = animation.second;
        derived[animatable].insert(animationId);
    }

    return areMapsEqual(derived, m_activeAnimations);
}

i32 tAnimator::numRegistrations() const
{
    return (i32) (m_animationPayloadMap.size() + m_activeAnimations.size());
}


}   // namespace app
}   // namespace rho
