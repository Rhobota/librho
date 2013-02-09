#include <rho/app/tDispatcher.h>


namespace rho
{
namespace app
{


tDispatcher::tDispatcher()
{
    // Nothing to do...
}

void tDispatcher::addTargetToSource(iTarget* target, iSource* source)
{
    m_srcToTargets[source].insert(target);
    m_targetToSrcs[target].insert(source);
}

void tDispatcher::addTargetToCode(iTarget* target, tNotificationCode code)
{
    m_codeToTargets[code].insert(target);
    m_targetToCodes[target].insert(code);
}

void tDispatcher::addTargetToSourceCodePair(iTarget* target,
                        iSource* source, tNotificationCode code)
{
    tSrcCodePair pair(source, code);
    m_pairToTargets[pair].insert(target);
    m_targetToPairs[target].insert(pair);
}

void tDispatcher::removeTarget(iTarget* target)
{
    // Remove all references to target from the source lists.
    {
        std::map< iTarget*, std::set<iSource*> >::iterator ttositr
            = m_targetToSrcs.find(target);
        if (ttositr != m_targetToSrcs.end())
        {
            std::set<iSource*>& sources = ttositr->second;
            std::set<iSource*>::iterator sitr;
            for (sitr = sources.begin(); sitr != sources.end(); ++sitr)
            {
                m_srcToTargets[*sitr].erase(target);
                if (m_srcToTargets[*sitr].size() == 0)
                    m_srcToTargets.erase(*sitr);
            }
            m_targetToSrcs.erase(target);
        }
    }

    // Remove all references to target from the code lists.
    {
        std::map< iTarget*, std::set<tNotificationCode> >::iterator ttocitr
            = m_targetToCodes.find(target);
        if (ttocitr != m_targetToCodes.end())
        {
            std::set<tNotificationCode>& codes = ttocitr->second;
            std::set<tNotificationCode>::iterator citr;
            for (citr = codes.begin(); citr != codes.end(); ++citr)
            {
                m_codeToTargets[*citr].erase(target);
                if (m_codeToTargets[*citr].size() == 0)
                    m_codeToTargets.erase(*citr);
            }
            m_targetToCodes.erase(target);
        }
    }

    // Remove all references to target from the pairs lists.
    {
        std::map< iTarget*, std::set<tSrcCodePair> >::iterator ttopitr
            = m_targetToPairs.find(target);
        if (ttopitr != m_targetToPairs.end())
        {
            std::set<tSrcCodePair>& pairs = ttopitr->second;
            std::set<tSrcCodePair>::iterator pitr;
            for (pitr = pairs.begin(); pitr != pairs.end(); ++pitr)
            {
                m_pairToTargets[*pitr].erase(target);
                if (m_pairToTargets[*pitr].size() == 0)
                    m_pairToTargets.erase(*pitr);
            }
            m_targetToPairs.erase(target);
        }
    }
}

void tDispatcher::removeTargetFromSource(iTarget* target, iSource* source)
{
    // Remove targets from the source-only lists.
    m_srcToTargets[source].erase(target);
    if (m_srcToTargets[source].size() == 0)
        m_srcToTargets.erase(source);
    m_targetToSrcs[target].erase(source);
    if (m_targetToSrcs[target].size() == 0)
        m_targetToSrcs.erase(target);

    // Remove from the pairs lists where pair_source==source.
    {
        std::map< iTarget*, std::set<tSrcCodePair> >::iterator ttopitr
            = m_targetToPairs.find(target);
        if (ttopitr != m_targetToPairs.end())
        {
            std::set<tSrcCodePair>& pairs = ttopitr->second;
            std::set<tSrcCodePair>::iterator pitr;
            std::vector<tSrcCodePair> needErasing;
            for (pitr = pairs.begin(); pitr != pairs.end(); ++pitr)
            {
                if (pitr->first == source)
                {
                    m_pairToTargets[*pitr].erase(target);
                    if (m_pairToTargets[*pitr].size() == 0)
                        m_pairToTargets.erase(*pitr);
                    needErasing.push_back(*pitr);     // <-- to avoid stale iterators
                }
            }
            for (size_t i = 0; i < needErasing.size(); i++)
                m_targetToPairs[target].erase(needErasing[i]);
            if (m_targetToPairs[target].size() == 0)
                m_targetToPairs.erase(target);
        }
    }
}

void tDispatcher::removeTargetFromCode(iTarget* target, tNotificationCode code)
{
    // Remove targets from the code-only lists.
    m_codeToTargets[code].erase(target);
    if (m_codeToTargets[code].size() == 0)
        m_codeToTargets.erase(code);
    m_targetToCodes[target].erase(code);
    if (m_targetToCodes[target].size() == 0)
        m_targetToCodes.erase(target);

    // Remove from the pairs lists where pair_code==code.
    {
        std::map< iTarget*, std::set<tSrcCodePair> >::iterator ttopitr
            = m_targetToPairs.find(target);
        if (ttopitr != m_targetToPairs.end())
        {
            std::set<tSrcCodePair>& pairs = ttopitr->second;
            std::set<tSrcCodePair>::iterator pitr;
            std::vector<tSrcCodePair> needErasing;
            for (pitr = pairs.begin(); pitr != pairs.end(); ++pitr)
            {
                if (pitr->second == code)
                {
                    m_pairToTargets[*pitr].erase(target);
                    if (m_pairToTargets[*pitr].size() == 0)
                        m_pairToTargets.erase(*pitr);
                    needErasing.push_back(*pitr);     // <-- to avoid stale iterators
                }
            }
            for (size_t i = 0; i < needErasing.size(); i++)
                m_targetToPairs[target].erase(needErasing[i]);
            if (m_targetToPairs[target].size() == 0)
                m_targetToPairs.erase(target);
        }
    }
}

void tDispatcher::removeTargetFromSourceCodePair(iTarget* target,
                        iSource* source, tNotificationCode code)
{
    tSrcCodePair pair(source, code);
    m_pairToTargets[pair].erase(target);
    if (m_pairToTargets[pairs].size() == 0)
        m_pairToTargets.erase(pairs);
    m_targetToPairs[target].erase(pair);
    if (m_targetToPairs[target].size() == 0)
        m_targetToPairs.erase(target);
}

void tDispatcher::notifyTargets(iSource* source,
                                tNotificationCode code,
                                iPayload* payload)
{
    std::set<iTarget*> needNotify;

    // Get targets from the source-only list.
    {
        std::map< iSource*, std::set<iTarget*> >::iterator sitr
            = m_srcToTargets.find(source);
        if (sitr != m_srcToTargets.end())
        {
            std::set<iTarget*>& targets = sitr->second;
            needNotify.insert(targets.begin(), targets.end());
        }
    }

    // Get targets from the code-only list.
    {
        std::map< tNotificationCode, std::set<iTarget*> >::iterator citr
            = m_codeToTargets.find(code);
        if (citr != m_codeToTargets.end())
        {
            std::set<iTarget*>& targets = citr->second;
            needNotify.insert(targets.begin(), targets.end());
        }
    }

    // Get targets from the pairs list.
    {
        tSrcCodePair pair(source, code);
        std::map< tSrcCodePair, std::set<iTarget*> >::iterator pitr
            = m_pairToTargets.find(pair);
        if (pitr != m_pairToTargets.end())
        {
            std::set<iTarget*>& targets = pitr->second;
            needNotify.insert(targets.begin(), targets.end());
        }
    }

    // Notify all!
    std::set<iTarget*>::iterator itr;
    for (itr = needNotify.begin(); itr != needNotify.end(); ++itr)
    {
        itr->notify(source, code, payload);
    }
}


}    // namespace app
}    // namespace rho
