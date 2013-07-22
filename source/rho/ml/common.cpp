#include <rho/ml/common.h>

#include <iostream>
#include <iomanip>


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


f64 hyperbolic_function(f64 z)
{
    // Recommended by: "Efficient BackProp" (LeCun et al.)
    return 1.7159 * std::tanh(2.0/3.0 * z);
}

f64 derivative_of_hyperbolic_function(f64 z)
{
    f64 s = 1.0 / std::cosh(2.0/3.0 * z);
    return 1.14393 * s * s;
}

f64 hyperbolic_function_min()
{
    return -1.7159;
}

f64 hyperbolic_function_max()
{
    return 1.7159;
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
    return error / ((f64)outputExamples.size());
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

    confusionMatrix.resize(targetExamples[0].size());
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        confusionMatrix[i] = std::vector<u32>(outputExamples[0].size(), 0);

    for (size_t i = 0; i < outputExamples.size(); i++)
    {
        u32 target = un_examplify(targetExamples[i]);
        u32 output = un_examplify(outputExamples[i]);
        confusionMatrix[target][output]++;
    }
}

static
void printDashes(const tConfusionMatrix& confusionMatrix, std::ostream& out, u32 s, u32 w)
{
    for (u32 i = 1; i < s; i++)
        out << " ";
    out << "+";
    for (size_t j = 0; j < confusionMatrix[0].size(); j++)
    {
        for (u32 i = 1; i < w; i++)
            out << "-";
        out << "+";
    }
    out << std::endl;
}

void print        (const tConfusionMatrix& confusionMatrix, std::ostream& out)
{
    if (confusionMatrix.size() == 0)
        throw eInvalidArgument("Invalid confusion matrix");

    u32 s = 14;
    u32 w = 10;

    out << "                   predicted" << std::endl;

    printDashes(confusionMatrix, out, s, w);

    for (size_t i = 0; i < confusionMatrix.size(); i++)
    {
        if (i == confusionMatrix.size()/2)
            out << "  correct    |";
        else
            out << "             |";
        for (size_t j = 0; j < confusionMatrix[i].size(); j++)
        {
            out << " " << std::right << std::setw(w-3) << confusionMatrix[i][j] << " |";
        }
        out << std::endl;
    }

    printDashes(confusionMatrix, out, s, w);

    out << std::endl;
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
    if (confusionMatrix.size() != 2)
        throw eInvalidArgument("Precision is only defined for boolean classification.");
    if (confusionMatrix[0].size() != 2 || confusionMatrix[1].size() != 2)
        throw eInvalidArgument("Precision is only defined for boolean classification.");

    f64 tp = (f64) confusionMatrix[1][1];
    f64 fp = (f64) confusionMatrix[0][1];
    //f64 tn = (f64) confusionMatrix[0][0];
    //f64 fn = (f64) confusionMatrix[1][0];

    return tp / (tp + fp);
}

f64  recall       (const tConfusionMatrix& confusionMatrix)
{
    if (confusionMatrix.size() != 2)
        throw eInvalidArgument("Recall is only defined for boolean classification.");
    if (confusionMatrix[0].size() != 2 || confusionMatrix[1].size() != 2)
        throw eInvalidArgument("Recall is only defined for boolean classification.");

    f64 tp = (f64) confusionMatrix[1][1];
    //f64 fp = (f64) confusionMatrix[0][1];
    //f64 tn = (f64) confusionMatrix[0][0];
    f64 fn = (f64) confusionMatrix[1][0];

    return tp / (tp + fn);
}


}   // namespace ml
}   // namespace rho
