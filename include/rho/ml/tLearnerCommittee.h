#ifndef __rho_ml_tLearnerCommittee_h__
#define __rho_ml_tLearnerCommittee_h__


#include <rho/refc.h>
#include <rho/ml/common.h>
#include <rho/ml/iLearner.h>
#include <rho/sync/tThreadPool.h>

#include <vector>


namespace rho
{
namespace ml
{


class tLearnerCommittee : public iLearner
{
    public:

        /**
         * Constructs a committee of trained learners.
         */
        tLearnerCommittee(const std::vector< refc<iLearner> >& committee);

        /**
         * Constructs a committee of trained learners, and uses a thread
         * pool of the given size for evalutate().
         */
        tLearnerCommittee(const std::vector< refc<iLearner> >& committee,
                          u32 threadPoolSize);

        /**
         * D'tor
         */
        ~tLearnerCommittee();

        /////////////////////////////////////////////////////////////////////
        // iLearner interface
        /////////////////////////////////////////////////////////////////////

        /**
         * Evaluate the input using the committee.
         */
        void evaluate(const tIO& input, tIO& output) const;

        /**
         * Prints the each of the committee's learner's configuration.
         */
        void printLearnerInfo(std::ostream& out) const;

        /**
         * Returns a single-line version of printLearnerInfo().
         */
        std::string learnerInfoString() const;

        /**
         * Calculates the average error of the learners in the committee.
         */
        f64 calculateError(const tIO& output, const tIO& target);

        /**
         * Calculates the average error of the learners in the committee.
         */
        f64 calculateError(const std::vector<tIO>& outputs,
                           const std::vector<tIO>& targets);

        /**
         * Do not call this. You cannot train a committee. They must be
         * already trained...
         */
        void addExample(const tIO& input, const tIO& target,
                              tIO& actualOutput);

        /**
         * Do not call this. You cannot train a committee. They must be
         * already trained...
         */
        void update();

        /**
         * Do not call this. Resetting the committee makes no sense
         * because the committee cannot be re-trained after the reset.
         */
        void reset();

    private:

        std::vector< refc<iLearner> > m_committee;

        sync::tThreadPool* m_threadPool;
};


}   // namespace ml
}   // namespace rho


#endif   // __rho_ml_tLearnerCommittee_h__
