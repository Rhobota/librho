#if __linux__
#pragma GCC optimize 3
#endif

#include <rho/ml/tANN.h>
#include <rho/algo/tLCG.h>

#include <cassert>
#include <iomanip>
#include <sstream>

using std::vector;
using std::string;
using std::endl;


namespace rho
{
namespace ml
{


static
string s_layerTypeToString(tANN::nLayerType type)
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
            assert(false);
    }
}


static
char s_layerTypeToChar(tANN::nLayerType type)
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
            assert(false);
    }
}


static
string s_weightUpRuleToString(tANN::nWeightUpRule rule)
{
    switch (rule)
    {
        case tANN::kWeightUpRuleNone:
            return "none";
        case tANN::kWeightUpRuleFixedLearningRate:
            return "fixedrate";
        default:
            assert(false);
    }
}


static
char s_weightUpRuleToChar(tANN::nWeightUpRule rule)
{
    switch (rule)
    {
        case tANN::kWeightUpRuleNone:
            return 'n';
        case tANN::kWeightUpRuleFixedLearningRate:
            return 'f';
        default:
            assert(false);
    }
}


static
f64 s_squash(f64 val, tANN::nLayerType type)
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
            assert(false);
    }
}


static
f64 s_derivative_of_squash(f64 val, tANN::nLayerType type)
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
            assert(false);
    }
}


static
f64 s_squash_min(tANN::nLayerType type)
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
            assert(false);
    }
}


static
f64 s_squash_max(tANN::nLayerType type)
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
            assert(false);
    }
}


class tLayer : public bNonCopyable
{ public:

    /////////////////////////////////////////////////////////////////////////////////////
    // Connection state
    /////////////////////////////////////////////////////////////////////////////////////

    vector<f64> a;     // the output of each neuron (the squashed values)
    vector<f64> A;     // the accumulated input of each neuron (the pre-squashed values)

    vector<f64> da;    // dE/da -- the error gradient wrt the output of each neuron
    vector<f64> dA;    // dE/dA -- the error gradient wrt the accumulated input of each neuron

    vector< vector<f64> > w;   // <-- the weights connecting to the layer below
                               //                       (i.e. the previous layer)

    vector< vector<f64> > dw_accum;   // dE/dw -- the error gradient wrt each weight
    u32 batchSize;                    // number of dE/dw inside dw_accum

    /////////////////////////////////////////////////////////////////////////////////////
    // Behavioral state -- defines the squashing function and derivative calculations
    /////////////////////////////////////////////////////////////////////////////////////

    tANN::nLayerType layerType;
    u8 normalizeLayerInput;

    /////////////////////////////////////////////////////////////////////////////////////
    // Behavioral state -- how is the gradient handled (learning rates, momentum, etc)
    /////////////////////////////////////////////////////////////////////////////////////

    tANN::nWeightUpRule weightUpRule;
    f64 alpha;

    /////////////////////////////////////////////////////////////////////////////////////
    // Methods...
    /////////////////////////////////////////////////////////////////////////////////////

    tLayer()
    {
        // This is an invalid object. You must call init() or unpack() to setup
        // this object properly.
        layerType = tANN::kLayerTypeMax;
        weightUpRule = tANN::kWeightUpRuleMax;
    }

    void init_data(u32 prevSize, u32 mySize)
    {
        // Setup connection state size.
        a  = vector<f64>(mySize, 0.0);
        A  = vector<f64>(mySize, 0.0);
        da = vector<f64>(mySize, 0.0);
        dA = vector<f64>(mySize, 0.0);
        w  = vector< vector<f64> >(prevSize+1, vector<f64>(mySize, 0.0));
        dw_accum = vector< vector<f64> >(prevSize+1, vector<f64>(mySize, 0.0));
        batchSize = 0;

        // Setup behavioral state to the default values.
        layerType = tANN::kLayerTypeHyperbolic;
        normalizeLayerInput = 1;
        weightUpRule = tANN::kWeightUpRuleNone;
        alpha = 0.0;

        // Make sure all is well. (Of course it should be...)
        assertState(prevSize);
    }

