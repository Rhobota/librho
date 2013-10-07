#include <rho/ml/tCNN.h>


namespace rho
{
namespace ml
{


class tLayerCNN : public bNonCopyable
{
  private:

    u32 m_inputSize;     // Information about this CNN-layer's input.
    u32 m_inputWidth;    // ...
    u32 m_inputHeight;   // ...

    u32 m_receptiveFieldWidth;    // The size of this CNN-layer's
    u32 m_receptiveFieldHeight;   // receptive field.

    u32 m_stepSizeX;   // The step size for the receptive field.
    u32 m_stepSizeY;   // ...

    u32 m_stepsX;   // The number of times you can step the
    u32 m_stepsY;   // receptive field.

  private:

    u32 m_numFeatureMapsInThisLayer;   // The number of features maps that this
                                       // CNN-layer will have. That is,
                                       // the number of unique filters that
                                       // will be trained then replicated
                                       // across the receptive fields.

    tLayer* m_layers;   // An array of tLayers.
                        // These layers are not stacked on one another
                        // like they are in tANN. Instead these layers
                        // sit side-by-side and take input from the same
                        // input vector.
                        //
                        // Each layer has many neurons (aka, filters).
                        // Each neuron in m_layers[0] is replicated within
                        // all the other layers. That is, the weights to
                        // the neurons in m_layers[0] equal the weights to
                        // the neurons in m_layers[1], which equal the weights
                        // to the neurons in m_layers[2], etc.
                        //
                        // The reason for having these replicas is that each
                        // replica is applied on a unique portion of the input
                        // vector (i.e. each is applied to its own receptive
                        // field).

    u32 m_numLayers;   // The number of tLayers in the "layers" array above.
                       // (Is equal to (m_stepsX+1)*(m_stepsY+1).)

  private:

    // The following is state to make calculations faster so that you don't
    // have to reallocate vectors all the time.
    vector<f64> m_output;
    vector<f64> m_da;
    vector<f64> m_fieldInput;

  public:

    tLayerCNN()
    {
        // This constructed object is invalid. You must call init()
        // to set it up properly.
        m_layers = NULL;
        m_numLayers = 0;
    }

    void init(u32 inputSize, u32 inputRowWidth,
              u32 receptiveFieldWidth, u32 receptiveFieldHeight,
              u32 stepSizeHorizontal, u32 stepSizeVertical,
              u32 numFeatureMapsInThisLayer,
              f64 rmin, f64 rmax, algo::iLCG& lcg)
    {
        // Validation and setup of useful member state.
        assert(inputSize > 0);
        assert(inputRowWidth > 0);
        assert((inputSize % inputRowWidth) == 0);
        m_inputSize = inputSize;
        m_inputWidth = inputRowWidth;
        m_inputHeight = (inputSize % inputRowWidth);

        assert(receptiveFieldWidth > 0);
        assert(receptiveFieldHeight > 0);
        assert(m_inputWidth >= receptiveFieldWidth);
        assert(m_inputHeight >= receptiveFieldHeight);
        m_receptiveFieldWidth = receptiveFieldWidth;
        m_receptiveFieldHeight = receptiveFieldHeight;
        u32 numPossibleStepsX = m_inputWidth - m_receptiveFieldWidth;
        u32 numPossibleStepsY = m_inputHeight - m_receptiveFieldHeight;

        assert(stepSizeHorizontal > 0);
        assert(stepSizeVertical > 0);
        m_stepSizeX = stepSizeHorizontal;
        m_stepSizeY = stepSizeVertical;
        m_stepsX = numPossibleStepsX / m_stepSizeX;
        m_stepsY = numPossibleStepsY / m_stepSizeY;

        assert(numFeatureMapsInThisLayer > 0);
        assert(rmin < rmax);
        m_numFeatureMapsInThisLayer = numFeatureMapsInThisLayer;

        // Initializing the replicated layers.
        m_numLayers = (m_stepsX+1)*(m_stepsY+1);
        m_layers = new tLayer[m_numLayers];
        for (u32 i = 0; i < m_numLayers; i++)
        {
            // If this is the representative layer, initialize it randomly.
            if (i == 0)
            {
                m_layers[i].init(m_receptiveFieldWidth * m_receptiveFieldHeight,
                                 m_numFeatureMapsInThisLayer,
                                 rmin, rmax, lcg);
            }

            // Else, force the weights of each replicated filter to be the
            // same!
            else
            {
                m_layers[i].init_data(m_receptiveFieldWidth * m_receptiveFieldHeight,
                                      m_numFeatureMapsInThisLayer);
                m_layers[i].w = m_layers[0].w;
            }
        }

        // Init the fast data members.
        m_output = vector<f64>(m_numLayers * m_numFeatureMapsInThisLayer, 0.0);
        m_da = m_output;
        m_fieldInput = vector<f64>(m_receptiveFieldWidth * m_receptiveFieldHeight, 0.0);
    }

