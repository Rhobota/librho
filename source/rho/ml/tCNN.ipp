#include <rho/ml/tCNN.h>

#include <rho/algo/string_util.h>


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

    u32 m_poolWidth;    // The output max-pooling size.
    u32 m_poolHeight;   // ...

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

    vector<f64> m_pooledOutput;  // Only used when (m_poolWidth > 1 || m_poolHeight > 1)
    vector<f64> m_pooled_da;     // (same)

    vector<f64> m_fieldInput;

  private:

    void m_fillField(const vector<f64>& input, u32 x, u32 y, vector<f64>& fieldInput) const
    {
        assert(input.size() == m_inputSize);
        assert(x+m_receptiveFieldWidth <= m_inputWidth);
        assert(y+m_receptiveFieldHeight <= m_inputHeight);
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
        assert(x+m_receptiveFieldWidth <= m_inputWidth);
        assert(y+m_receptiveFieldHeight <= m_inputHeight);
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

    void m_poolOutput()
    {
        size_t outWidth = (m_stepsX+1) * m_numFeatureMapsInThisLayer;
        size_t pooledoutIndex = 0;
        for (u32 y = 0; y <= (m_stepsY+1-m_poolHeight); y += m_poolHeight)
        {
            for (u32 x = 0; x <= (m_stepsX+1-m_poolWidth); x += m_poolWidth)
            {
                size_t outputIndex = y*outWidth + x*m_numFeatureMapsInThisLayer;
                for (u32 m = 0; m < m_numFeatureMapsInThisLayer; m++)
                {
                    size_t off = outputIndex + m;
                    f64 maxval = m_output[off];
                    for (u32 i = 0; i < m_poolHeight; i++)
                    {
                        size_t off2 = off;
                        for (u32 j = 0; j < m_poolWidth; j++)
                        {
                            maxval = std::max(maxval, m_output[off2]);
                            off2 += m_numFeatureMapsInThisLayer;
                        }
                        off += outWidth;
                    }
                    m_pooledOutput[pooledoutIndex++] = maxval;
                }
            }
        }
        assert(pooledoutIndex == m_pooledOutput.size());
    }

    void m_unpool_da_sparse()
    {
        size_t outWidth = (m_stepsX+1) * m_numFeatureMapsInThisLayer;
        size_t pooledoutIndex = 0;
        for (size_t i = 0; i < m_da.size(); i++)
            m_da[i] = 0.0;
        for (u32 y = 0; y <= (m_stepsY+1-m_poolHeight); y += m_poolHeight)
        {
            for (u32 x = 0; x <= (m_stepsX+1-m_poolWidth); x += m_poolWidth)
            {
                size_t outputIndex = y*outWidth + x*m_numFeatureMapsInThisLayer;
                for (u32 m = 0; m < m_numFeatureMapsInThisLayer; m++)
                {
                    size_t off = outputIndex + m;
                    f64 maxval = m_output[off];
                    size_t maxi = off;
                    for (u32 i = 0; i < m_poolHeight; i++)
                    {
                        size_t off2 = off;
                        for (u32 j = 0; j < m_poolWidth; j++)
                        {
                            if (maxval < m_output[off2])
                            {
                                maxval = m_output[off2];
                                maxi = off2;
                            }
                            off2 += m_numFeatureMapsInThisLayer;
                        }
                        off += outWidth;
                    }
                    m_da[maxi] = m_pooled_da[pooledoutIndex++];
                }
            }
        }
        assert(pooledoutIndex == m_pooled_da.size());
    }

    void m_unpool_da_dense()
    {
        size_t outWidth = (m_stepsX+1) * m_numFeatureMapsInThisLayer;
        size_t pooledoutIndex = 0;
        for (size_t i = 0; i < m_da.size(); i++)
            m_da[i] = 0.0;
        for (u32 y = 0; y <= m_stepsY; y += m_poolHeight)
        {
            for (u32 x = 0; x <= m_stepsX; x += m_poolWidth)
            {
                size_t outputIndex = y*outWidth + x*m_numFeatureMapsInThisLayer;
                for (u32 m = 0; m < m_numFeatureMapsInThisLayer; m++)
                {
                    size_t off = outputIndex + m;
                    for (u32 i = 0; i < m_poolHeight; i++)
                    {
                        size_t off2 = off;
                        for (u32 j = 0; j < m_poolWidth; j++)
                        {
                            m_da[off2] = m_pooled_da[pooledoutIndex];
                            off2 += m_numFeatureMapsInThisLayer;
                        }
                        off += outWidth;
                    }
                    pooledoutIndex++;
                }
            }
        }
        assert(pooledoutIndex == m_pooled_da.size());
    }

    void m_accum_dw(vector< vector<f64> >& accum, vector< vector<f64> >& vals)
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

    void m_norm_dw(vector< vector<f64> >& accum)
    {
        if (m_numLayers == 1)
            return;
        f64 div = (f64)m_numLayers;
        f64 mul = 1.0 / div;
        for (size_t s = 0; s < accum.size(); s++)
            for (size_t i = 0; i < accum[s].size(); i++)
                accum[s][i] *= mul;
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
              u32 poolWidth, u32 poolHeight,
              f64 rmin, f64 rmax, algo::iLCG& lcg)
    {
        // Validation and setup of useful member state.
        assert(inputSize > 0);
        assert(inputRowWidth > 0);
        assert((inputSize % inputRowWidth) == 0);
        m_inputSize = inputSize;
        m_inputWidth = inputRowWidth;
        m_inputHeight = (inputSize / inputRowWidth);

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
        m_numFeatureMapsInThisLayer = numFeatureMapsInThisLayer;

        assert(poolWidth > 0);
        assert(poolHeight > 0);
        assert(poolWidth <= (m_stepsX+1));
        assert(poolHeight <= (m_stepsY+1));
        m_poolWidth = poolWidth;
        m_poolHeight = poolHeight;

        assert(rmin < rmax);

        // Initializing the replicated layers.
        m_numLayers = (m_stepsX+1)*(m_stepsY+1);
        delete [] m_layers;
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
        m_output = vector<f64>((m_stepsX+1)*(m_stepsY+1) * m_numFeatureMapsInThisLayer, 0.0);
        m_da = m_output;

        m_pooledOutput = vector<f64>(((m_stepsX+1)/m_poolWidth) * ((m_stepsY+1)/m_poolHeight) * m_numFeatureMapsInThisLayer, 0.0);
        m_pooled_da = m_pooledOutput;

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
        assert(m_numLayers > 0);
        assert(m_layers != NULL);
        assert(prevLayerSize == m_inputSize);
        assert(m_inputSize == m_inputWidth * m_inputHeight);
        assert(m_numLayers == (m_stepsX+1)*(m_stepsY+1));

        for (u32 i = 0; i < m_numLayers; i++)
        {
            m_layers[i].assertState(m_receptiveFieldWidth * m_receptiveFieldHeight);
            assert(m_layers[i].a.size() == m_numFeatureMapsInThisLayer);
            assert(m_layers[i].w == m_layers[0].w);
            assert(m_layers[i].layerType == m_layers[0].layerType);
            assert(m_layers[i].normalizeLayerInput == m_layers[0].normalizeLayerInput);
            assert(m_layers[i].weightUpRule == m_layers[0].weightUpRule);
            assert(m_layers[i].alpha == m_layers[0].alpha);
        }
    }

    void setLayerType(tANN::nLayerType type)
    {
        assert(type >= 0 && type < tANN::kLayerTypeMax);
        for (u32 i = 0; i < m_numLayers; i++)
            m_layers[i].layerType = type;
    }

    void setLayerNormalizeLayerInput(bool norm)
    {
        for (u32 i = 0; i < m_numLayers; i++)
            m_layers[i].normalizeLayerInput = (norm ? 1 : 0);
    }

    void setLayerWeightUpdateRule(tANN::nWeightUpRule rule)
    {
        assert(rule >= 0 && rule < tANN::kWeightUpRuleMax);
        for (u32 i = 0; i < m_numLayers; i++)
            m_layers[i].weightUpRule = rule;
    }

    void setLayerAlpha(f64 alpha)
    {
        assert(alpha > 0.0);
        for (u32 i = 0; i < m_numLayers; i++)
            m_layers[i].alpha = alpha;
    }

    tLayer& getPrimaryLayer()
    {
        assert(m_numLayers > 0);
        return m_layers[0];
    }

    u32 getNumLayers() const
    {
        return m_numLayers;
    }

    tLayer& getLayer(u32 layerIndex)
    {
        assert(layerIndex < m_numLayers);
        return m_layers[layerIndex];
    }

    u32 getInputSize() const
    {
        return m_inputSize;
    }

    u32 getInputWidth() const
    {
        return m_inputWidth;
    }

    u32 getInputHeight() const
    {
        return m_inputHeight;
    }

    u32 getReceptiveFieldWidth() const
    {
        return m_receptiveFieldWidth;
    }

    u32 getReceptiveFieldHeight() const
    {
        return m_receptiveFieldHeight;
    }

    u32 getStepSizeX() const
    {
        return m_stepSizeX;
    }

    u32 getStepSizeY() const
    {
        return m_stepSizeY;
    }

    u32 getStepsX() const
    {
        return m_stepsX;
    }

    u32 getStepsY() const
    {
        return m_stepsY;
    }

    u32 getNumFeatureMaps() const
    {
        return m_numFeatureMapsInThisLayer;
    }

    u32 getPoolWidth() const
    {
        return m_poolWidth;
    }

    u32 getPoolHeight() const
    {
        return m_poolHeight;
    }

    void takeInput(const vector<f64>& input)
    {
        // Give input to each filter.
        u32 layerIndex = 0;
        for (u32 y = 0; y < m_inputHeight-m_receptiveFieldHeight+1; y += m_stepSizeY)
        {
            for (u32 x = 0; x < m_inputWidth-m_receptiveFieldWidth+1; x += m_stepSizeX)
            {
                m_fillField(input, x, y, m_fieldInput);
                m_layers[layerIndex++].takeInput(m_fieldInput);
            }
        }
        assert(layerIndex == (m_stepsX+1)*(m_stepsY+1));
        assert(layerIndex == m_numLayers);

        // Put the output of each filter into one output vector.
        size_t outputIndex = 0;
        for (u32 i = 0; i < m_numLayers; i++)
        {
            for (u32 j = 0; j < m_layers[i].a.size(); j++)
                m_output[outputIndex++] = m_layers[i].a[j];
        }
        assert(outputIndex == m_output.size());

        // Pool the output vector (only if we do pooling in this layer)
        if (m_poolWidth > 1 || m_poolHeight > 1)
        {
            m_poolOutput();   // <-- fills m_pooledOutput from the contents of m_output
        }
    }

    vector<f64>& getOutput()
    {
        if (m_poolWidth > 1 || m_poolHeight > 1)
            return m_pooledOutput;
        else
            return m_output;
    }

    vector<f64>& getRealOutput()
    {
        return m_output;
    }

    vector<f64>& get_da()
    {
        if (m_poolWidth > 1 || m_poolHeight > 1)
            return m_pooled_da;
        else
            return m_da;
    }

    void copy_da_to_output()
    {
        // Copy da to output.
        for (size_t i = 0; i < m_da.size(); i++)
            m_output[i] = m_da[i];

        // Normalize the output vector so that it actually looks like legitimate
        // output.
        assert(m_output.size() > 0);
        f64 minval = m_output[0];
        f64 maxval = m_output[0];
        for (size_t i = 1; i < m_output.size(); i++)
        {
            minval = std::min(minval, m_output[i]);
            maxval = std::max(maxval, m_output[i]);
        }
        f64 absmax = std::max(std::fabs(minval), std::fabs(maxval));
        f64 minpos = s_squash_min(getPrimaryLayer().layerType);
        f64 maxpos = s_squash_max(getPrimaryLayer().layerType);
        assert(minpos <= 0.0);  // <-- the code in the loop below needs these
        assert(maxpos > 0.0);   // <-- to be true
        for (size_t i = 0; i < m_output.size(); i++)
        {
            if (m_output[i] < 0.0)
                m_output[i] = m_output[i] / absmax * -minpos;
            else
                m_output[i] = m_output[i] / absmax * maxpos;
            assert(m_output[i] >= minpos && m_output[i] <= maxpos);
        }

        // Pool the output.
        m_poolOutput();   // <-- fills m_pooledOutput from the contents of m_output

        // Put output back into the output of each filter.
        // (Opposite operation that is done at the end of takeInput())
        size_t outputIndex = 0;
        for (u32 i = 0; i < m_numLayers; i++)
        {
            for (u32 j = 0; j < m_layers[i].a.size(); j++)
            {
                m_layers[i].a[j] = m_output[outputIndex++];
                m_layers[i].A[j] = 0.0;
            }
        }
        assert(outputIndex == m_output.size());
    }

    void accumError(const vector<f64>& prev_a)
    {
        u32 layerIndex = 0;
        for (u32 y = 0; y < m_inputHeight-m_receptiveFieldHeight+1; y += m_stepSizeY)
        {
            for (u32 x = 0; x < m_inputWidth-m_receptiveFieldWidth+1; x += m_stepSizeX)
            {
                m_fillField(prev_a, x, y, m_fieldInput);
                m_layers[layerIndex++].accumError(m_fieldInput);
            }
        }
        assert(layerIndex == (m_stepsX+1)*(m_stepsY+1));
    }

    void distribute_da(bool dense)
    {
        // If we do max pooling, we will need to expand the pooled da.
        if (m_poolWidth > 1 || m_poolHeight > 1)
        {
            if (dense)
                m_unpool_da_dense();   // <-- fills m_da from the contents of m_pooled_da
            else
                m_unpool_da_sparse();  // <-- fills m_da from the contents of m_pooled_da
        }

        // Distribute.
        size_t daindex = 0;
        for (u32 i = 0; i < m_numLayers; i++)
        {
            for (u32 j = 0; j < m_layers[i].da.size(); j++)
                 m_layers[i].da[j] = m_da[daindex++];
        }
        assert(daindex == m_da.size());
    }

    void backpropagate(vector<f64>& prev_da)
    {
        for (size_t s = 0; s < prev_da.size(); s++)
            prev_da[s] = 0.0;

        u32 layerIndex = 0;
        for (u32 y = 0; y < m_inputHeight-m_receptiveFieldHeight+1; y += m_stepSizeY)
        {
            for (u32 x = 0; x < m_inputWidth-m_receptiveFieldWidth+1; x += m_stepSizeX)
            {
                m_layers[layerIndex++].backpropagate(m_fieldInput);
                m_reverseFillField(m_fieldInput, x, y, prev_da);
            }
        }
        assert(layerIndex == (m_stepsX+1)*(m_stepsY+1));
    }

    void updateWeights()
    {
        assertState(m_inputSize);

        for (u32 i = 1; i < m_numLayers; i++)
            m_accum_dw(m_layers[0].dw_accum, m_layers[i].dw_accum);

        // m_norm_dw(m_layers[0].dw_accum);    // !!!!!!!! This used to be
                                               // active, so I'm leaving it
                                               // here in case it is good
                                               // to have I'll come back to it.

        m_layers[0].updateWeights();

        for (u32 i = 1; i < m_numLayers; i++)
            m_set(m_layers[i].w, m_layers[0].w);
    }

    void pack(iWritable* out) const
    {
        rho::pack(out, m_inputSize);
        rho::pack(out, m_inputWidth);
        rho::pack(out, m_inputHeight);
        rho::pack(out, m_receptiveFieldWidth);
        rho::pack(out, m_receptiveFieldHeight);
        rho::pack(out, m_stepSizeX);
        rho::pack(out, m_stepSizeY);
        rho::pack(out, m_stepsX);
        rho::pack(out, m_stepsY);
        rho::pack(out, m_poolWidth);
        rho::pack(out, m_poolHeight);
        rho::pack(out, m_numFeatureMapsInThisLayer);
        rho::pack(out, m_numLayers);
        m_layers[0].pack(out);
    }

    void unpack(iReadable* in)
    {
        rho::unpack(in, m_inputSize);
        rho::unpack(in, m_inputWidth);
        rho::unpack(in, m_inputHeight);
        rho::unpack(in, m_receptiveFieldWidth);
        rho::unpack(in, m_receptiveFieldHeight);
        rho::unpack(in, m_stepSizeX);
        rho::unpack(in, m_stepSizeY);
        rho::unpack(in, m_stepsX);
        rho::unpack(in, m_stepsY);
        rho::unpack(in, m_poolWidth);
        rho::unpack(in, m_poolHeight);
        rho::unpack(in, m_numFeatureMapsInThisLayer);
        rho::unpack(in, m_numLayers);
        assert(m_numLayers > 0);

        delete [] m_layers;
        m_layers = new tLayer[m_numLayers];
        m_layers[0].unpack(in);

        for (u32 i = 1; i < m_numLayers; i++)
        {
            tByteWritable bw;
            m_layers[0].pack(&bw);
            tByteReadable br(bw.getBuf());
            m_layers[i].unpack(&br);
        }

        assertState(m_inputSize);

        m_output = vector<f64>((m_stepsX+1)*(m_stepsY+1) * m_numFeatureMapsInThisLayer, 0.0);
        m_da = m_output;

        m_pooledOutput = vector<f64>(((m_stepsX+1)/m_poolWidth) * ((m_stepsY+1)/m_poolHeight) * m_numFeatureMapsInThisLayer, 0.0);
        m_pooled_da = m_pooledOutput;

        m_fieldInput = vector<f64>(m_receptiveFieldWidth * m_receptiveFieldHeight, 0.0);
    }
};