    void init_weights(f64 rmin, f64 rmax, algo::iLCG& lcg)
    {
        // Randomize the initial weights.
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

    void init(u32 prevSize, u32 mySize,
              f64 rmin, f64 rmax, algo::iLCG& lcg)
    {
        init_data(prevSize, mySize);
        init_weights(rmin, rmax, lcg);
    }

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

        assert(layerType >= 0 && layerType < tANN::kLayerTypeMax);
        assert(weightUpRule >= 0 && weightUpRule < tANN::kWeightUpRuleMax);
    }

    void takeInput(const vector<f64>& input)
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
            f64 denom = 0.0;
            for (u32 i = 0; i < a.size(); i++)
            {
                a[i] = std::min(std::exp(A[i]), 1e100);
                denom += a[i];
            }
            for (u32 i = 0; i < a.size(); i++)
                a[i] /= denom;
        }
        else
        {
            for (u32 i = 0; i < A.size(); i++)
                a[i] = s_squash(A[i], layerType);
        }
    }

    void accumError(const vector<f64>& prev_a)
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
                dA[i] = da[i] * s_derivative_of_squash(A[i], layerType);
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
        batchSize++;
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

    void updateWeights()
    {
        assertState(w.size()-1);

        switch (weightUpRule)
        {
            case tANN::kWeightUpRuleNone:
            {
                for (u32 s = 0; s < w.size(); s++)
                    for (u32 i = 0; i < w[s].size(); i++)
                        dw_accum[s][i] = 0.0;
                batchSize = 0;
                break;
            }

            case tANN::kWeightUpRuleFixedLearningRate:
            {
                if (alpha <= 0.0)
                    throw eLogicError("When using the fixed learning rate rule, alpha must be set.");
                double mult = (10.0 / batchSize) * alpha;
                for (u32 s = 0; s < w.size(); s++)
                {
                    for (u32 i = 0; i < w[s].size(); i++)
                    {
                        w[s][i] -= mult * dw_accum[s][i];
                        dw_accum[s][i] = 0.0;
                    }
                }
                batchSize = 0;
                break;
            }

            default:
            {
                assert(false);
                break;
            }
        }
    }

    void pack(iWritable* out) const
    {
        u32 mySize = (u32) a.size();
        u32 prevLayerSize = (u32) w.size()-1;
        u8 type = (u8) layerType;
        u8 rule = (u8) weightUpRule;

        rho::pack(out, mySize);
        rho::pack(out, prevLayerSize);
        rho::pack(out, type);
        rho::pack(out, normalizeLayerInput);
        rho::pack(out, rule);

        switch (weightUpRule)
        {
            case tANN::kWeightUpRuleNone:
                break;

            case tANN::kWeightUpRuleFixedLearningRate:
                rho::pack(out, alpha);
                break;

            default:
                assert(false);
        }

        rho::pack(out, w);
    }

    void unpack(iReadable* in)
    {
        u32 mySize, prevLayerSize;
        rho::unpack(in, mySize);
        rho::unpack(in, prevLayerSize);
        if (mySize == 0 || prevLayerSize == 0)
            throw eRuntimeError("Invalid layer stream -- invalid sizes");
        init_data(prevLayerSize, mySize);

        u8 type;
        rho::unpack(in, type);
        layerType = (tANN::nLayerType) type;
        if (layerType < 0 || layerType >= tANN::kLayerTypeMax)
            throw eRuntimeError("Invalid layer stream -- invalid layer type");

        rho::unpack(in, normalizeLayerInput);
        if (normalizeLayerInput > 1)
            throw eRuntimeError("Invalid layer stream -- invalid normalizeLayerInput");

        u8 rule;
        rho::unpack(in, rule);
        weightUpRule = (tANN::nWeightUpRule) rule;
        if (weightUpRule < 0 || weightUpRule >= tANN::kWeightUpRuleMax)
            throw eRuntimeError("Invalid layer stream -- invalid weight update rule");

        switch (weightUpRule)
        {
            case tANN::kWeightUpRuleNone:
                break;

            case tANN::kWeightUpRuleFixedLearningRate:
                rho::unpack(in, alpha);
                break;

            default:
                assert(false);
        }

        rho::unpack(in, w);
        assertState(prevLayerSize);
    }
};


