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

void tDispatcher::addTargetToSourceAndCode(iTarget* target,
                        iSource* source, tNotificationCode code)
{
    addTargetForSource(target, source);
    addTargetForCode(target, code);
}

void tDispatcher::removeTarget(iTarget* target)
{
    // Remove all references to target from the source lists.
    {
        std::map< iTarget*, std::set<iSource*> >::iterator ttositr;
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
        std::map< iTarget*, std::set<tNotificationCode> >::iterator ttocitr;
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
}

void tDispatcher::removeTargetFromSource(iTarget* target, iSource* source)
{
    m_srcToTargets[source].erase(target);
    if (m_srcToTargets[source].size() == 0)
        m_srcToTargets.erase(source);
    m_targetToSrcs[target].erase(source);
    if (m_targetToSrcs[target].size() == 0)
        m_targetToSrcs.erase(target);
}

void tDispatcher::removeTargetFromCode(iTarget* target, tNotificationCode code)
{
    m_codeToTargets[code].erase(target);
    if (m_codeToTargets[code].size() == 0)
        m_codeToTargets.erase(code);
    m_targetToCodes[target].erase(code);
    if (m_targetToCodes[target].size() == 0)
        m_targetToCodes.erase(target);
}

void tDispatcher::removeTargetFromSourceAndCode(iTarget* target,
                        iSource* source, tNotificationCode code)
{
    removeTargetFromSource(target, source);
    removeTargetFromCode(target, code);
}

void tDispatcher::notifyTargets(iSource* source,
                                tNotificationCode code,
                                iPayload* payload)
{
}


}    // namespace app
}    // namespace rho