    void resetWeights(f64 rmin, f64 rmax, algo::iLCG& lcg)
    {
        for (u32 i = 0; i < m_numLayers; i++)
        {
            if (i == 0)
            {
                m_layers[i].init_weights(rmin, rmax, lcg);
            }

            else
            {
                m_layers[i].w = m_layers[0].w;
            }
        }
    }

    ~tLayerCNN()
    {
        delete [] m_layers;
        m_layers = NULL;
        m_numLayers = 0;
    }

    void assertState(size_t prevLayerSize) const
    {
        assert(prevLayerSize == m_inputSize);

        assert(m_numLayers > 0);
        assert(m_layers != NULL);

        for (u32 i = 0; i < m_numLayers; i++)
        {
            m_layers[i].assertState(m_receptiveFieldWidth * m_receptiveFieldHeight);
            assert(m_layers[i].a.size() == m_numFeatureMapsInThisLayer);
            assert(m_layers[i].w == m_layers[0].w);
        }
    }

    tANN::nLayerType getLayerType() const
    {
        return m_layers[0].layerType;
    }

    u32 getInputSize() const
    {
        return m_inputSize;
    }

    void m_fillField(const vector<f64>& input, u32 x, u32 y, vector<f64>& fieldInput) const
    {
        assert(input.size() == m_inputSize);
        assert(x+m_receptiveFieldWidth < m_inputWidth);
        assert(y+m_receptiveFieldHeight < m_inputHeight);
        assert(fieldInput.size() == m_receptiveFieldWidth * m_receptiveFieldHeight);

        u32 inputIndex = y*m_inputWidth + x;
        u32 fieldInputIndex = 0;
        for (u32 yy = 0; yy < m_receptiveFieldHeight; yy++)
        {
            for (u32 xx = 0; xx < m_receptiveFieldWidth; xx++)
            {
                fieldInput[fieldInputIndex++] = input[inputIndex+xx];
            }
            inputIndex += m_inputWidth;
        }
    }

    void m_reverseFillField(const vector<f64>& fieldInput, u32 x, u32 y, vector<f64>& input) const
    {
        assert(input.size() == m_inputSize);
        assert(x+m_receptiveFieldWidth < m_inputWidth);
        assert(y+m_receptiveFieldHeight < m_inputHeight);
        assert(fieldInput.size() == m_receptiveFieldWidth * m_receptiveFieldHeight);

        u32 inputIndex = y*m_inputWidth + x;
        u32 fieldInputIndex = 0;
        for (u32 yy = 0; yy < m_receptiveFieldHeight; yy++)
        {
            for (u32 xx = 0; xx < m_receptiveFieldWidth; xx++)
            {
                 input[inputIndex+xx] += fieldInput[fieldInputIndex++];
            }
            inputIndex += m_inputWidth;
        }
    }

    void takeInput(const vector<f64>& input)
    {
        assertState(input.size());

        u32 layerIndex = 0;

        for (u32 y = 0; y < m_inputHeight-m_receptiveFieldHeight; y += m_stepSizeY)
        {
            for (u32 x = 0; x < m_inputWidth-m_receptiveFieldWidth; x += m_stepSizeX)
            {
                m_fillField(input, x, y, m_fieldInput);
                m_layers[layerIndex++].takeInput(m_fieldInput);
            }
        }

        assert(layerIndex == (m_stepsX+1)*(m_stepsY+1));

        size_t outputIndex = 0;
        for (u32 i = 0; i < m_numLayers; i++)
        {
            for (u32 j = 0; j < m_numFeatureMapsInThisLayer; j++)
                m_output[outputIndex++] = m_layers[i].a[j];
        }
        assert(outputIndex == m_output.size());
    }