tANN::tANN(iReadable* in)
    : m_layers(NULL),
      m_numLayers(0)
{
    this->unpack(in);
}

tANN::tANN(vector<u32> layerSizes,
           f64 randWeightMin,
           f64 randWeightMax)
    : m_layers(NULL),
      m_numLayers(0),
      m_randWeightMin(randWeightMin),
      m_randWeightMax(randWeightMax)
{
    if (layerSizes.size() < 2)
        throw eInvalidArgument("There must be at least an input and output layer.");
    for (size_t i = 0; i < layerSizes.size(); i++)
        if (layerSizes[i] == 0)
            throw eInvalidArgument("Every layer must have size > 0");
    if (randWeightMin >= randWeightMax)
        throw eInvalidArgument("Invalid [randWeightMin, randWeightMax] range.");

    m_numLayers = (u32) layerSizes.size()-1;
                            // we don't need a layer for the input

    m_layers = new tLayer[m_numLayers];
                            // m_layers[0] is the lowest layer
                            // (i.e. first one above the input layer)

    algo::tKnuthLCG lcg;

    for (u32 i = 1; i < layerSizes.size(); i++)
    {
        m_layers[i-1].init(layerSizes[i-1], layerSizes[i],
                           m_randWeightMin, m_randWeightMax, lcg);
    }
}

void tANN::resetWeights()
{
    algo::tKnuthLCG lcg;

    for (u32 i = 0; i < m_numLayers; i++)
    {
        m_layers[i].init_weights(m_randWeightMin, m_randWeightMax, lcg);
    }
}

tANN::~tANN()
{
    delete [] m_layers;
    m_layers = NULL;
    m_numLayers = 0;
}

void tANN::setLayerType(nLayerType type, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (type < 0 || type >= kLayerTypeMax)
        throw eInvalidArgument("Invalid layer type");
    if (type == kLayerTypeSoftmax && layerIndex != m_numLayers-1)
        throw eInvalidArgument("Only the top layer may be a softmax group.");
    m_layers[layerIndex].layerType = type;
}

void tANN::setLayerType(nLayerType type)
{
    if (type < 0 || type >= kLayerTypeMax)
        throw eInvalidArgument("Invalid layer type");
    if (type == kLayerTypeSoftmax && m_numLayers>1)
        throw eInvalidArgument("Only the top layer may be a softmax group.");
    for (u32 i = 0; i < m_numLayers; i++)
        setLayerType(type, i);
}

void tANN::setNormalizeLayerInput(bool on, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    m_layers[layerIndex].normalizeLayerInput = (on ? 1 : 0);
}

void tANN::setNormalizeLayerInput(bool on)
{
    for (u32 i = 0; i < m_numLayers; i++)
        setNormalizeLayerInput(on, i);
}

void tANN::setWeightUpRule(nWeightUpRule rule, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (rule < 0 || rule >= kWeightUpRuleMax)
        throw eInvalidArgument("Invalid weight update rule");
    m_layers[layerIndex].weightUpRule = rule;
}

void tANN::setWeightUpRule(nWeightUpRule rule)
{
    if (rule < 0 || rule >= kWeightUpRuleMax)
        throw eInvalidArgument("Invalid weight update rule");
    for (u32 i = 0; i < m_numLayers; i++)
        setWeightUpRule(rule, i);
}

void tANN::setAlpha(f64 alpha, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (alpha <= 0.0)
        throw eInvalidArgument("Alpha must be greater than zero.");
    m_layers[layerIndex].alpha = alpha;
}

void tANN::setAlpha(f64 alpha)
{
    if (alpha <= 0.0)
        throw eInvalidArgument("Alpha must be greater than zero.");
    for (u32 i = 0; i < m_numLayers; i++)
        setAlpha(alpha, i);
}