static
u32 s_toInt(const string& str)
{
    std::istringstream in(str);
    u32 val;
    if (!(in >> val))
        throw eInvalidArgument("Expected numeric value where there was not one.");
    return val;
}


tCNN::tCNN(string descriptionString)
    : m_layers(NULL),
      m_numLayers(0)
{
    m_randWeightMin = -1.0;
    m_randWeightMax = 1.0;
    algo::tKnuthLCG lcg;

    {
        m_numLayers = 0;
        std::istringstream in(descriptionString);
        string line;
        getline(in, line);
        while (getline(in, line)) m_numLayers++;
    }

    if (m_numLayers == 0)
        throw eInvalidArgument("There must be at least one layer in the CNN.");

    m_layers = new tLayerCNN[m_numLayers];
                            // m_layers[0] is the lowest layer
                            // (i.e. first one above the input layer)

    try
    {
        std::istringstream in(descriptionString);
        string line;

        u32 width, height;
        {
            if (!getline(in, line))
                throw eInvalidArgument("Not sure why this failed.");
            std::istringstream linein(line);
            if (!(linein >> width >> height))
                throw eInvalidArgument("Cannot read input description line.");
        }

        u32 nmaps = 1;

        for (u32 i = 0; i < m_numLayers; i++)
        {
            if (!getline(in, line))
                throw eInvalidArgument("Not sure why this failed.");

            vector<string> parts = algo::split(line, " ");

            if (parts.size() == 7)
            {
                // A convolutional layer:

                u32 rfwidth = s_toInt(parts[0]);
                u32 rfheight = s_toInt(parts[1]);
                u32 rfstepx = s_toInt(parts[2]);
                u32 rfstepy = s_toInt(parts[3]);
                u32 nmapsHere = s_toInt(parts[4]);
                u32 poolwidth = s_toInt(parts[5]);
                u32 poolheight = s_toInt(parts[6]);

                m_layers[i].init(width*height*nmaps,  //   u32 inputSize
                                 width*nmaps,         //   u32 inputRowWidth
                                 rfwidth*nmaps,       //   u32 receptiveFieldWidth
                                 rfheight,            //   u32 receptiveFieldHeight
                                 rfstepx*nmaps,       //   u32 stepSizeHorizontal
                                 rfstepy,             //   u32 stepSizeVertical
                                 nmapsHere,           //   u32 numFeatureMapsInThisLayer
                                 poolwidth,           //   u32 poolWidth
                                 poolheight,          //   u32 poolHeight
                                 m_randWeightMin,     //   f64 rmin
                                 m_randWeightMax,     //   f64 rmax
                                 lcg);                //   algo::iLCG& lcg

                nmaps = nmapsHere;
                width = (m_layers[i].getStepsX()+1) / poolwidth;
                height = (m_layers[i].getStepsY()+1) / poolheight;
            }
            else if (parts.size() == 1)
            {
                // A fully-connected layer:

                u32 numouts = s_toInt(parts[0]);

                m_layers[i].init(width*height*nmaps,  //   u32 inputSize
                                 width*nmaps,         //   u32 inputRowWidth
                                 width*nmaps,         //   u32 receptiveFieldWidth
                                 height,              //   u32 receptiveFieldHeight
                                 1,                   //   u32 stepSizeHorizontal
                                 1,                   //   u32 stepSizeVertical
                                 numouts,             //   u32 numFeatureMapsInThisLayer
                                 1,                   //   u32 poolWidth
                                 1,                   //   u32 poolHeight
                                 m_randWeightMin,     //   f64 rmin
                                 m_randWeightMax,     //   f64 rmax
                                 lcg);                //   algo::iLCG& lcg

                nmaps = numouts;
                width = 1;
                height = 1;
            }
            else
            {
                throw eInvalidArgument("Invalid line in description string.");
            }
        }
    }
    catch (std::exception& e)
    {
        delete [] m_layers;
        m_layers = NULL;
        m_numLayers = 0;
        throw;
    }
}

