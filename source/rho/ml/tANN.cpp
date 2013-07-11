#include <rho/ml/tANN.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>


using std::vector;
using std::cout;
using std::endl;


namespace rho
{
namespace ml
{


struct tLayer
{
    vector<f64> a;
    vector<f64> A;

    vector<f64> da;
    vector<f64> dA;

    vector< vector<f64> > w;   // <-- the weights connecting to the layer below
                               //                       (i.e. the previous layer)

    vector< vector<f64> > dw_accum;

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

    void randomizeWeights(f64 rmin, f64 rmax)
    {
        for (u32 s = 0; s < w.size(); s++)
        {
            for (u32 i = 0; i < w[s].size(); i++)
            {
                w[s][i] = ((f64)rand()) / RAND_MAX;    // [0.0, 1.0]
                w[s][i] *= rmax-rmin;                  // [0.0, rmax-rmin]
                w[s][i] += rmin;                       // [rmin, rmax]
            }
        }
    }

    void takeInput(const vector<f64>& input, u8 normalizeLayerInput)
    {
        assert(input.size()+1 == w.size());
        assert(w.size() == dw_accum.size());
        assert(a.size() == A.size());
        assert(a.size() == da.size());
        assert(a.size() == dA.size());

        for (u32 i = 0; i < A.size(); i++)
            A[i] = 0.0;

        for (u32 s = 0; s < input.size(); s++)
        {
            assert(A.size() == w[s].size());
            assert(w[s].size() == dw_accum[s].size());

            for (u32 i = 0; i < A.size(); i++)
                A[i] += w[s][i] * input[s];
        }

        assert(A.size() == w.back().size());
        assert(w.back().size() == dw_accum.back().size());

        for (u32 i = 0; i < A.size(); i++)
            A[i] += w.back()[i] * 1.0;

        if (normalizeLayerInput)
        {
            for (u32 i = 0; i < A.size(); i++)
                A[i] /= (f64) input.size();
        }

        for (u32 i = 0; i < a.size(); i++)
            a[i] = logistic_function(A[i]);
    }

    void accumError(const vector<f64>& prev_a)
    {
        assert(prev_a.size()+1 == w.size());
        assert(w.size() == dw_accum.size());
        assert(a.size() == A.size());
        assert(a.size() == da.size());
        assert(a.size() == dA.size());

        for (u32 i = 0; i < da.size(); i++)
        {
            dA[i] = da[i] * derivative_of_logistic_function(A[i]);
        }

        for (u32 s = 0; s < prev_a.size(); s++)
        {
            assert(dA.size() == w[s].size());
            assert(w[s].size() == dw_accum[s].size());

            for (u32 i = 0; i < dA.size(); i++)
                dw_accum[s][i] += dA[i] * prev_a[s];
        }

        assert(dA.size() == w.back().size());
        assert(w.back().size() == dw_accum.back().size());

        for (u32 i = 0; i < dA.size(); i++)
            dw_accum.back()[i] += dA[i] * 1.0;
    }

    void backpropagate(vector<f64>& prev_da) const
    {
        assert(prev_da.size()+1 == w.size());
        assert(w.size() == dw_accum.size());
        assert(a.size() == A.size());
        assert(a.size() == da.size());
        assert(a.size() == dA.size());

        for (u32 s = 0; s < prev_da.size(); s++)
        {
            assert(dA.size() == w[s].size());
            assert(w[s].size() == dw_accum[s].size());

            prev_da[s] = 0.0;

            for (u32 i = 0; i < dA.size(); i++)
                prev_da[s] += w[s][i] * dA[i];
        }
    }

    void updateWeights(f64 alpha)
    {
        assert(w.size() == dw_accum.size());
        assert(a.size() == A.size());
        assert(a.size() == da.size());
        assert(a.size() == dA.size());

        for (u32 s = 0; s < w.size(); s++)
        {
            assert(a.size() == w[s].size());
            assert(w[s].size() == dw_accum[s].size());

            for (u32 i = 0; i < w[s].size(); i++)
            {
                w[s][i] -= alpha * dw_accum[s][i];
                dw_accum[s][i] = 0.0;
            }
        }
    }

    void pack(iWritable* out)
    {
        rho::pack(out, a);
        rho::pack(out, A);
        rho::pack(out, da);
        rho::pack(out, dA);
        rho::pack(out, w);
        rho::pack(out, dw_accum);
    }

    void unpack(iReadable* in)
    {
        rho::unpack(in, a);
        rho::unpack(in, A);
        rho::unpack(in, da);
        rho::unpack(in, dA);
        rho::unpack(in, w);
        rho::unpack(in, dw_accum);
    }
};


tANN::tANN(vector<u32> layerSizes,
           f64 alpha,
           f64 alphaMultiplier,
           bool normalizeLayerInput,
           f64 randWeightMin,
           f64 randWeightMax)
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

    m_numLayers = (u32) layerSizes.size()-1;
                            // we don't need a layer for the input

    m_layers = new tLayer[m_numLayers];
                            // m_layers[0] is the lowest layer
                            // (i.e. first one above the input layer)

    for (u32 i = 1; i < layerSizes.size(); i++)
    {
        m_layers[i-1].setSizes(layerSizes[i-1], layerSizes[i]);
        m_layers[i-1].randomizeWeights(randWeightMin, randWeightMax);
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

void tANN::addExample(const vector<f64>& input, const vector<f64>& target,
                      vector<f64>& actualOutput)
{
    evaluate(input, actualOutput);

    if (target.size() != actualOutput.size())
        throw eInvalidArgument("The target vector must be the same size as the ANN's output.");

    for (u32 i = 0; i < target.size(); i++)
        if (target[i] < logistic_function_min() || target[i] > logistic_function_max())
            throw eInvalidArgument("The target vector must be in the range of the logistic function.");

    vector<f64>& top_da = m_layers[m_numLayers-1].da;
    top_da = actualOutput;
    for (u32 i = 0; i < top_da.size(); i++)
        top_da[i] -= target[i];

    for (u32 i = m_numLayers-1; i > 0; i--)
    {
        m_layers[i].accumError(m_layers[i-1].a);
        m_layers[i].backpropagate(m_layers[i-1].da);
    }

    m_layers[0].accumError(input);
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

void tANN::updateWeights()
{
    for (u32 i = 0; i < m_numLayers; i++)
    {
        f64 distFromTop = m_numLayers-1-i;
        f64 alphaHere = m_alpha * std::pow(m_alphaMultiplier, distFromTop);
        m_layers[i].updateWeights(alphaHere);
    }
}

void tANN::evaluate(const vector<f64>& input, vector<f64>& output) const
{
    if (input.size()+1 != m_layers[0].w.size())
        throw eInvalidArgument("The input vector must be the same size as the ANN's input.");

    for (u32 i = 0; i < input.size(); i++)
        if (input[i] < logistic_function_min() || input[i] > logistic_function_max())
            throw eInvalidArgument("The input vector must be in the range of the logistic function.");

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
                    img::tImage* image, bool color, u32 width) const
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
    for (u32 i = 0; i < weights.size(); i++)
    {
        f64 val = (weights[i] - minval) * 255.0 / (maxval - minval);
        weights[i] = val;
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
    f64 output = getOutput(layerIndex, nodeIndex);
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