void tANN::addExample(const tIO& input, const tIO& target,
                tIO& actualOutput)
{
    // Validate the target vector.
    nLayerType type = m_layers[m_numLayers-1].layerType;
    for (u32 i = 0; i < target.size(); i++)
    {
        if (target[i] < s_squash_min(type) || target[i] > s_squash_max(type))
        {
            throw eInvalidArgument("The target vector must be in the range of the "
                                   "top layer's squashing function.");
        }
    }
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
    if (target.size() != getNumNeuronsInLayer(m_numLayers-1))
        throw eInvalidArgument("The target vector must be the same size as the ANN's output.");

    // Show the example to the network.
    {
        evaluate(input, actualOutput);
        assert(target.size() == actualOutput.size());

        vector<f64>& top_da = m_layers[m_numLayers-1].da;
        assert(top_da.size() == actualOutput.size());

        for (u32 i = 0; i < top_da.size(); i++)
            top_da[i] = actualOutput[i] - target[i];

        for (u32 i = m_numLayers-1; i > 0; i--)
        {
            m_layers[i].accumError(m_layers[i-1].a);
            m_layers[i].backpropagate(m_layers[i-1].da);
        }

        m_layers[0].accumError(input);
    }
}

void tANN::update()
{
    for (u32 i = 0; i < m_numLayers; i++)
    {
        m_layers[i].updateWeights();
    }
}

void tANN::evaluate(const tIO& input, tIO& output) const
{
    if (input.size()+1 != m_layers[0].w.size())
        throw eInvalidArgument("The input vector must be the same size as the ANN's input.");

    m_layers[0].takeInput(input);

    for (u32 i = 1; i < m_numLayers; i++)
        m_layers[i].takeInput(m_layers[i-1].a);

    output = m_layers[m_numLayers-1].a;
}

void tANN::printNetworkInfo(std::ostream& out) const
{
    int colw = 20;

    out << "Artificial Neural Network Info:" << endl;

    // Network size:
    out << "   (bottom-to-top) size:";
    out << std::right << std::setw(colw) << m_layers[0].w.size()-1;
    for (u32 i = 0; i < m_numLayers; i++)
        out << std::right << std::setw(colw) << m_layers[i].a.size();
    out << endl;

    // Layer type (and normalizeLayerInput):
    out << "             layer type:";
    out << std::right << std::setw(colw) << "input";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        string print = s_layerTypeToString(m_layers[i].layerType);
        if (m_layers[i].normalizeLayerInput)
            print += "(norm'd)";
        out << std::right << std::setw(colw) << print;
    }
    out << endl;

    // Weight update rule:
    out << "     weight update rule:";
    out << std::right << std::setw(colw) << "N/A";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        std::ostringstream ss;
        ss << s_weightUpRuleToString(m_layers[i].weightUpRule);
        switch (m_layers[i].weightUpRule)
        {
            case kWeightUpRuleNone:
                break;
            case kWeightUpRuleFixedLearningRate:
                ss << "(a=" << m_layers[i].alpha << ")";
                break;
            default:
                assert(false);
        }
        out << std::right << std::setw(colw) << ss.str();
    }
    out << endl << endl;
}

string tANN::networkInfoString() const
{
    std::ostringstream out;

    out << "size=" << m_layers[0].w.size()-1;
    for (u32 i = 0; i < m_numLayers; i++)
        out << '-' << m_layers[i].a.size();

    out << "__type=" << 'i';
    for (u32 i = 0; i < m_numLayers; i++)
    {
        out << '-' << s_layerTypeToChar(m_layers[i].layerType);
        if (m_layers[i].normalizeLayerInput)
            out << 'n';
    }

    out << "__wrule=" << 'i';
    for (u32 i = 0; i < m_numLayers; i++)
    {
        out << '-' << s_weightUpRuleToChar(m_layers[i].weightUpRule);
        switch (m_layers[i].weightUpRule)
        {
            case kWeightUpRuleNone:
                break;
            case kWeightUpRuleFixedLearningRate:
                out << m_layers[i].alpha;
                break;
            default:
                assert(false);
        }
    }

    return out.str();
}