    vector<f64>& getOutput()
    {
        return m_output;
    }

    void accumError(const vector<f64>& prev_a)
    {
        assertState(prev_a.size());

        u32 layerIndex = 0;

        for (u32 y = 0; y < m_inputHeight-m_receptiveFieldHeight; y += m_stepSizeY)
        {
            for (u32 x = 0; x < m_inputWidth-m_receptiveFieldWidth; x += m_stepSizeX)
            {
                m_fillField(prev_a, x, y, m_fieldInput);
                m_layers[layerIndex++].accumError(m_fieldInput);
            }
        }

        assert(layerIndex == (m_stepsX+1)*(m_stepsY+1));
    }

    vector<f64>& get_da()
    {
        return m_da;
    }

    void distribute_da()
    {
        size_t daindex = 0;
        for (u32 i = 0; i < m_numLayers; i++)
        {
            for (u32 j = 0; j < m_numFeatureMapsInThisLayer; j++)
                 m_layers[i].da[j] = m_da[daindex++];
        }
        assert(daindex == m_da.size());
    }

    void backpropagate(vector<f64>& prev_da)
    {
        assertState(prev_da.size());

        for (size_t s = 0; s < prev_da.size(); s++)
            prev_da[s] = 0.0;

        u32 layerIndex = 0;

        for (u32 y = 0; y < m_inputHeight-m_receptiveFieldHeight; y += m_stepSizeY)
        {
            for (u32 x = 0; x < m_inputWidth-m_receptiveFieldWidth; x += m_stepSizeX)
            {
                m_layers[layerIndex++].backpropagate(m_fieldInput);
                m_reverseFillField(m_fieldInput, x, y, prev_da);
            }
        }

        assert(layerIndex == (m_stepsX+1)*(m_stepsY+1));
    }

    void m_accumWeights(vector< vector<f64> >& accum, vector< vector<f64> >& vals)
    {
        for (size_t s = 0; s < accum.size(); s++)
        {
            for (size_t i = 0; i < accum[s].size(); i++)
            {
                accum[s][i] += vals[s][i];
                vals[s][i] = 0.0;
            }
        }
    }

    void m_set(vector< vector<f64> >& dest, const vector< vector<f64> >& source)
    {
        for (size_t s = 0; s < dest.size(); s++)
        {
            for (size_t i = 0; i < dest[s].size(); i++)
            {
                dest[s][i] = source[s][i];
            }
        }
    }

