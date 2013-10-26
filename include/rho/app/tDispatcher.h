#ifndef __rho_app_tDispatcher_h__
#define __rho_app_tDispatcher_h__


#include <rho/app/iPayload.h>
#include <rho/app/iSource.h>
#include <rho/app/iTarget.h>
#include <rho/bNonCopyable.h>
#include <rho/types.h>


namespace rho
{
namespace app
{


class tDispatcher : public bNonCopyable
{
    public:

        tDispatcher();

        ///////////////////////////////////////////////////////////////
        // Interface for targets:
        ///////////////////////////////////////////////////////////////

        // Adding targets.
        void addTargetToSource(iTarget* target, iSource* source);
        void addTargetToCode(iTarget* target, tNotificationCode code);
        void addTargetToSourceCodePair(iTarget* target,
                iSource* source, tNotificationCode code);

        // Removing targets.
        void removeTarget(iTarget* target);
        void removeTargetFromSource(iTarget* target, iSource* source);
        void removeTargetFromCode(iTarget* target, tNotificationCode code);
        void removeTargetFromSourceCodePair(iTarget* target,
                iSource* source, tNotificationCode code);

        ///////////////////////////////////////////////////////////////
        // Interface for sources:
        ///////////////////////////////////////////////////////////////

        // Posting notifications.
        void notifyTargets(iSource* source,
                           tNotificationCode code,
                           iPayload* payload = NULL);

    private:

        typedef std::pair<iSource*,tNotificationCode> tSrcCodePair;

        // Used for posting notification.
        std::map< iSource*, std::set<iTarget*> >          m_srcToTargets;
        std::map< tNotificationCode, std::set<iTarget*> > m_codeToTargets;
        std::map< tSrcCodePair, std::set<iTarget*> >      m_pairToTargets;

        // Used for removing targets quickly.
        std::map< iTarget*, std::set<iSource*> >          m_targetToSrcs;
        std::map< iTarget*, std::set<tNotificationCode> > m_targetToCodes;
        std::map< iTarget*, std::set<tSrcCodePair> >      m_targetToPairs;

    public:

        ///////////////////////////////////////////////////////////////
        // Debugging methods (used only for testing):
        ///////////////////////////////////////////////////////////////

        bool isConsistent() const;
        i32 numRegistrations() const;
};


}    // namespace app
}    // namespace rho


#endif   // __rho_app_tDispatcher_h__
