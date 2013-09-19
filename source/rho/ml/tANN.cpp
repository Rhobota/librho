#include <rho/ml/tANN.h>
#include <rho/algo/tLCG.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>


using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;


namespace rho
{
namespace ml
{


static
string layerTypeToString(tANN::nLayerType type)
{
    switch (type)
    {
        case tANN::kLayerTypeLogistic:
            return "logistic";
        case tANN::kLayerTypeHyperbolic:
            return "hyperbolic";
        case tANN::kLayerTypeSoftmax:
            return "softmax";
        default:
            throw eInvalidArgument("Invalid layer type");
    }
}


static
char layerTypeToChar(tANN::nLayerType type)
{
    switch (type)
    {
        case tANN::kLayerTypeLogistic:
            return 'l';
        case tANN::kLayerTypeHyperbolic:
            return 'h';
        case tANN::kLayerTypeSoftmax:
            return 's';
        default:
            throw eInvalidArgument("Invalid layer type");
    }
}


static
f64 squash(f64 val, tANN::nLayerType type)
{
    switch (type)
    {
        case tANN::kLayerTypeLogistic:
            return logistic_function(val);
        case tANN::kLayerTypeHyperbolic:
            return hyperbolic_function(val);
        case tANN::kLayerTypeSoftmax:
            // Softmax layers must be handled specially
        default:
            throw eInvalidArgument("Invalid layer type");
    }
}


static
f64 derivative_of_squash(f64 val, tANN::nLayerType type)
{
    switch (type)
    {
        case tANN::kLayerTypeLogistic:
            return derivative_of_logistic_function(val);
        case tANN::kLayerTypeHyperbolic:
            return derivative_of_hyperbolic_function(val);
        case tANN::kLayerTypeSoftmax:
            // Softmax layers must be handled specially
        default:
            throw eInvalidArgument("Invalid layer type");
    }
}


static
f64 squash_min(tANN::nLayerType type)
{
    switch (type)
    {
        case tANN::kLayerTypeLogistic:
            return logistic_function_min();
        case tANN::kLayerTypeHyperbolic:
            return hyperbolic_function_min();
        case tANN::kLayerTypeSoftmax:
            return 0.0;
        default:
            throw eInvalidArgument("Invalid layer type");
    }
}


static
f64 squash_max(tANN::nLayerType type)
{
    switch (type)
    {
        case tANN::kLayerTypeLogistic:
            return logistic_function_max();
        case tANN::kLayerTypeHyperbolic:
            return hyperbolic_function_max();
        case tANN::kLayerTypeSoftmax:
            return 1.0;
        default:
            throw eInvalidArgument("Invalid layer type");
    }
}


class tLayer : public bNonCopyable
{ public:

    vector<f64> a;
    vector<f64> A;

    vector<f64> da;
    vector<f64> dA;

    vector< vector<f64> > w;   // <-- the weights connecting to the layer below
                               //                       (i.e. the previous layer)

    vector< vector<f64> > dw_accum;

    tANN::nLayerType layerType;

    void assertState(size_t prevLayerSize) const
    {
        assert(prevLayerSize+1 == w.size());
        assert(w.size() == dw_accum.size());
        assert(a.size() == A.size());
        assert(a.size() == da.size());
        assert(a.size() == dA.size());

        for (size_t s = 0; s < prevLayerSize+1; s++)
        {
            assert(A.size() == w[s].size());
            assert(w[s].size() == dw_accum[s].size());
        }
    }

    void setSizes(u32 prevSize, u32 mySize)
    {
        a.resize(mySize, 0.0);
        A.resize(mySize, 0.0);
        da.resize(mySize, 0.0);
        dA.resize(mySize, 0.0);
        w.resize(prevSize+1);
        for (u32 s = 0; s < w.size(); s++)
            w[s].resize(mySize, 0.0);
        dw_accum = vector< vector<f64> >(prevSize+1, vector<f64>(mySize, 0.0));
    }