tCNN::tCNN(iReadable* in)
    : m_layers(NULL),
      m_numLayers(0)
{
    this->unpack(in);
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

void tCNN::setLayerType(tANN::nLayerType type, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (type < 0 || type >= tANN::kLayerTypeMax)
        throw eInvalidArgument("Invalid layer type");
    if (type == tANN::kLayerTypeSoftmax && layerIndex != m_numLayers-1)
        throw eInvalidArgument("Only the top layer may be a softmax group.");
    m_layers[layerIndex].setLayerType(type);
}

void tCNN::setLayerType(tANN::nLayerType type)
{
    if (type < 0 || type >= tANN::kLayerTypeMax)
        throw eInvalidArgument("Invalid layer type");
    if (type == tANN::kLayerTypeSoftmax && m_numLayers>1)
        throw eInvalidArgument("Only the top layer may be a softmax group.");
    for (u32 i = 0; i < m_numLayers; i++)
        setLayerType(type, i);
}

void tCNN::setNormalizeLayerInput(bool on, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    m_layers[layerIndex].setLayerNormalizeLayerInput(on);
}

void tCNN::setNormalizeLayerInput(bool on)
{
    for (u32 i = 0; i < m_numLayers; i++)
        setNormalizeLayerInput(on, i);
}

void tCNN::setWeightUpRule(tANN::nWeightUpRule rule, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (rule < 0 || rule >= tANN::kWeightUpRuleMax)
        throw eInvalidArgument("Invalid weight update rule");
    m_layers[layerIndex].setLayerWeightUpdateRule(rule);
}

void tCNN::setWeightUpRule(tANN::nWeightUpRule rule)
{
    if (rule < 0 || rule >= tANN::kWeightUpRuleMax)
        throw eInvalidArgument("Invalid weight update rule");
    for (u32 i = 0; i < m_numLayers; i++)
        setWeightUpRule(rule, i);
}

void tCNN::setAlpha(f64 alpha, u32 layerIndex)
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");
    if (alpha <= 0.0)
        throw eInvalidArgument("Alpha must be greater than zero.");
    m_layers[layerIndex].setLayerAlpha(alpha);
}

