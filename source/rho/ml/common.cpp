#include <rho/ml/common.h>


namespace rho
{
namespace ml
{


f64 logistic_function(f64 z)
{
    return (1.0 / (1.0 + std::exp(-z)));
}

f64 derivative_of_logistic_function(f64 z)
{
    f64 y = logistic_function(z);
    f64 slope = (y * (1.0 - y));
    slope = std::max(slope, 1e-5);    // <-- Experimental
    return slope;
}

f64 logistic_function_min()
{
    return 0.0;
}

f64 logistic_function_max()
{
    return 1.0;
}


tIO examplify(const img::tImage* image)
{
    if (image->bufUsed() == 0)
        throw eInvalidArgument("The example image must have at least one pixel in it!");
    const u8* buf = image->buf();
    tIO input(image->bufUsed(), 0.0);
    for (u32 i = 0; i < image->bufUsed(); i++)
        input[i] = buf[i] / 255.0;
    return input;
}

tIO examplify(u32 val, u32 vectorSize)
{
    if (val >= vectorSize)
        throw eInvalidArgument("val must be < vectorSize");
    tIO target(vectorSize, 0.0);
    target[val] = 1.0;
    return target;
}

u32 un_examplify(const tIO& output, f64* error)
{
    if (output.size() == 0)
        throw eInvalidArgument("The output vector must have at least one dimension!");
    u32 maxindex = 0;
    for (size_t i = 1; i < output.size(); i++)
        if (output[i] > output[maxindex])
            maxindex = (u32)i;
    if (error)
        *error = standardSquaredError(output, examplify(maxindex, (u32)output.size()));
    return maxindex;
}


f64 standardSquaredError(const tIO& output, const tIO& target)
{
    if (output.size() != target.size())
        throw eInvalidArgument(
                "The output vector must have the same dimensionality as the target vector!");
    if (output.size() == 0)
        throw eInvalidArgument("The output and target vectors must have at least one dimension!");
    f64 error = 0.0;
    for (size_t i = 0; i < output.size(); i++)
        error += (output[i]-target[i]) * (output[i]-target[i]);
    return 0.5*error;
}

f64 standardSquaredError(const std::vector<tIO>& outputExamples,
                         const std::vector<tIO>& targetExamples)
{
    if (outputExamples.size() != targetExamples.size())
    {
        throw eInvalidArgument("The number of examples in outputExamples and targetExamples must "
                "be the same!");
    }

    if (outputExamples.size() == 0)
    {
        throw eInvalidArgument("There must be at least one output/target pair!");
    }

    f64 error = 0.0;
    for (size_t i = 0; i < outputExamples.size(); i++)
        error += standardSquaredError(outputExamples[i], targetExamples[i]);
    return error;
}


void buildConfusionMatrix(const std::vector<tIO>& outputExamples,
                          const std::vector<tIO>& targetExamples,
                                tConfusionMatrix& confusionMatrix)
{
    if (outputExamples.size() != targetExamples.size())
    {
        throw eInvalidArgument("The number of examples in outputExamples and targetExamples must "
                "be the same!");
    }

    if (outputExamples.size() == 0)
    {
        throw eInvalidArgument("There must be at least one output/target pair!");
    }

    for (size_t i = 0; i < outputExamples.size(); i++)
    {
        if (outputExamples[i].size() != targetExamples[i].size() ||
            outputExamples[i].size() != outputExamples[0].size())
        {
            throw eInvalidArgument("Every output/target pair must have the same dimensionality!");
        }
    }

    confusionMatrix.resize(outputExamples[0].size());
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        confusionMatrix[i] = std::vector<u32>(targetExamples[0].size(), 0);

    for (size_t i = 0; i < outputExamples.size(); i++)
    {
        u32 output = un_examplify(outputExamples[i]);
        u32 target = un_examplify(targetExamples[i]);
        confusionMatrix[output][target]++;
    }
}

void print        (const tConfusionMatrix& confusionMatrix, std::ostream& out)
{
    out << "rho::ml::print() not implemented" << std::endl;
    throw eNotImplemented("I'm lazy right now and I don't actually need this yet.");
}

f64  errorRate    (const tConfusionMatrix& confusionMatrix)
{
    u32 total = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        for (size_t j = 0; j < confusionMatrix[i].size(); j++)
            total += confusionMatrix[i][j];
    u32 correct = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        correct += confusionMatrix[i][i];
    return ((f64)(total - correct)) / total;
}

f64  accuracy     (const tConfusionMatrix& confusionMatrix)
{
    u32 total = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        for (size_t j = 0; j < confusionMatrix[i].size(); j++)
            total += confusionMatrix[i][j];
    u32 correct = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        correct += confusionMatrix[i][i];
    return ((f64)correct) / total;
}

f64  precision    (const tConfusionMatrix& confusionMatrix)
{
    throw eNotImplemented("I'm lazy right now and I don't actually need this yet.");
}

f64  recall       (const tConfusionMatrix& confusionMatrix)
{
    throw eNotImplemented("I'm lazy right now and I don't actually need this yet.");
}


}   // namespace ml
}   // namespace rho
