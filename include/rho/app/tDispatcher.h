#ifndef __rho_app_tDispatcher_h__
#define __rho_app_tDispatcher_h__


#include <rho/app/iPayload.h>
#include <rho/app/iSource.h>
#include <rho/app/iTarget.h>
#include <rho/bNonCopyable.h>


namespace rho
{
namespace app
{


class tDispatcher : public bNonCopyable
{
    public:

        tDispatcher();

        ///////////////////////////////////////////////////////////////
        // Interface from targets:
        ///////////////////////////////////////////////////////////////

        // Adding targets.
        void addTargetToSource(iTarget* target, iSource* source);
        void addTargetToCode(iTarget* target, tNotificationCode code);
        void addTargetToSourceAndCode(iTarget* target,
                iSource* source, tNotificationCode code);

        // Removing targets.
        void removeTarget(iTarget* target);
        void removeTargetFromSource(iTarget* target, iSource* source);
        void removeTargetFromCode(iTarget* target, tNotificationCode code);
        void removeTargetFromSourceAndCode(iTarget* target,
                iSource* source, tNotificationCode code);

        ///////////////////////////////////////////////////////////////
        // Interface from sources:
        ///////////////////////////////////////////////////////////////

        // Posting notifications.
        void notifyTargets(iSource* source,
                           tNotificationCode code,
                           iPayload* payload = NULL);

    private:

        // Used for posting notification.
        std::map< iSource*, std::set<iTarget*> >          m_srcToTargets;
        std::map< tNotificationCode, std::set<iTarget*> > m_codeToTargets;

        // Used for removing targets quickly.
        std::map< iTarget*, std::set<iSource*> >          m_targetToSrcs;
        std::map< iTarget*, std::set<tNotificationCode> > m_targetToCodes;
};


}    // namespace app
}    // namespace rho


#endif   // __rho_app_tDispatcher_h__
