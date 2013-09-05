#ifndef __rho_ml_common_h__
#define __rho_ml_common_h__


#include <rho/ml/iLearner.h>
#include <rho/eRho.h>
#include <rho/types.h>
#include <rho/img/tImage.h>

#include <cmath>
#include <iostream>
#include <vector>


namespace rho
{
namespace ml
{


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// The logistic function:
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

f64 logistic_function(f64 z);
f64 derivative_of_logistic_function(f64 z);
f64 logistic_function_min();
f64 logistic_function_max();


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// The hyperbolic function:
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

f64 hyperbolic_function(f64 z);
f64 derivative_of_hyperbolic_function(f64 z);
f64 hyperbolic_function_min();
f64 hyperbolic_function_max();


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// IO manipulation tools:
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
 * Turns the given image into an example that can be trained-on.
 */
tIO examplify(const img::tImage* image);

/**
 * Turns the integer value into an example that can be trained-on.
 * The returned training example has 'numDimensions' number of
 * dimensions, where one dimension is set to 1.0, and all others
 * are set to 0.0. The high dimension's index is given by
 * 'highDimension'.
 */
tIO examplify(u32 highDimension, u32 numDimensions);

/**
 * Does the opposite operation as the above examplify() function.
 * It does so by determining which dimension has the highest
 * value, and returns the index to that dimension. If 'error'
 * is not NULL, the standard error between the given output
 * and the "correct" output is calculated and stored in 'error'.
 * The "correct" output is obtained by calling the examplify()
 * function above. The assumption is made that the returned
 * index for the highest dimension is correct, thus the method
 * calculates the standard error between the given output and
 * the "correct" output.
 */
u32 un_examplify(const tIO& output, f64* error = NULL);


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Error measures:
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
 * Calculates and returns the standard error between the given
 * output and the given target.
 */
f64 standardSquaredError(const tIO& output, const tIO& target);

/**
 * Calculates the average standard error between each output/target
 * pair.
 */
f64 standardSquaredError(const std::vector<tIO>& outputs,
                         const std::vector<tIO>& targets);


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Confusion matrix tools:
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
 * This typedef make creating a confusion matrix easier.
 */
typedef std::vector< std::vector<u32> > tConfusionMatrix;

/**
 * Creates a confusion matrix for the given output/target
 * pairs. For each output/target pair, un_examplify() is
 * called twice (once for the output and once for the target).
 * The corresponding entry in the confusion matrix is then
 * incremented for each output/target pair.
 */
void buildConfusionMatrix(const std::vector<tIO>& outputs,
                          const std::vector<tIO>& targets,
                                tConfusionMatrix& confusionMatrix);

/**
 * Prints the confusion matrix in a pretty format.
 */
void print        (const tConfusionMatrix& confusionMatrix, std::ostream& out);

/**
 * Calculates the error rate for the given confusion matrix.
 * Works for any confusion matrix.
 */
f64  errorRate    (const tConfusionMatrix& confusionMatrix);

/**
 * Calculates the accuracy for the given confusion matrix.
 * Works for any confusion matrix.
 */
f64  accuracy     (const tConfusionMatrix& confusionMatrix);

/**
 * Calculates the precision of the confusion matrix.
 * Only works for confusion matrices that have true/false
 * dimensions (aka, confusion matrices that are two-by-two).
 */
f64  precision    (const tConfusionMatrix& confusionMatrix);

/**
 * Calculates the recall of the confusion matrix.
 * Only works for confusion matrices that have true/false
 * dimensions (aka, confusion matrices that are two-by-two).
 */
f64  recall       (const tConfusionMatrix& confusionMatrix);


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Training helpers:
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/**
 * This callback function is called by train(). It should
 * return true if all is well and training should continue.
 * It should return false if the training process should
 * halt. This is useful if you need to cancel training
 * due to user input, or something like that.
 */
typedef bool (*train_didUpdate_callback)(iLearner* learner, void* context);

/**
 * This functions trains the leaner on the given examples,
 * calling the callback function after each batch has been
 * processed by the learner. This function returns true if
 * the training process completed fully, and it returns false
 * if the callback function indicated that training should
 * halt.
 * Use this for training on the training-set examples.
 */
bool train(iLearner* learner, const std::vector<tIO>& inputs,
                              const std::vector<tIO>& targets,
                                    std::vector<tIO>& outputs,  // <-- populated during training
                              u32 batchSize,
                              train_didUpdate_callback callback = NULL,
                              void* callbackContext = NULL);

/**
 * This function tests the learner on the given examples.
 * Use this for evaluating the test-set examples.
 */
void evaluate(iLearner* learner, const std::vector<tIO>& inputs,
                                       std::vector<tIO>& outputs);


}    // namespace ml
}    // namespace rho


#endif   // __rho_ml_common_h__
