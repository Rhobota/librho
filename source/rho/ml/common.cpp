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

tIO examplify(u32 highDimension, u32 numDimensions)
{
    if (highDimension >= numDimensions)
        throw eInvalidArgument("highDimension must be < numDimensions");
    tIO target(numDimensions, 0.0);
    target[highDimension] = 1.0;
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

f64 standardSquaredError(const std::vector<tIO>& outputs,
                         const std::vector<tIO>& targets)
{
    if (outputs.size() != targets.size())
    {
        throw eInvalidArgument("The number of examples in outputs and targets must "
                "be the same!");
    }

    if (outputs.size() == 0)
    {
        throw eInvalidArgument("There must be at least one output/target pair!");
    }

    f64 error = 0.0;
    for (size_t i = 0; i < outputs.size(); i++)
        error += standardSquaredError(outputs[i], targets[i]);
    return error / ((f64)outputs.size());
}


void buildConfusionMatrix(const std::vector<tIO>& outputs,
                          const std::vector<tIO>& targets,
                                tConfusionMatrix& confusionMatrix)
{
    if (outputs.size() != targets.size())
    {
        throw eInvalidArgument("The number of examples in outputs and targets must "
                "be the same!");
    }

    if (outputs.size() == 0)
    {
        throw eInvalidArgument("There must be at least one output/target pair!");
    }

    for (size_t i = 0; i < outputs.size(); i++)
    {
        if (outputs[i].size() != targets[i].size() ||
            outputs[i].size() != outputs[0].size())
        {
            throw eInvalidArgument("Every output/target pair must have the same dimensionality!");
        }
    }

    confusionMatrix.resize(targets[0].size());
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        confusionMatrix[i] = std::vector<u32>(outputs[0].size(), 0);

    for (size_t i = 0; i < outputs.size(); i++)
    {
        u32 target = un_examplify(targets[i]);
        u32 output = un_examplify(outputs[i]);
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


bool train(iLearner* learner, const std::vector<tIO>& inputs,
                              const std::vector<tIO>& targets,
                                    std::vector<tIO>& outputs,  // <-- populated during training
                              u32 batchSize,
                              train_didUpdate_callback callback,
                              void* callbackContext)
{
    if (inputs.size() != targets.size())
    {
        throw eInvalidArgument("The number of examples in inputs and targets must "
                "be the same!");
    }

    if (inputs.size() == 0)
    {
        throw eInvalidArgument("There must be at least one input/target pair!");
    }

    if (batchSize == 0)
    {
        throw eInvalidArgument("batchSize must be positive!");
    }

    outputs.resize(inputs.size());

    u32 batchCounter = 0;

    for (size_t i = 0; i < inputs.size(); i++)
    {
        learner->addExample(inputs[i], targets[i], outputs[i]);
        batchCounter++;
        if (batchCounter == batchSize)
        {
            learner->update();
            if (callback && !callback(learner, callbackContext))
                return false;
            batchCounter = 0;
        }
    }
    if (batchCounter > 0)
    {
        learner->update();
        if (callback && !callback(learner, callbackContext))
            return false;
    }

    return true;
}

void evaluate(iLearner* learner, const std::vector<tIO>& inputs,
                                       std::vector<tIO>& outputs)
{
    if (inputs.size() == 0)
    {
        throw eInvalidArgument("There must be at least one input example!");
    }
    outputs.resize(inputs.size());
    for (size_t i = 0; i < inputs.size(); i++)
    {
        learner->evaluate(inputs[i], outputs[i]);
    }
}


}   // namespace ml
}   // namespace rho