    void updateWeights()
    {
        assertState(m_inputSize);

        for (u32 i = 1; i < m_numLayers; i++)
            m_accumWeights(m_layers[0].dw_accum, m_layers[i].dw_accum);

        m_layers[0].updateWeights();

        for (u32 i = 1; i < m_numLayers; i++)
            m_set(m_layers[i].w, m_layers[0].w);
    }
};


tCNN::tCNN(std::string descriptionString)
{
//     if (layerSizes.size() < 2)
//         throw eInvalidArgument("There must be at least an input and output layer.");
//     for (size_t i = 0; i < layerSizes.size(); i++)
//         if (layerSizes[i] == 0)
//             throw eInvalidArgument("Every layer must have size > 0");
//     if (randWeightMin >= randWeightMax)
//         throw eInvalidArgument("Invalid [randWeightMin, randWeightMax] range.");

    m_randWeightMin = -1.0;
    m_randWeightMax = 1.0;

    m_numLayers = 3;     // we don't need a layer for the input

    m_layers = new tLayerCNN[m_numLayers];
                            // m_layers[0] is the lowest layer
                            // (i.e. first one above the input layer)

    algo::tKnuthLCG lcg;

    m_layers[0].init(28*28,               //   u32 inputSize
                     28,                  //   u32 inputRowWidth
                     4,                   //   u32 receptiveFieldWidth
                     4,                   //   u32 receptiveFieldHeight
                     4,                   //   u32 stepSizeHorizontal
                     4,                   //   u32 stepSizeVertical
                     6,                   //   u32 numFeatureMapsInThisLayer
                     m_randWeightMin,     //   f64 rmin
                     m_randWeightMax,     //   f64 rmax
                     lcg);                //   algo::iLCG& lcg

    m_layers[1].init(294,                 //   u32 inputSize
                     42,                  //   u32 inputRowWidth
                     24,                  //   u32 receptiveFieldWidth
                     4,                   //   u32 receptiveFieldHeight
                     6,                   //   u32 stepSizeHorizontal
                     1,                   //   u32 stepSizeVertical
                     10,                  //   u32 numFeatureMapsInThisLayer
                     m_randWeightMin,     //   f64 rmin
                     m_randWeightMax,     //   f64 rmax
                     lcg);                //   algo::iLCG& lcg

    m_layers[1].init(160,                 //   u32 inputSize
                     40,                  //   u32 inputRowWidth
                     40,                  //   u32 receptiveFieldWidth
                     4,                   //   u32 receptiveFieldHeight
                     1,                   //   u32 stepSizeHorizontal
                     1,                   //   u32 stepSizeVertical
                     10,                  //   u32 numFeatureMapsInThisLayer
                     m_randWeightMin,     //   f64 rmin
                     m_randWeightMax,     //   f64 rmax
                     lcg);                //   algo::iLCG& lcg
}

void tCNN::resetWeights()
{
    algo::tKnuthLCG lcg;

    for (u32 i = 0; i < m_numLayers; i++)
    {
        m_layers[i].resetWeights(m_randWeightMin, m_randWeightMax, lcg);
    }
}

tCNN::~tCNN()
{
    delete [] m_layers;
    m_layers = NULL;
    m_numLayers = 0;
}

void tCNN::addExample(const tIO& input, const tIO& target,
                      tIO& actualOutput)
{
    // Validate the target vector.
    tANN::nLayerType type = m_layers[m_numLayers-1].getLayerType();
    for (u32 i = 0; i < target.size(); i++)
    {
        if (target[i] < s_squash_min(type) || target[i] > s_squash_max(type))
        {
            throw eInvalidArgument("The target vector must be in the range of the "
                                   "top layer's squashing function.");
        }
    }
    if (type == tANN::kLayerTypeSoftmax)
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

    // Show the example to the network.
    {
        evaluate(input, actualOutput);
        if (target.size() != actualOutput.size())
            throw eInvalidArgument("The target vector must be the same size as the CNN's output.");

        vector<f64>& top_da = m_layers[m_numLayers-1].get_da();
        assert(top_da.size() == actualOutput.size());
        for (u32 i = 0; i < top_da.size(); i++)
            top_da[i] = actualOutput[i] - target[i];
        m_layers[m_numLayers-1].distribute_da();

        for (u32 i = m_numLayers-1; i > 0; i--)
        {
            m_layers[i].accumError(m_layers[i-1].getOutput());
            m_layers[i].backpropagate(m_layers[i-1].get_da());
            m_layers[i-1].distribute_da();
        }

        m_layers[0].accumError(input);
    }
}

void tCNN::update()
{
    for (u32 i = 0; i < m_numLayers; i++)
    {
        m_layers[i].updateWeights();
    }
}

void tCNN::evaluate(const tIO& input, tIO& output) const
{
    if (input.size() != (size_t)m_layers[0].getInputSize())
        throw eInvalidArgument("The input vector must be the same size as the CNN's input.");

    m_layers[0].takeInput(input);

    for (u32 i = 1; i < m_numLayers; i++)
        m_layers[i].takeInput(m_layers[i-1].getOutput());

    output = m_layers[m_numLayers-1].getOutput();
}

void tCNN::pack(iWritable* out) const
{
    // TODO
}

void tCNN::unpack(iReadable* in)
{
    // TODO
}


}   // namespace ml
}   // namespace rho
