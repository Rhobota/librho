#if __linux__
#pragma GCC optimize 3
#endif

#include <rho/ml/common.h>
#include <rho/img/tCanvas.h>

#include <rho/ml/tANN.h>
#include <rho/ml/tCNN.h>

#include <cassert>
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

f64 inverse_of_logistic_function(f64 y)
{
    if (y < 0.0001) y = 0.0001;
    if (y > 0.9999) y = 0.9999;
    return -std::log((1.0 / y) - 1.0);
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

f64 inverse_of_hyperbolic_function(f64 y)
{
    if (y < -1.71589) y = -1.71589;
    if (y > 1.71589) y = 1.71589;
    return 1.5 * atanh(0.582785 * y);
}

f64 hyperbolic_function_min()
{
    return -1.7159;
}

f64 hyperbolic_function_max()
{
    return 1.7159;
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

void un_examplify(const tIO& io, bool color, u32 width,
                  bool absolute, img::tImage* dest,
                  const f64* minValue, const f64* maxValue)
{
    if (io.size() == 0)
        throw eInvalidArgument("The example io must have at least one dimension!");

    // Create a copy of io that can be modified.
    std::vector<f64> weights = io;

    // Normalize the weights to [0.0, 255.0].
    f64 maxval;
    f64 minval;
    if (minValue && maxValue)
    {
        maxval = *maxValue;
        minval = *minValue;
        if (minval > maxval)
            throw eInvalidArgument("The minValue must be less than or equal to the maxValue.");
        for (u32 i = 0; i < weights.size(); i++)
            if (weights[i] < minval || weights[i] > maxval)
                throw eInvalidArgument("The minValue and maxValue cannot be true given this input vector.");
    }
    else
    {
        maxval = weights[0];
        minval = weights[0];
        for (u32 i = 1; i < weights.size(); i++)
        {
            maxval = std::max(maxval, weights[i]);
            minval = std::min(minval, weights[i]);
        }
        if (maxval == minval) maxval += 0.000001;
    }
    f64 absmax = std::max(std::fabs(maxval), std::fabs(minval));
    if (color)
    {
        if (absolute)
        {
            for (u32 i = 0; i < weights.size(); i++)
                weights[i] = (std::fabs(weights[i]) / absmax) * 255.0;
        }
        else
        {
            for (u32 i = 0; i < weights.size(); i++)
            {
                f64 val = ((weights[i] - minval) / (maxval - minval)) * 255.0;
                weights[i] = val;
            }
        }
    }

    // Calculate some stuff.
    u32 pixWidth = color ? 3 : 1;
    if ((weights.size() % pixWidth) > 0)
        throw eLogicError("Pixels do not align with the number of weights.");
    u32 numPix = (u32) weights.size() / pixWidth;
    if ((numPix % width) > 0)
        throw eLogicError("Cannot build image of that width. Last row not filled.");
    u32 height = numPix / width;

    // Create the image.
    dest->setFormat(img::kRGB24);
    dest->setBufSize(width*height*3);
    dest->setBufUsed(width*height*3);
    dest->setWidth(width);
    dest->setHeight(height);
    u8* buf = dest->buf();
    u32 bufIndex = 0;
    u32 wIndex = 0;
    for (u32 i = 0; i < height; i++)
    {
        for (u32 j = 0; j < width; j++)
        {
            if (color)
            {
                buf[bufIndex++] = (u8) weights[wIndex++];
                buf[bufIndex++] = (u8) weights[wIndex++];
                buf[bufIndex++] = (u8) weights[wIndex++];
            }
            else
            {
                u8 r = 0;     // <-- used if the weight is negative
                u8 g = 0;     // <-- used if the weight is positive
                u8 b = 0;     // <-- not used

                f64 w = weights[wIndex++];

                if (w > 0.0)
                    g = (u8)(w / absmax * 255.0);

                if (w < 0.0)
                    r = (u8)(-w / absmax * 255.0);

                buf[bufIndex++] = r;
                buf[bufIndex++] = g;
                buf[bufIndex++] = b;
            }
        }
    }
}

void zscore(std::vector<tIO>& trainingInputs, std::vector<tIO>& testInputs)
{
    f64 mean = algo::mean(trainingInputs);
    f64 stddev = algo::stddev(trainingInputs);

    for (size_t i = 0; i < trainingInputs.size(); i++)
        for (size_t j = 0; j < trainingInputs[i].size(); j++)
            trainingInputs[i][j] = (trainingInputs[i][j]-mean) / stddev;

    for (size_t i = 0; i < testInputs.size(); i++)
        for (size_t j = 0; j < testInputs[i].size(); j++)
            testInputs[i][j] = (testInputs[i][j]-mean) / stddev;
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

    for (size_t i = 0; i < outputs.size(); i++)
    {
        if (outputs[i].size() != targets[i].size() ||
            outputs[i].size() != outputs[0].size())
        {
            throw eInvalidArgument("Every output/target pair must have the same dimensionality!");
        }
    }

    if (outputs[0].size() == 0)
    {
        throw eInvalidArgument("The output/target pairs must have at least one dimension!");
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

    if (outputs[0].size() == 0)
    {
        throw eInvalidArgument("The output/target pairs must have at least one dimension!");
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
void s_drawGrid(img::tCanvas& canvas, u32 gridSize, u32 distBetweenLines)
{
    {
        img::tImage horiz;
        horiz.setFormat(img::kRGB24);
        horiz.setWidth(gridSize*distBetweenLines);
        horiz.setHeight(1);
        horiz.setBufSize(horiz.width() * horiz.height() * 3);
        horiz.setBufUsed(horiz.bufSize());
        for (u32 i = 0; i < horiz.bufUsed(); i++) horiz.buf()[i] = 255;
        for (u32 i = 0; i <= gridSize; i++)
            canvas.drawImage(&horiz, 0, i*distBetweenLines);
    }

    {
        img::tImage vert;
        vert.setFormat(img::kRGB24);
        vert.setWidth(1);
        vert.setHeight(gridSize*distBetweenLines);
        vert.setBufSize(vert.width() * vert.height() * 3);
        vert.setBufUsed(vert.bufSize());
        for (u32 i = 0; i < vert.bufUsed(); i++) vert.buf()[i] = 255;
        for (u32 i = 0; i <= gridSize; i++)
            canvas.drawImage(&vert, i*distBetweenLines, 0);
    }
}

void buildVisualConfusionMatrix(const std::vector<tIO>& inputs,
                                bool color, u32 width, bool absolute,
                                const std::vector<tIO>& outputs,
                                const std::vector<tIO>& targets,
                                      img::tImage* dest)
{
    if (outputs.size() != targets.size())
    {
        throw eInvalidArgument("The number of examples in outputs and targets must "
                "be the same!");
    }

    if (outputs.size() != inputs.size())
    {
        throw eInvalidArgument("The number of examples in outputs and inputs must "
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

    if (outputs[0].size() == 0)
    {
        throw eInvalidArgument("The output/target pairs must have at least one dimension!");
    }

    u32 numClasses = (u32) targets[0].size();      // same as outputs[0].size()
    u32 boxWidth = 5 * width;

    u8 bgColor[3] = { 0, 0, 205 };    // "Medium Blue" from http://www.tayloredmktg.com/rgb/
    img::tCanvas canvas(img::kRGB24, bgColor, 3);

    algo::tKnuthLCG lcg;

    img::tImage image;

    for (size_t i = 0; i < outputs.size(); i++)
    {
        u32 target = un_examplify(targets[i]);
        u32 output = un_examplify(outputs[i]);

        f64 rx = ((f64)lcg.next()) / ((f64)lcg.randMax()) * (boxWidth-width) + output*boxWidth;
        f64 ry = ((f64)lcg.next()) / ((f64)lcg.randMax()) * (boxWidth-width) + target*boxWidth;

        un_examplify(inputs[i], color, width, absolute, &image);

        canvas.drawImage(&image, (i32) round(rx), (i32) round(ry));
    }

    canvas.expandToIncludePoint(0, 0);
    canvas.expandToIncludePoint(numClasses*boxWidth, numClasses*boxWidth);
    s_drawGrid(canvas, numClasses, boxWidth);
    canvas.genImage(dest);
}

static
void checkConfusionMatrix(const tConfusionMatrix& confusionMatrix)
{
    if (confusionMatrix.size() == 0)
        throw eInvalidArgument("Invalid confusion matrix");

    for (size_t i = 0; i < confusionMatrix.size(); i++)
    {
        if (confusionMatrix[i].size() != confusionMatrix.size())
            throw eInvalidArgument("Invalid confusion matrix");
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

void print(const tConfusionMatrix& confusionMatrix, std::ostream& out)
{
    checkConfusionMatrix(confusionMatrix);

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

f64  errorRate(const tConfusionMatrix& confusionMatrix)
{
    checkConfusionMatrix(confusionMatrix);

    u32 total = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        for (size_t j = 0; j < confusionMatrix[i].size(); j++)
            total += confusionMatrix[i][j];
    u32 correct = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        correct += confusionMatrix[i][i];
    return ((f64)(total - correct)) / total;
}

f64  accuracy(const tConfusionMatrix& confusionMatrix)
{
    checkConfusionMatrix(confusionMatrix);

    u32 total = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        for (size_t j = 0; j < confusionMatrix[i].size(); j++)
            total += confusionMatrix[i][j];
    u32 correct = 0;
    for (size_t i = 0; i < confusionMatrix.size(); i++)
        correct += confusionMatrix[i][i];
    return ((f64)correct) / total;
}

f64  precision(const tConfusionMatrix& confusionMatrix)
{
    checkConfusionMatrix(confusionMatrix);

    if (confusionMatrix.size() != 2)
        throw eInvalidArgument("Precision is only defined for boolean classification.");

    f64 tp = (f64) confusionMatrix[1][1];
    f64 fp = (f64) confusionMatrix[0][1];
    //f64 tn = (f64) confusionMatrix[0][0];
    //f64 fn = (f64) confusionMatrix[1][0];

    return tp / (tp + fp);
}

f64  recall(const tConfusionMatrix& confusionMatrix)
{
    checkConfusionMatrix(confusionMatrix);

    if (confusionMatrix.size() != 2)
        throw eInvalidArgument("Recall is only defined for boolean classification.");

    f64 tp = (f64) confusionMatrix[1][1];
    //f64 fp = (f64) confusionMatrix[0][1];
    //f64 tn = (f64) confusionMatrix[0][0];
    f64 fn = (f64) confusionMatrix[1][0];

    return tp / (tp + fn);
}


bool train(iLearner* learner, const std::vector<tIO>& inputs,
                              const std::vector<tIO>& targets,
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

    for (size_t i = 1; i < inputs.size(); i++)
    {
        if (inputs[i].size() != inputs[0].size())
        {
            throw eInvalidArgument("Every input must have the same dimensionality!");
        }
    }

    for (size_t i = 1; i < targets.size(); i++)
    {
        if (targets[i].size() != targets[0].size())
        {
            throw eInvalidArgument("Every target must have the same dimensionality!");
        }
    }

    if (batchSize == 0)
    {
        throw eInvalidArgument("batchSize must be positive!");
    }

    std::vector<tIO> mostRecentBatch(batchSize);
    u32 batchCounter = 0;
    tIO output;

    for (size_t i = 0; i < inputs.size(); i++)
    {
        learner->addExample(inputs[i], targets[i], output);
        mostRecentBatch[batchCounter] = inputs[i];
        batchCounter++;
        if (batchCounter == batchSize)
        {
            learner->update();
            if (callback && !callback(learner, mostRecentBatch, callbackContext))
                return false;
            batchCounter = 0;
        }
    }
    if (batchCounter > 0)
    {
        learner->update();
        mostRecentBatch.resize(batchCounter);
        if (callback && !callback(learner, mostRecentBatch, callbackContext))
            return false;
    }

    return true;
}

void evaluate(iLearner* learner, const std::vector<tIO>& inputs,
                                       std::vector<tIO>& outputs)
{
    if (inputs.size() == 0)
    {
        throw eInvalidArgument("There must be at least one input vector!");
    }
    outputs.resize(inputs.size());
    for (size_t i = 0; i < inputs.size(); i++)
    {
        learner->evaluate(inputs[i], outputs[i]);
    }
}

/*
 * This function is used "deinterlace" ("deinterleave" is the correct
 * term, actually) a vector of repeating component.
 *
 * For example, say you have a vector with the contents: a1b2c3d4
 * And you want to convert that to a vector: abcd1234
 * To do that call this function with numComponents=2 and unitLength=1.
 *
 * Or, say you have a vector with the contents: ab12cd34ef56
 * And you want to convert that to a vector: abcdef123456
 * To do that call this function with numComponents=2 and unitLength=2.
 *
 * Or, say you have a vector with the contents: RGBRGBRGBRGB
 * And you want to convert that to a vector: RRRRGGGGBBBB
 * To do that call this function with numComponents=3 and unitLength=1.
 *
 * 'output' must be allocated by the caller, and of course delete by
 * the caller as well.
 */
template <class T>
void deinterlace(const T* input, T* output, u32 arrayLen, u32 numComponents, u32 unitLength=1)
{
    assert((arrayLen % unitLength) == 0);
    u32 numUnits = arrayLen / unitLength;

    assert((numUnits % numComponents) == 0);
    u32 groupSize = numUnits / numComponents;

    u32 stride = groupSize * unitLength;

    u32 s = 0;

    for (u32 g = 0; g < groupSize; g++)
    {
        u32 d = g * unitLength;

        for (u32 c = 0; c < numComponents; c++)
        {
            for (u32 u = 0; u < unitLength; u++)
                output[d+u] = input[s+u];

            s += unitLength;
            d += stride;
        }
    }
}

/*
 * This function is used to re-order the components of a CNN hidden layer weight
 * image. The issue is that in the image, weights to each of the previous receptive
 * fields are interlaced together, but when we view that image we'd rather see
 * the weights to the same previous receptive field together. So this function
 * fixes that.
 */
static
void s_fixHiddenLayerImage(img::tImage& image, u32 numComponents)
{
    u32 width = 0;
    u32 unitLength = 0;

    switch (image.format())
    {
        case img::kRGB24:
            width = image.width() * 3;
            unitLength = 3;
            break;

        case img::kGrey:
            width = image.width();
            unitLength = 1;
            break;

        default:
            assert(false);
            break;
    }

    u8* temp = new u8[width];

    for (u32 h = 0; h < image.height(); h++)
    {
        u8* row = image[h][0];
        deinterlace(row, temp, width, numComponents, unitLength);
        for (u32 i = 0; i < width; i++)
            row[i] = temp[i];
    }

    delete [] temp;
}

static
void s_drawLayer(const ml::tCNN* cnn, u32 layerIndex,
                 bool color, bool absolute,
                 u32& currX, img::tCanvas& canvas)
{
    const u32 padding = 10;
    const u32 weigthImageScale = 5;

    u32 currY = padding;

    for (u32 n = 0; n < cnn->getNumFeatureMaps(layerIndex); n++)
    {
        u32 heightHere = 0;
        u32 xHere = currX;

        if (cnn->isLayerFullyConnected(layerIndex))
        {
            {
                img::tImage wImage;
                cnn->getFeatureMapImage(layerIndex, n, layerIndex == 0 ? color : false, absolute, &wImage);
                if (layerIndex > 0)
                    s_fixHiddenLayerImage(wImage, cnn->getNumFeatureMaps(layerIndex-1));
                img::tImage wImageScaled;
                wImage.scale(wImage.width()*weigthImageScale, wImage.height()*weigthImageScale, &wImageScaled);
                canvas.drawImage(&wImageScaled, xHere, currY);
                heightHere = wImageScaled.height();
                xHere += wImageScaled.width();
            }
            {
                xHere += padding;
                img::tImage image;
                image.setFormat(img::kRGB24); image.setWidth(padding*2); image.setHeight(heightHere);
                image.setBufSize(image.width() * image.height() * 3);
                image.setBufUsed(image.bufSize());
                f64 minpossible, maxpossible;
                f64 val = cnn->getOutput(layerIndex, n, 0, &minpossible, &maxpossible);
                for (u32 i = 0; i < image.bufUsed(); i += 3)
                {
                    u8 r = 0;   // <-- used if val is negative
                    u8 g = 0;   // <-- used if val is positive
                    u8 b = 0;   // <-- not used
                    if (val < 0.0)
                        r = (u8) (val / minpossible * 255.0);
                    else if (val > 0.0)
                        g = (u8) (val / maxpossible * 255.0);
                    image.buf()[i+0] = r;
                    image.buf()[i+1] = g;
                    image.buf()[i+2] = b;
                }
                canvas.drawImage(&image, xHere, currY);
                xHere += image.width();
            }
        }

        else
        {
            {
                img::tImage wImage;
                cnn->getFeatureMapImage(layerIndex, n, layerIndex == 0 ? color : false, absolute, &wImage);
                if (layerIndex > 0)
                    s_fixHiddenLayerImage(wImage, cnn->getNumFeatureMaps(layerIndex-1));
                img::tImage wImageScaled;
                wImage.scale(wImage.width()*weigthImageScale, wImage.height()*weigthImageScale, &wImageScaled);
                canvas.drawImage(&wImageScaled, xHere, currY);
                heightHere = std::max(heightHere, wImageScaled.height());
                xHere += wImageScaled.width();
            }
            {
                xHere += padding;
                img::tImage layerImage;
                cnn->getOutputImage(layerIndex, n, false, &layerImage);   // <-- false for "not pooled"
                canvas.drawImage(&layerImage, xHere, currY);
                heightHere = std::max(heightHere, layerImage.height());
                xHere += layerImage.width();
            }
            if (cnn->isLayerPooled(layerIndex))
            {
                xHere += padding;
                img::tImage layerImage;
                cnn->getOutputImage(layerIndex, n, true, &layerImage);   // <-- true for "pooled"
                canvas.drawImage(&layerImage, xHere, currY);
                heightHere = std::max(heightHere, layerImage.height());
                xHere += layerImage.width();
            }
        }

        currY += heightHere + padding;
        if (n+1 == cnn->getNumFeatureMaps(layerIndex))
            currX = xHere;

        canvas.expandToIncludePoint(currX, currY);
    }
}

void visualize(iLearner* learner, const tIO& example,
               bool color, u32 width, bool absolute,
               img::tImage* dest)
{
    tANN* ann = dynamic_cast<tANN*>(learner);
    tCNN* cnn = dynamic_cast<tCNN*>(learner);

    if (ann)
    {
        tIO output;
        ann->evaluate(example, output);

        u8 bgColor[3] = { 0, 0, 205 };    // "Medium Blue" from http://www.tayloredmktg.com/rgb/
        img::tCanvas canvas(img::kRGB24, bgColor, 3);

        u32 numHoriz = (u32) std::ceil(std::sqrt(3.0 * ann->getNumNeuronsInLayer(0)));
        if (numHoriz < 2) numHoriz = 2;
        u32 horizCount = 0;
        u32 currWidth = 0;
        u32 currHeight = 0;

        img::tImage image;
        ml::un_examplify(example, color, width, absolute, &image);
        canvas.drawImage(&image, currWidth, currHeight);
        horizCount++;
        currWidth += image.width();

        for (u32 n = 0; n < ann->getNumNeuronsInLayer(0); n++)
        {
            ann->getImage(0, n, color, width, absolute, &image);
            canvas.drawImage(&image, currWidth, currHeight);
            currWidth += image.width();
            if (++horizCount == numHoriz)
            {
                horizCount = 0;
                currWidth = 0;
                currHeight += image.height();
            }
        }

        canvas.genImage(dest);
    }

    else if (cnn)
    {
        tIO output;
        cnn->evaluate(example, output);

        u8 bgColor[3] = { 0, 0, 205 };    // "Medium Blue" from http://www.tayloredmktg.com/rgb/
        img::tCanvas canvas(img::kRGB24, bgColor, 3);

        const u32 padding = 50;
        u32 currX = 10;

        img::tImage exampleImage;
        ml::un_examplify(example, color, width, absolute, &exampleImage);
        canvas.drawImage(&exampleImage, currX, currX);
        currX += exampleImage.width() + padding;

        for (u32 i = 0; i < cnn->getNumLayers(); i++)
        {
            s_drawLayer(cnn, i, color, absolute, currX, canvas);
            currX += padding;
        }

        canvas.expandToIncludePoint(0, 0);
        canvas.expandToIncludePoint(currX, 0);
        canvas.genImage(dest);
    }

    else
    {
        throw eNotImplemented("Is there some type of learner that I don't know how to draw?");
    }
}


}   // namespace ml
}   // namespace rho