void tCNN::setAlpha(f64 alpha)
{
    if (alpha <= 0.0)
        throw eInvalidArgument("Alpha must be greater than zero.");
    for (u32 i = 0; i < m_numLayers; i++)
        setAlpha(alpha, i);
}

void tCNN::addExample(const tIO& input, const tIO& target,
                      tIO& actualOutput)
{
    // Validate the target vector.
    tANN::nLayerType type = m_layers[m_numLayers-1].getPrimaryLayer().layerType;
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
        m_layers[m_numLayers-1].distribute_da(false);

        for (u32 i = m_numLayers-1; i > 0; i--)
        {
            m_layers[i].accumError(m_layers[i-1].getOutput());
            m_layers[i].backpropagate(m_layers[i-1].get_da());
            m_layers[i-1].distribute_da(false);
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

void tCNN::reset()
{
    resetWeights();
}

void tCNN::printLearnerInfo(std::ostream& out) const
{
    int colw = 20;

    out << "Convolutional Neural Network Info:" << endl;

    // Layer type (and normalizeLayerInput):
    out << "                 layer type:";
    out << std::right << std::setw(colw) << "input";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        string print = s_layerTypeToString(m_layers[i].getPrimaryLayer().layerType);
        if (m_layers[i].getPrimaryLayer().normalizeLayerInput)
            print += "(norm'd)";
        out << std::right << std::setw(colw) << print;
    }
    out << endl;

    // Weight update rule:
    out << "         weight update rule:";
    out << std::right << std::setw(colw) << "-";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        std::ostringstream ss;
        ss << s_weightUpRuleToString(m_layers[i].getPrimaryLayer().weightUpRule);
        switch (m_layers[i].getPrimaryLayer().weightUpRule)
        {
            case tANN::kWeightUpRuleNone:
                break;
            case tANN::kWeightUpRuleFixedLearningRate:
                ss << "(a=" << m_layers[i].getPrimaryLayer().alpha << ")";
                break;
            default:
                assert(false);
        }
        out << std::right << std::setw(colw) << ss.str();
    }
    out << endl;

    // Number of feature maps:
    out << "        number feature maps:";
    out << std::right << std::setw(colw) << "-";
    for (u32 i = 0; i < m_numLayers; i++)
        out << std::right << std::setw(colw) << m_layers[i].getPrimaryLayer().a.size();
    out << endl;

    // Receptive field sizes:
    out << "       receptive field size:";
    out << std::right << std::setw(colw) << "-";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        std::ostringstream ss;
        ss << m_layers[i].getReceptiveFieldWidth() / (i > 0 ? m_layers[i-1].getNumFeatureMaps() : 1)
           << "x"
           << m_layers[i].getReceptiveFieldHeight();
        out << std::right << std::setw(colw) << ss.str();
    }
    out << endl;

    // Receptive field step sizes:
    out << "            field step size:";
    out << std::right << std::setw(colw) << "-";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        std::ostringstream ss;
        ss << m_layers[i].getStepSizeX() / (i > 0 ? m_layers[i-1].getNumFeatureMaps() : 1)
           << "x"
           << m_layers[i].getStepSizeY();
        out << std::right << std::setw(colw) << ss.str();
    }
    out << endl;

    // Network output dimensions:
    out << "                  out dim's:";
    {
        std::ostringstream ss;
        ss << m_layers[0].getInputWidth() << "x"
           << m_layers[0].getInputHeight();
        out << std::right << std::setw(colw) << ss.str();
    }
    for (u32 i = 0; i < m_numLayers; i++)
    {
        std::ostringstream ss;
        ss << (m_layers[i].getStepsX()+1);
        ss << "x";
        ss << (m_layers[i].getStepsY()+1);
        out << std::right << std::setw(colw) << ss.str();
    }
    out << endl;

    // Pool dimensions:
    out << "                 pool dim's:";
    out << std::right << std::setw(colw) << "1x1";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        std::ostringstream ss;
        ss << m_layers[i].getPoolWidth() << "x" << m_layers[i].getPoolHeight();
        out << std::right << std::setw(colw) << ss.str();
    }
    out << endl;

    // Network output size:
    out << "                   out size:";
    out << std::right << std::setw(colw) << m_layers[0].getInputSize();
    for (u32 i = 0; i < m_numLayers; i++)
        out << std::right << std::setw(colw) << m_layers[i].getOutput().size();
    out << endl;

    out << endl;
}

