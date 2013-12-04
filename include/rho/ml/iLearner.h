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
        virtual void evaluate(const tIO& input, tIO& output) const = 0;

        /**
         * Asks the learner to calculate the error between the given output
         * and the given target. For example, the learner may calculate
         * the standard squared error or the cross-entropy loss, if one of
         * those is appropriate. Or the learner may do something else.
         */
        virtual f64 calculateError(const tIO& output, const tIO& target) = 0;

        /**
         * Asks the learner to calculate the error between all the given
         * output/target pairs. For example, the learner may calculate
         * the average standard squared error or the average cross-entropy
         * loss, if one of those is appropriate. Or the learner may do
         * something else.
         */
        virtual f64 calculateError(const std::vector<tIO>& outputs,
                                   const std::vector<tIO>& targets) = 0;

        /**
         * Resets the learner to its initial state.
         */
        virtual void reset() = 0;

        /**
         * Prints the learner's configuration in a readable format.
         */
        virtual void printLearnerInfo(std::ostream& out) const = 0;

        /**
         * Returns a single-line version of printLearnerInfo().
         */
        virtual std::string learnerInfoString() const = 0;

        /**
         * Virtual dtor...
         */
        virtual ~iLearner() { }

    public:

        /**
         * Deserialize the given vector into a new copy of an iLearner
         * object.
         *
         * The caller of this method must call delete on the returned
         * value when they are finished using it.
         */
        static iLearner* newDeserializedLearner(const std::vector<u8>& serializedLearner);

        /**
         * Serializes a learner to a flat vector.
         */
        static void serializeLearner(iLearner* learner, std::vector<u8>& serializedLearner);
};


}   // namespace ml
}   // namespace rho


#endif  // __rho_ml_iLearner_h__
