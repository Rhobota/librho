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


// Logistic function:
f64 logistic_function(f64 z);
f64 derivative_of_logistic_function(f64 z);
f64 logistic_function_min();
f64 logistic_function_max();


// Hyperbolic function:
f64 hyperbolic_function(f64 z);
f64 derivative_of_hyperbolic_function(f64 z);
f64 hyperbolic_function_min();
f64 hyperbolic_function_max();


// Learner's IO manipulation tools:
tIO examplify(const img::tImage* image);
tIO examplify(u32 highDimension, u32 numDimensions);
u32 un_examplify(const tIO& output, f64* error = NULL);


// Error measures:
f64 standardSquaredError(const tIO& output, const tIO& target);
f64 standardSquaredError(const std::vector<tIO>& outputs,
                         const std::vector<tIO>& targets);


// Confusion matrix tools:
typedef std::vector< std::vector<u32> > tConfusionMatrix;
void buildConfusionMatrix(const std::vector<tIO>& outputs,
                          const std::vector<tIO>& targets,
                                tConfusionMatrix& confusionMatrix);
void print        (const tConfusionMatrix& confusionMatrix, std::ostream& out);
f64  errorRate    (const tConfusionMatrix& confusionMatrix);
f64  accuracy     (const tConfusionMatrix& confusionMatrix);
f64  precision    (const tConfusionMatrix& confusionMatrix);
f64  recall       (const tConfusionMatrix& confusionMatrix);


// Training helpers:
void train(iLearner& learner, const std::vector<tIO>& inputs,
                              const std::vector<tIO>& targets,
                                    std::vector<tIO>& outputs,  // <-- populated during training
                              u32 batchSize);
void evaluate(iLearner& learner, const std::vector<tIO>& inputs,
                                       std::vector<tIO>& outputs);


}    // namespace ml
}    // namespace rho


#endif   // __rho_ml_common_h__