    void randomizeWeights(f64 rmin, f64 rmax, algo::iLCG& lcg)
    {
        for (u32 s = 0; s < w.size(); s++)
        {
            for (u32 i = 0; i < w[s].size(); i++)
            {
                u64 r = lcg.next();
                w[s][i] = ((f64)r) / ((f64)lcg.randMax());    // [0.0, 1.0]
                w[s][i] *= rmax-rmin;                         // [0.0, rmax-rmin]
                w[s][i] += rmin;                              // [rmin, rmax]
            }
        }
    }

    void takeInput(const vector<f64>& input, u8 normalizeLayerInput)
    {
        assertState(input.size());

        for (u32 i = 0; i < A.size(); i++)
            A[i] = 0.0;

        for (u32 s = 0; s < input.size(); s++)
        {
            for (u32 i = 0; i < A.size(); i++)
                A[i] += w[s][i] * input[s];
        }

        for (u32 i = 0; i < A.size(); i++)
            A[i] += w.back()[i] * 1.0;

        if (normalizeLayerInput)
        {
            for (u32 i = 0; i < A.size(); i++)
                A[i] /= (f64) input.size();
        }

        if (layerType == tANN::kLayerTypeSoftmax)
        {
            for (u32 i = 0; i < A.size(); i++)
                a[i] = std::min(std::exp(A[i]), 1e100);
            f64 denom = 0.0;
            for (size_t i = 0; i < a.size(); i++)
                denom += a[i];
            for (size_t i = 0; i < a.size(); i++)
                a[i] /= denom;
        }
        else
        {
            for (u32 i = 0; i < A.size(); i++)
                a[i] = squash(A[i], layerType);
        }
    }

    void accumError(const vector<f64>& prev_a, u8 normalizeLayerInput)
    {
        assertState(prev_a.size());

        if (layerType == tANN::kLayerTypeSoftmax)
        {
            for (u32 i = 0; i < dA.size(); i++)
                dA[i] = da[i];
        }
        else
        {
            for (u32 i = 0; i < dA.size(); i++)
                dA[i] = da[i] * derivative_of_squash(A[i], layerType);
        }

        if (normalizeLayerInput)
        {
            f64 norm = 1.0 / ((f64)prev_a.size());
            for (u32 s = 0; s < prev_a.size(); s++)
                for (u32 i = 0; i < dA.size(); i++)
                    dw_accum[s][i] += dA[i] * prev_a[s] * norm;
            for (u32 i = 0; i < dA.size(); i++)
                dw_accum.back()[i] += dA[i] * 1.0 * norm;
        }
        else
        {
            for (u32 s = 0; s < prev_a.size(); s++)
                for (u32 i = 0; i < dA.size(); i++)
                    dw_accum[s][i] += dA[i] * prev_a[s];
            for (u32 i = 0; i < dA.size(); i++)
                dw_accum.back()[i] += dA[i] * 1.0;
        }
    }

    void backpropagate(vector<f64>& prev_da) const
    {
        assertState(prev_da.size());

        for (u32 s = 0; s < prev_da.size(); s++)
        {
            prev_da[s] = 0.0;

            for (u32 i = 0; i < dA.size(); i++)
                prev_da[s] += w[s][i] * dA[i];
        }
    }

    void updateWeights(f64 alpha)
    {
        assertState(w.size()-1);

        for (u32 s = 0; s < w.size(); s++)
        {
            for (u32 i = 0; i < w[s].size(); i++)
            {
                w[s][i] -= alpha * dw_accum[s][i];
                dw_accum[s][i] = 0.0;
            }
        }
    }

    void pack(iWritable* out) const
    {
        u8 type = (u8) layerType;
        rho::pack(out, type);

        u32 mySize = (u32) a.size();
        u32 prevLayerSize = (u32) w.size()-1;
        rho::pack(out, mySize);
        rho::pack(out, prevLayerSize);
        rho::pack(out, w);
    }

