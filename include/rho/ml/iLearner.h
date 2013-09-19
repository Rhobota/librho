#ifndef __rho_ml_iLearner_h__
#define __rho_ml_iLearner_h__


#include <rho/types.h>

#include <vector>


namespace rho
{
namespace ml
{


/**
 * This object will be used to denote input to the learner as
 * well as output from the learner. Both input/output to/from
 * this learner are vector data.
 *
 * This typedef makes it easier to represent several input examples
 * or several target examples without having to explicitly declare
 * vectors of vectors of floats.
 */
typedef std::vector<f64> tIO;


/**
 * This learner learns with vector data input and vector data output.
 */
class iLearner
{
    public:

        /**
         * Shows the learner one example. The learner will calculate error rates
         * (or whatever it does with examples), then accumulate the error in some
         * way or another.
         *
         * The learner will not become smarter by calling this method. You must
         * subsequently call update().
         */
        virtual void addExample(const tIO& input, const tIO& target,
                                  tIO& actualOutput) = 0;

        /**
         * Updates the learner to account for all the examples it's seen since
         * the last call to update().
         *
         * The accumulated error rates (or whatever) are then cleared.
         */
        virtual void update() = 0;

        /**
         * Uses the current knowledge of the learner to evaluate the given input.
         */
        virtual void evaluate(const tIO& input, tIO& output) = 0;

        /**
         * Virtual dtor...
         */
        virtual ~iLearner() { }
};


}   // namespace ml
}   // namespace rho


#endif  // __rho_ml_iLearner_h__
