#ifndef __rho_ml_common_h__
#define __rho_ml_common_h__


#include <rho/eRho.h>
#include <rho/types.h>
#include <rho/img/tImage.h>

#include <cmath>
#include <iostream>
#include <vector>
using std::vector;


namespace rho
{
namespace ml
{


// Logistic function:
f64 logistic_function(f64 z);
f64 derivative_of_logistic_function(f64 z);
f64 logistic_function_min();
f64 logistic_function_max();


// Example manipulation tools:
vector<f64> examplify(const img::tImage* image);
vector<f64> examplify(u32 val, u32 vectorSize);
u32 un_examplify(const vector<f64>& output, f64* error = NULL);


// Error measures:
f64 standardSquaredError(const vector<f64>& output, const vector<f64>& target);
f64 standardSquaredError(const vector< vector<f64> >& outputExamples,
                         const vector< vector<f64> >& targetExamples);


// Confusion matrix tools:
void buildConfusionMatrix(const vector< vector<f64> >& outputExamples,
                          const vector< vector<f64> >& targetExamples,
                                vector< vector<u32> >& confusionMatrix);
void print        (const vector< vector<u32> >& confusionMatrix, std::ostream& out);
f64  errorRate    (const vector< vector<u32> >& confusionMatrix);
f64  accuracy     (const vector< vector<u32> >& confusionMatrix);
f64  precision    (const vector< vector<u32> >& confusionMatrix);
f64  recall       (const vector< vector<u32> >& confusionMatrix);


}    // namespace ml
}    // namespace rho


#endif   // __rho_ml_common_h__
