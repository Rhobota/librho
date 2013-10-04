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

    u32 m_receptiveFieldWidth;   // The size of this CNN-layer's
    u32 m_receptiveFieldHeight;  // receptive field.

    u32 m_stepSizeX;    // The step size for the receptive field.
    u32 m_stepSizeY;    // ...

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

    u32 m_numLayers;    // The number of tLayers in the "layers" array above.
                        // (Is equal to (m_stepsX+1)*(m_stepsY+1).)

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

    void takeInput(const vector<f64>& input)
    {
        assertState(input.size());

        // TODO
    }

    void accumError(const vector<f64>& prev_a)
    {
        assertState(prev_a.size());

        // TODO
    }

    void backpropagate(vector<f64>& prev_da) const
    {
        assertState(prev_da.size());

        // TODO
    }

    void updateWeights()
    {
        assertState(m_inputSize);

        // TODO
    }
};


tCNN::tCNN(std::string descriptionString)
{
    // TODO
}

void tCNN::resetWeights()
{
    // TODO
}

tCNN::~tCNN()
{
    // TODO
}

void tCNN::addExample(const tIO& input, const tIO& target,
                      tIO& actualOutput)
{
    // TODO
}

void tCNN::update()
{
    // TODO
}

void tCNN::evaluate(const tIO& input, tIO& output) const
{
    // TODO
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
