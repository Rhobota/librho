#include <rho/ml/tTSNE.h>

#include "bh_tsne/tsne.h"


namespace rho
{
namespace ml
{


void tsne(const std::vector<tIO>& originalData, f64 theta, f64 perplexity,
                std::vector<tIO>& reducedDimData)
{
    // Verify that the input is reasonable.
    if (originalData.size() == 0)
        throw eInvalidArgument("There must be at least one original data point.");
    for (size_t i = 1; i < originalData.size(); i++)
        if (originalData[i].size() != originalData[0].size())
            throw eInvalidArgument("All data points must have the same dimensionality.");
    if (originalData[0].size() == 0)
        throw eInvalidArgument("The dimensionality of the data points must be non-zero.");
    if (theta < 0.0 || theta > 1.0)
        throw eInvalidArgument("Theta must be in the range [0.0, 1.0].");

    // Setup the state that the t-SNE algo needs.
    int N = (int) originalData.size();
    int D = (int) originalData[0].size();
    int redDims = 2;
    double* data = (double*) malloc(N * D * sizeof(double));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < D; j++)
            data[i*D + j] = originalData[i][j];
    }
    double* Y = (double*) malloc(N * redDims * sizeof(double));

    // Run t-SNE.
    TSNE tsneobj;
    tsneobj.run(data, N, D, Y, redDims, perplexity, theta);
    free(data); data = NULL;

    // Copy the results out.
    reducedDimData.resize(N);
    for (int i = 0; i < N; i++)
    {
        reducedDimData[i].resize(redDims);
        for (int j = 0; j < redDims; j++)
            reducedDimData[i][j] = Y[i*redDims + j];
    }
    free(Y); Y = NULL;
}


}  // namespace ml
}  // namespace rho