void tCNN::backpropagateMaxError(i32 outputDimensionIndex, tIO& errorOnInput)
{
    vector<f64>& top_da = m_layers[m_numLayers-1].get_da();
    for (size_t i = 0; i < top_da.size(); i++)
        top_da[i] = (outputDimensionIndex < 0 || i == (size_t)outputDimensionIndex) ? 1.0 : 0.0;
    m_layers[m_numLayers-1].distribute_da(true);
    m_layers[m_numLayers-1].copy_da_to_output();

    for (u32 i = m_numLayers-1; i > 0; i--)
    {
        m_layers[i].accumError(m_layers[i-1].getOutput());
        m_layers[i].backpropagate(m_layers[i-1].get_da());
        m_layers[i-1].distribute_da(true);
        m_layers[i-1].copy_da_to_output();
    }

    errorOnInput.resize(m_layers[0].getInputSize(), 0.0);
    m_layers[0].accumError(errorOnInput);
    m_layers[0].backpropagate(errorOnInput);
}

string tCNN::learnerInfoString() const
{
    std::ostringstream out;

    // Network output sizes:
    out << "size=";
    out << m_layers[0].getInputSize();
    for (u32 i = 0; i < m_numLayers; i++)
        out << '-' << m_layers[i].getOutput().size();

    // Layer type (and normalizeLayerInput):
    out << "__type=";
    out << "i";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        out << '-' << s_layerTypeToChar(m_layers[i].getPrimaryLayer().layerType);
        if (m_layers[i].getPrimaryLayer().normalizeLayerInput)
            out << 'n';
    }

    // Weight update rule:
    out << "__wrule=";
    out << "i";
    for (u32 i = 0; i < m_numLayers; i++)
    {
        out << '-' << s_weightUpRuleToChar(m_layers[i].getPrimaryLayer().weightUpRule);
        switch (m_layers[i].getPrimaryLayer().weightUpRule)
        {
            case tANN::kWeightUpRuleNone:
                break;
            case tANN::kWeightUpRuleFixedLearningRate:
                out << m_layers[i].getPrimaryLayer().alpha;
                break;
            default:
                assert(false);
        }
    }

    // Number of feature maps:
    out << "__maps=";
    out << "i";
    for (u32 i = 0; i < m_numLayers; i++)
        out << '-' << m_layers[i].getPrimaryLayer().a.size();

    return out.str();
}