    void unpack(iReadable* in)
    {
        u8 type;
        rho::unpack(in, type);
        layerType = (tANN::nLayerType) type;

        u32 mySize, prevLayerSize;
        rho::unpack(in, mySize);
        rho::unpack(in, prevLayerSize);
        rho::unpack(in, w);

        a = vector<f64>(mySize, 0.0);
        A = vector<f64>(mySize, 0.0);
        da = vector<f64>(mySize, 0.0);
        dA = vector<f64>(mySize, 0.0);
        dw_accum = vector< vector<f64> >(prevLayerSize+1, vector<f64>(mySize, 0.0));
    }
};


tANN::tANN(vector<u32> layerSizes,
           f64 alpha,
           f64 alphaMultiplier,
           bool normalizeLayerInput,
           f64 randWeightMin,
           f64 randWeightMax,
           nLayerType defaultLayerType)
    : m_layers(NULL),
      m_numLayers(0),
      m_alpha(alpha),
      m_alphaMultiplier(alphaMultiplier),
      m_normalizeLayerInput(normalizeLayerInput ? 1 : 0)
{
    if (layerSizes.size() < 2)
        throw eInvalidArgument("There must be at least an input and output layer.");
    if (alpha <= 0.0)
        throw eInvalidArgument("The learning rate must be greater than 0.0.");
    if (alphaMultiplier <= 0.0)
        throw eInvalidArgument("The learning rate multiplier must be greater than 0.0.");
    if (defaultLayerType < 0 || defaultLayerType >= kLayerTypeMax)
        throw eInvalidArgument("Invalid layer type");
    if (defaultLayerType == kLayerTypeSoftmax)
    {
        throw eInvalidArgument("Only the top layer may be a softmax group. Use setLayerType() to "
                "set the top layer to be a softmax group if that is what you want.");
    }

    m_numLayers = (u32) layerSizes.size()-1;
                            // we don't need a layer for the input

    m_layers = new tLayer[m_numLayers];
                            // m_layers[0] is the lowest layer
                            // (i.e. first one above the input layer)

    algo::tKnuthLCG lcg;

    for (u32 i = 1; i < layerSizes.size(); i++)
    {
        m_layers[i-1].layerType = defaultLayerType;
        m_layers[i-1].setSizes(layerSizes[i-1], layerSizes[i]);
        m_layers[i-1].randomizeWeights(randWeightMin, randWeightMax, lcg);
    }
}

tANN::tANN(iReadable* in)
    : m_layers(NULL),
      m_numLayers(0),
      m_alpha(1.0),
      m_alphaMultiplier(1.0),
      m_normalizeLayerInput(1)
{
    this->unpack(in);
}

tANN::~tANN()
{
    delete [] m_layers;
    m_layers = NULL;
    m_numLayers = 0;
}

void tANN::setLayerType(u32 layerIndex, nLayerType type)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (type < 0 || type >= kLayerTypeMax)
        throw eInvalidArgument("Invalid layer type");
    if (type == kLayerTypeSoftmax && layerIndex != m_numLayers-1)
        throw eInvalidArgument("Only the top layer may be a softmax group.");
    m_layers[layerIndex].layerType = type;
}

void tANN::printNetworkInfo(std::ostream& out) const
{
    out << "Artificial Neural Network Info:\n";

    out << "   size (top-to-bottom):";
    for (i32 i = ((i32)m_numLayers-1); i >= 0; i--)
        out << "  " << m_layers[i].a.size() << "(" << layerTypeToString(m_layers[i].layerType) << ")";
    out << "  " << m_layers[0].w.size()-1 << "(input)\n";

    out << "   learning rate:        " << m_alpha << "\n";
    out << "   rate multiplier:      " << m_alphaMultiplier << "\n";
    out << "   normalizes input:     " << (m_normalizeLayerInput ? "yes" : "no") << "\n";

    out << endl;
}

string tANN::networkInfoString() const
{
    std::ostringstream out;

    // Network size:
    out << m_layers[0].w.size()-1 << 'i';
    for (u32 i = 0; i < m_numLayers; i++)
        out << '-' << m_layers[i].a.size() << layerTypeToChar(m_layers[i].layerType);

    // Other network parameters:
    out << "__" << m_alpha << 'a';
    out << "__" << m_alphaMultiplier << 'm';
    out << "__" << (m_normalizeLayerInput ? "norm" : "full");

    return out.str();
}

void tANN::addExample(const tIO& input, const tIO& target,
                      tIO& actualOutput)
{
    evaluate(input, actualOutput);

    if (target.size() != actualOutput.size())
        throw eInvalidArgument("The target vector must be the same size as the ANN's output.");

    nLayerType type = m_layers[m_numLayers-1].layerType;
    for (u32 i = 0; i < target.size(); i++)
        if (target[i] < squash_min(type) || target[i] > squash_max(type))
            throw eInvalidArgument("The target vector must be in the range of the top layer's squashing function.");

    if (type == kLayerTypeSoftmax)
    {
        f64 summation = 0.0;
        for (u32 i = 0; i < target.size(); i++)
            summation += target[i];
        if (summation < 0.9999 || summation > 1.0001)
        {
            throw eInvalidArgument("For networks with a softmax top layer, the sum of the target "
                    "vector must be 1.0");
        }
    }

    vector<f64>& top_da = m_layers[m_numLayers-1].da;
    top_da = actualOutput;
    for (u32 i = 0; i < top_da.size(); i++)
        top_da[i] -= target[i];

    for (u32 i = m_numLayers-1; i > 0; i--)
    {
        m_layers[i].accumError(m_layers[i-1].a, m_normalizeLayerInput);
        m_layers[i].backpropagate(m_layers[i-1].da);
    }

    m_layers[0].accumError(input, m_normalizeLayerInput);
}

void tANN::printNodeInfo(std::ostream& out) const
{
    for (i32 i = ((i32)m_numLayers-1); i >= 0; i--)
    {
        out << "Layer " << i << ":\n";
        for (u32 n = 0; n < getNumNodesAtLayer(i); n++)
        {
            out << "   a:  " << m_layers[i].a[n] << "\n";
            out << "   A:  " << m_layers[i].A[n] << "\n";
            out << "   da: " << m_layers[i].da[n] << "\n";
            out << "   dA: " << m_layers[i].dA[n] << "\n";
            out << "   b:  " << m_layers[i].w.back()[n] << "\n";
            out << "\n";
        }
    }
    out << endl;
}

void tANN::update()
{
    for (u32 i = 0; i < m_numLayers; i++)
    {
        f64 distFromTop = m_numLayers-1-i;
        f64 alphaHere = m_alpha * std::pow(m_alphaMultiplier, distFromTop);
        m_layers[i].updateWeights(alphaHere);
    }
}

void tANN::evaluate(const tIO& input, tIO& output)
{
    if (input.size()+1 != m_layers[0].w.size())
        throw eInvalidArgument("The input vector must be the same size as the ANN's input.");

    m_layers[0].takeInput(input, m_normalizeLayerInput);

    for (u32 i = 1; i < m_numLayers; i++)
        m_layers[i].takeInput(m_layers[i-1].a, m_normalizeLayerInput);

    output = m_layers[m_numLayers-1].a;
}

u32 tANN::getNumLayers() const
{
    return m_numLayers;
}

u32 tANN::getNumNodesAtLayer(u32 index) const
{
    if (index >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    return (u32) m_layers[index].a.size();
}

void tANN::getWeights(u32 layerIndex, u32 nodeIndex, vector<f64>& weights) const
{
    if (nodeIndex >= getNumNodesAtLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");

    vector< vector<f64> >& w = m_layers[layerIndex].w;
    weights.resize(w.size()-1);
    for (u32 s = 0; s < w.size()-1; s++)
        weights[s] = w[s][nodeIndex];
}

f64 tANN::getBias(u32 layerIndex, u32 nodeIndex) const
{
    if (nodeIndex >= getNumNodesAtLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");

    return m_layers[layerIndex].w.back()[nodeIndex];
}

f64 tANN::getOutput(u32 layerIndex, u32 nodeIndex) const
{
    if (nodeIndex >= getNumNodesAtLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");

    return m_layers[layerIndex].a[nodeIndex];
}

void tANN::getImage(u32 layerIndex, u32 nodeIndex,
                    img::tImage* image, bool color, u32 width,
                    bool absolute) const
{
    if (nodeIndex >= getNumNodesAtLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");

    // Get the weights.
    vector<f64> weights;
    getWeights(layerIndex, nodeIndex, weights);

    // Normalize the weights to [0.0, 255.0].
    f64 maxval = -1e100;
    f64 minval = 1e100;
    for (u32 i = 0; i < weights.size(); i++)
    {
        maxval = std::max(maxval, weights[i]);
        minval = std::min(minval, weights[i]);
    }
    if (absolute)
    {
        f64 absmax = std::max(std::fabs(maxval), std::fabs(minval));
        for (u32 i = 0; i < weights.size(); i++)
            weights[i] = (std::fabs(weights[i]) / absmax) * 255.0;
    }
    else
    {
        for (u32 i = 0; i < weights.size(); i++)
        {
            f64 val = (weights[i] - minval) * 255.0 / (maxval - minval);
            weights[i] = val;
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
    image->setFormat(img::kRGB24);
    image->setBufSize(width*height*3);
    image->setBufUsed(width*height*3);
    image->setWidth(width);
    image->setHeight(height);
    u8* buf = image->buf();
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
                buf[bufIndex++] = (u8) weights[wIndex];
                buf[bufIndex++] = (u8) weights[wIndex];
                buf[bufIndex++] = (u8) weights[wIndex++];
            }
        }
    }

    // Add an output indicator.
    nLayerType type = m_layers[layerIndex].layerType;
    f64 output = (getOutput(layerIndex, nodeIndex) - squash_min(type))
                    / (squash_max(type) - squash_min(type));
    u8 outputByte = (u8) (output*255.0);
    u8 red = 0;
    u8 green = (u8) (255 - outputByte);
    u8 blue = outputByte;
    u32 ySpan = height / 5;
    u32 yStart = 0;
    u32 xSpan = width / 5;
    u32 xStart = (u32) (output * (width-xSpan));
    for (u32 r = yStart; r < yStart+ySpan; r++)
    {
        for (u32 c = xStart; c < xStart+xSpan; c++)
        {
            buf = image->buf() + r*image->width()*3 + c*3;
            buf[0] = red;
            buf[1] = green;
            buf[2] = blue;
        }
    }
}

void tANN::pack(iWritable* out) const
{
    rho::pack(out, m_numLayers);
    rho::pack(out, m_alpha);
    rho::pack(out, m_alphaMultiplier);
    rho::pack(out, m_normalizeLayerInput);
    for (u32 i = 0; i < m_numLayers; i++)
        m_layers[i].pack(out);
}

void tANN::unpack(iReadable* in)
{
    rho::unpack(in, m_numLayers);
    rho::unpack(in, m_alpha);
    rho::unpack(in, m_alphaMultiplier);
    rho::unpack(in, m_normalizeLayerInput);
    delete [] m_layers;
    m_layers = new tLayer[m_numLayers];
    for (u32 i = 0; i < m_numLayers; i++)
        m_layers[i].unpack(in);
}


}    // namespace ml
}    // namespace rho
