#ifndef __rho_ml_tANN_h__
#define __rho_ml_tANN_h__


#include <rho/bNonCopyable.h>
#include <rho/eRho.h>
#include <rho/iPackable.h>
#include <rho/img/tImage.h>
#include <rho/ml/common.h>
#include <rho/sync/tThreadPool.h>

#include <cmath>
#include <iostream>
#include <vector>


namespace rho
{
namespace ml
{


class tANN : public rho::iPackable, public rho::bNonCopyable
{
    public:

        /**
         * layerSizes.front() will be the dimensionality of the ANN's input.
         * This layer will be called the bottom layer.
         *
         * layerSizes.back() will be the dimensionality of the ANN's output.
         * This layer will be called the top layer.
         *
         * All other specified layers will be hidden layers in the ANN,
         * meaning that there are (layerSizes.size()-2) hidden layers.
         *
         * The learning rate of the top layer is given by the alpha parameter.
         * The layer immediately below the top layer will learn at a rate
         * of alpha*alphaMultiplier. The next previous layer will learn at
         * a rate of alpha*alphaMultiplier^2. Etc...
         *
         * Notice that setting alphaMultiplier equal to 1.0 causes all layers
         * of the network to learn at the same rate (i.e. alpha itself).
         * Set alphaMultiplier greater than 1.0 to cause the lower layers of
         * the network to learn at faster rates.
         *
         * When a layer has many inputs, the weighted sum will be initially
         * very large. You can choose to normalize each layer's input with
         * respect to the number of inputs so that the weighted sum is not
         * so large. Do so by setting normalizeLayerInput to true.
         *
         * The network's weights are initialized randomly over the uniform
         * range [randWeightMin, randWeightMax].
         */
        tANN(std::vector<u32> layerSizes,
             f64 alpha,
             f64 alphaMultiplier = 1.0,
             bool normalizeLayerInput = true,
             f64 randWeightMin = -1.0,
             f64 randWeightMax = 1.0);

        /**
         * Reads the ANN from an input stream.
         */
        tANN(iReadable* in);

        /**
         * The ANN's calculations can be parallelized in a thread pool, if you
         * provide it a thread pool. Use this method to set the thread pool.
         */
        void setThreadPool(refc<sync::tThreadPool> pool);

        /**
         * Shows the ANN one example. The ANN will calculate error rates at
         * each node, then add those rates to the accumulated error at each
         * node.
         *
         * The inter-node weights will not be updated by this method.
         */
        void addExample(const tIO& input, const tIO& target,
                        tIO& actualOutput);

        /**
         * Prints the most recently calculated error rates for each node and
         * the bias value of each node.
         */
        void printNodeInfo(std::ostream& out) const;

        /**
         * Updates the inter-node weights using the accumulated error at each
         * node.
         *
         * The accumulated error rates are then cleared.
         */
        void updateWeights();

        /**
         * Uses the current inter-node weights to evaluate the given input.
         */
        void evaluate(const tIO& input, tIO& output);

        /**
         * Returns the number of layers in the network. This will be one less than
         * what was specified to the constructor, because the ANN does not actually
         * use a layer for the input.
         */
        u32 getNumLayers() const;

        /**
         * Returns the number of nodes in the specified layer. The given parameter
         * is interpreted as the layer index where 0 is first layer above the input,
         * and all consecutive layers are higher in the network (i.e. closer to the
         * output layer).
         */
        u32 getNumNodesAtLayer(u32 index) const;

        /**
         * Returns the weights of the inter-node connections below the specified
         * node. The number of weights returned will equal the number of nodes
         * in the layer below the specified layer. If the specified layer is 0,
         * the number of weights returned will equal the dimensionality of the
         * input.
         */
        void getWeights(u32 layerIndex, u32 nodeIndex, std::vector<f64>& weights) const;

        /**
         * Returns the bias value of the specified node.
         */
        f64 getBias(u32 layerIndex, u32 nodeIndex) const;

        /**
         * Returns the output value of the specified node. This will be
         * the node's output value from the last call to addExample() or
         * evaluate().
         */
        f64 getOutput(u32 layerIndex, u32 nodeIndex) const;

        /**
         * Generates an image representation of the specified node. The
         * image includes the weights of the inter-node connections
         * below the specified node as well as the node's most recent
         * output value.
         *
         * If the weights should be interpreted as an RGB image, set
         * color to true. If the weights should be interpreted as a
         * grey image, set color to false.
         *
         * Specify the width of the image, and the height will be derived.
         *
         * If absolute is set to true, the absolute value of the weights
         * will be used when producing the image. Otherwise, the relative
         * weights will be used to produce the image (meaning that weights
         * of value zero will be some shade of grey if any negative weights
         * are present).
         */
        void getImage(u32 layerIndex, u32 nodeIndex,
                      img::tImage* image, bool color, u32 width,
                      bool absolute=false) const;

        /**
         * Destructor...
         */
        ~tANN();

    public:

        // iPackable interface:
        void pack(iWritable* out) const;
        void unpack(iReadable* in);

    private:

        struct tLayer* m_layers;
        u32 m_numLayers;

        f64 m_alpha;
        f64 m_alphaMultiplier;

        u8 m_normalizeLayerInput;

        refc<sync::tThreadPool> m_pool;
};


}    // namespace ml
}    // namespace rho


#endif // __rho_ml_tANN_h__