u32 tCNN::getNumLayers() const
{
    return m_numLayers;
}

bool tCNN::isLayerPooled(u32 layerIndex) const
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");

    return (m_layers[layerIndex].getPoolWidth() > 1) ||
           (m_layers[layerIndex].getPoolHeight() > 1);
}

bool tCNN::isLayerFullyConnected(u32 layerIndex) const
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");

    return (m_layers[layerIndex].getStepsX() == 0) &&
           (m_layers[layerIndex].getStepsY() == 0);
}

u32 tCNN::getNumFeatureMaps(u32 layerIndex) const
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");

    return (u32) (m_layers[layerIndex].getPrimaryLayer().a.size());
}

void tCNN::getWeights(u32 layerIndex, u32 mapIndex, vector<f64>& weights) const
{
    if (mapIndex >= getNumFeatureMaps(layerIndex))
        throw eInvalidArgument("No layer/map with that index.");

    vector< vector<f64> >& w = m_layers[layerIndex].getPrimaryLayer().w;
    weights.resize(w.size()-1);
    for (u32 s = 0; s < w.size()-1; s++)
        weights[s] = w[s][mapIndex];
}

f64 tCNN::getBias(u32 layerIndex, u32 mapIndex) const
{
    if (mapIndex >= getNumFeatureMaps(layerIndex))
        throw eInvalidArgument("No layer/map with that index.");

    vector< vector<f64> >& w = m_layers[layerIndex].getPrimaryLayer().w;
    return w.back()[mapIndex];
}