void tANN::printNeuronInfo(std::ostream& out) const
{
    for (i32 i = ((i32)m_numLayers-1); i >= 0; i--)
    {
        out << "Layer " << i << ":\n";
        for (u32 n = 0; n < getNumNeuronsInLayer(i); n++)
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

u32 tANN::getNumLayers() const
{
    return m_numLayers;
}

u32 tANN::getNumNeuronsInLayer(u32 layerIndex) const
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    return (u32) m_layers[layerIndex].a.size();
}

void tANN::getWeights(u32 layerIndex, u32 neuronIndex, vector<f64>& weights) const
{
    if (neuronIndex >= getNumNeuronsInLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");

    vector< vector<f64> >& w = m_layers[layerIndex].w;
    weights.resize(w.size()-1);
    for (u32 s = 0; s < w.size()-1; s++)
        weights[s] = w[s][neuronIndex];
}

f64 tANN::getBias(u32 layerIndex, u32 neuronIndex) const
{
    if (neuronIndex >= getNumNeuronsInLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");
    return m_layers[layerIndex].w.back()[neuronIndex];
}

f64 tANN::getOutput(u32 layerIndex, u32 neuronIndex) const
{
    if (neuronIndex >= getNumNeuronsInLayer(layerIndex))
        throw eInvalidArgument("No layer/node with that index.");
    return m_layers[layerIndex].a[neuronIndex];
}

void tANN::getImage(u32 layerIndex, u32 neuronIndex,
              bool color, u32 width, bool absolute,
              img::tImage* dest) const
{
    // Get the weights.
    vector<f64> weights;
    getWeights(layerIndex, neuronIndex, weights);
    assert(weights.size() > 0);

    // Normalize the weights to [0.0, 255.0].
    f64 maxval = weights[0];
    f64 minval = weights[0];
    for (u32 i = 1; i < weights.size(); i++)
    {
        maxval = std::max(maxval, weights[i]);
        minval = std::min(minval, weights[i]);
    }
    if (maxval == minval) maxval += 0.000001;
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
            f64 val = ((weights[i] - minval) / (maxval - minval)) * 255.0;
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
                buf[bufIndex++] = (u8) weights[wIndex];
                buf[bufIndex++] = (u8) weights[wIndex];
                buf[bufIndex++] = (u8) weights[wIndex++];
            }
        }
    }

    // Add an output indicator.
    nLayerType type = m_layers[layerIndex].layerType;
    f64 output = (getOutput(layerIndex, neuronIndex) - s_squash_min(type))
                    / (s_squash_max(type) - s_squash_min(type));   // output now in [0, 1]
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
            buf = dest->buf() + r*dest->width()*3 + c*3;
            buf[0] = red;
            buf[1] = green;
            buf[2] = blue;
        }
    }
}

void tANN::pack(iWritable* out) const
{
    rho::pack(out, m_numLayers);
    for (u32 i = 0; i < m_numLayers; i++)
        m_layers[i].pack(out);
    rho::pack(out, m_randWeightMin);
    rho::pack(out, m_randWeightMax);
}

void tANN::unpack(iReadable* in)
{
    // Try to unpack the network.
    u32 numLayers;
    f64 randWeightMin, randWeightMax;
    rho::unpack(in, numLayers);
    tLayer* layers = new tLayer[numLayers];
    try
    {
        for (u32 i = 0; i < numLayers; i++)
            layers[i].unpack(in);
        rho::unpack(in, randWeightMin);
        rho::unpack(in, randWeightMax);
    }
    catch (ebObject& e)
    {
        delete [] layers;
        throw;
    }

    // If it worked, clobber the current network.
    m_numLayers = numLayers;
    delete [] m_layers;
    m_layers = layers;
    m_randWeightMin = randWeightMin;
    m_randWeightMax = randWeightMax;
}


}   // namespace ml
}   // namespace rho


#include "tCNN.ipp"    // this is done because tCNN uses tLayer, and this will
                       // allow the compiler to optimize the code a lot
