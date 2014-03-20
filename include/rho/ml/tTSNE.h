#ifndef __rho_ml_tTSNE_h__
#define __rho_ml_tTSNE_h__


#include <rho/ml/common.h>

#include <vector>


namespace rho
{
namespace ml
{


/**
 * Runs the t-SNE algorithm developed by L.J.P. van der Maaten and G.E. Hinton.
 *
 * This algorithm is for visualizing high-dimensional data in a low-dimensional
 * space. This is also known as "embedding".
 *
 * This implementation always reduces the original data to two dimensions.
 *
 * More specifically, this function uses the "Barnes-Hut-SNE" algorithm, which
 * is an approximation version of the original t-SNE algorithm, but scales much
 * better to many data points. Even more specifically, this function uses the
 * implementation of Barnes-Hut-SNE which is provided by the authors under a
 * BSD license. This function merely wraps that implementation. The authors'
 * original code (and publications) are available on their website at:
 *       http://homepage.tudelft.nl/19j49/t-SNE.html
 *
 * Pass your high-dimensional data in as 'originalData'.
 *
 * 'theta' sets the accuracy of the calculates. There is a trade-off between speed
 * and accuracy. A theta of 0.0 corresponds to the standard t-SNE algorithm. A theta
 * of 1.0 causes very terrible calculations. The authors suggest a theta in [0.1, 0.7].
 *
 * 'perplexity' has something to do with the number of neighbors that are considered
 * while clustering. The authors say that if your dataset is large and/or dense, you
 * need a larger perplexity. They suggest keeping it in [5, 50].
 *
 * 'reducedDimData' is an output parameter, which will contain the same number of
 * data points on return as is in 'originalData', but each will be reduced to a lower
 * number of dimensions.
 */
void tsne(const std::vector<tIO>& originalData, f64 theta, f64 perplexity,
                std::vector<tIO>& reducedDimData);


}  // namespace ml
}  // namespace rho


#endif   // __rho_ml_tTSNE_h__