u32 tCNN::getNumReplicatedFilters(u32 layerIndex) const
{
    if (layerIndex >= m_numLayers)
        throw eInvalidArgument("No layer with that index.");

    return m_layers[layerIndex].getNumLayers();
}

f64 tCNN::getOutput(u32 layerIndex, u32 mapIndex, u32 filterIndex,
                    f64* minValue, f64* maxValue) const
{
    if (mapIndex >= getNumFeatureMaps(layerIndex))
        throw eInvalidArgument("No layer/map with that index.");

    if (filterIndex >= getNumReplicatedFilters(layerIndex))
        throw eInvalidArgument("No layer/filter with that index.");

    if (minValue)
        *minValue = s_squash_min(m_layers[layerIndex].getLayer(filterIndex).layerType);
    if (maxValue)
        *maxValue = s_squash_max(m_layers[layerIndex].getLayer(filterIndex).layerType);

    return m_layers[layerIndex].getLayer(filterIndex).a[mapIndex];
}

void tCNN::getFeatureMapImage(u32 layerIndex, u32 mapIndex,
                              bool color, bool absolute,
                              img::tImage* dest) const
{
    // Get the weights.
    vector<f64> weights;
    getWeights(layerIndex, mapIndex, weights);
    assert(weights.size() > 0);
    u32 width = m_layers[layerIndex].getReceptiveFieldWidth();
    assert(width > 0);
    if (color)
    {
        if ((width % 3) > 0)
            throw eLogicError("Pixels do not align with width of the receptive field.");
        width /= 3;
    }

    // Use the image creating method in ml::common to do the work.
    un_examplify(weights, color, width, absolute, dest);
}

void tCNN::getOutputImage(u32 layerIndex, u32 mapIndex,
                          bool pooled,
                          img::tImage* dest) const
{
    if (mapIndex >= getNumFeatureMaps(layerIndex))
        throw eInvalidArgument("No layer/map with that index.");

    // Get the weights.
    vector<f64> weights;
    u32 width;
    {
        size_t stride = getNumFeatureMaps(layerIndex);
        vector<f64>& alloutput = pooled ? m_layers[layerIndex].getOutput()
                                        : m_layers[layerIndex].getRealOutput();

        for (size_t i = mapIndex; i < alloutput.size(); i += stride)
            weights.push_back(alloutput[i]);
        assert(weights.size() > 0);

        width = pooled ? (m_layers[layerIndex].getStepsX()+1) / m_layers[layerIndex].getPoolWidth()
                       : (m_layers[layerIndex].getStepsX()+1);
        assert(width > 0);
    }

    // Tell un_examplify() about the range of this data.
    // (Note, when creating images from weight values, the range is (-inf, inf), so it
    // is okay to let un_examplify() determine a good range itself, but here we know
    // the range and we want the resulting image to represent the values relative to that
    // range.
    f64 minValue = s_squash_min(m_layers[layerIndex].getPrimaryLayer().layerType);
    f64 maxValue = s_squash_max(m_layers[layerIndex].getPrimaryLayer().layerType);

    // Use the image creating method in ml::common to do the work.
    un_examplify(weights, false, width, false, dest, &minValue, &maxValue);
}

void tCNN::pack(iWritable* out) const
{
    rho::pack(out, m_numLayers);
    for (u32 i = 0; i < m_numLayers; i++)
        m_layers[i].pack(out);
    rho::pack(out, m_randWeightMin);
    rho::pack(out, m_randWeightMax);
}

void tCNN::unpack(iReadable* in)
{
    // Try to unpack the network.
    u32 numLayers;
    f64 randWeightMin, randWeightMax;
    rho::unpack(in, numLayers);
    tLayerCNN* layers = new tLayerCNN[numLayers];
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
