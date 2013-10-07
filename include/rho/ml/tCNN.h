#ifndef __rho_ml_tCNN_h__
#define __rho_ml_tCNN_h__


#include <rho/bNonCopyable.h>
#include <rho/eRho.h>
#include <rho/iPackable.h>
#include <rho/img/tImage.h>
#include <rho/ml/common.h>
#include <rho/ml/iLearner.h>


namespace rho
{
namespace ml
{


class tCNN : public rho::iPackable, public rho::bNonCopyable, public iLearner
{
    public:

        //////////////////////////////////////////////////////////////////////
        // Constructors / resetWeights() / destructor
        //////////////////////////////////////////////////////////////////////

        /**
         * Creates a new CNN which will be subsequently trained.
         *
         * The hyper-parameters of the CNN will be described by the given
         * string.
         *
         * TODO -- define the descriptionString format.
         */
        tCNN(std::string descriptionString);

        /**
         * Resets the weighs in the network to the initial random weights
         * set by the above constructor.
         *
         * This is useful when you need to train the same network several
         * times with varying training sets (e.g. for ten-fold cross-
         * validation, or for exploring how the amount of training data
         * affects performance).
         */
        void resetWeights();

        /**
         * Destructor...
         */
        ~tCNN();

        //////////////////////////////////////////////////////////////////////
        // Training -- this is the iLearner interface
        //////////////////////////////////////////////////////////////////////

        /**
         * Shows the CNN one example. The CNN will calculate the error
         * gradients for each weight in the network, then add those gradients
         * to the accumulated error gradients for each weight.
         *
         * The network's weights will not be updated by this method.
         */
        void addExample(const tIO& input, const tIO& target,
                        tIO& actualOutput);

        /**
         * Updates the network's weights using the accumulated error
         * gradients. Updates are made according to the current weight
         * update rule for each layer.
         *
         * The accumulated error gradients are reset by this method.
         */
        void update();

        /**
         * Uses the network's current weights to evaluate the given input.
         */
        void evaluate(const tIO& input, tIO& output) const;

        //////////////////////////////////////////////////////////////////////
        // Debugging
        //////////////////////////////////////////////////////////////////////

        /**
         * Prints the network's configuration in a readable format.
         */
        void printNetworkInfo(std::ostream& out) const;

        /**
         * Returns a single-line version of printNetworkInfo().
         *
         * Useful for generating file names for storing CNN-related data.
         */
        std::string networkInfoString() const;

        //////////////////////////////////////////////////////////////////////
        // Getters
        //////////////////////////////////////////////////////////////////////

        /**
         * Returns the number of layers in the network. This will be one less
         * than what was specified to the constructor, because the CNN does
         * not actually use a layer for the input.
         */
        u32 getNumLayers() const;

        /**
         * Returns the number of feature maps in the specified layer. The given
         * parameter is interpreted as the layer index where 0 is the first
         * layer above the input, and all consecutive layers are higher in the
         * network (i.e. closer to the output layer).
         */
        u32 getNumFeatureMaps(u32 layerIndex) const;

        /**
         * Returns the weights of the connections below the specified feature
         * map. The number of weights returned will equal the size of the
         * receptive field below the specified feature map.
         */
        void getWeights(u32 layerIndex, u32 mapIndex, std::vector<f64>& weights) const;

        /**
         * Returns the bias value of the specified feature map.
         *
         * The getWeights() method omits the bias of each map so that
         * the number of weights returned is equal to the dimensionality
         * of the field below. This method is how you can access the bias
         * of a map.
         */
        f64 getBias(u32 layerIndex, u32 mapIndex) const;

        /**
         * Returns the number of replicated filters in the specified
         * layer. Aka, the number of receptive fields that are imposed
         * on the input of the specified layer.
         */
        u32 getNumReplicatedFilters(u32 layerIndex) const;

        /**
         * Returns the output value of the specified filter. This will be
         * the filter's output value from the last call to addExample() or
         * evaluate().
         */
        f64 getOutput(u32 layerIndex, u32 mapIndex, u32 filterIndex) const;

        /**
         * Generates an image representation of the specified feature map.
         * The image shows a visual representation of the weights of
         * the connections below the specified feature map.
         *
         * If the weights should be interpreted as an RGB image, set
         * 'color' to true. If the weights should be interpreted as a
         * grey image, set 'color' to false.
         *
         * If 'absolute' is set to true, the absolute value of the weights
         * will be used when producing the image. Otherwise, the relative
         * weights will be used to produce the image (meaning that weights
         * of value zero will be some shade of grey if any negative weights
         * are present).
         *
         * The generated image is stored in 'dest'.
         */
        void getFeatureMapImage(u32 layerIndex, u32 mapIndex,
                                bool color, bool absolute,
                                img::tImage* dest) const;

        /**
         * Generates an image representation of the output of every
         * replicated filter in the specified feature map.
         *
         * If the outputs should be interpreted as an RGB image, set
         * 'color' to true. If the outputs should be interpreted as a
         * grey image, set 'color' to false.
         *
         * If 'absolute' is set to true, the absolute value of the outputs
         * will be used when producing the image. Otherwise, the relative
         * outputs will be used to produce the image (meaning that outputs
         * of value zero will be some shade of grey if any negative outputs
         * are present).
         *
         * The generated image is stored in 'dest'.
         */
        void getOutputImage(u32 layerIndex, u32 mapIndex,
                            bool color, bool absolute,
                            img::tImage* dest) const;

        //////////////////////////////////////////////////////////////////////
        // END -- read no further
        //////////////////////////////////////////////////////////////////////

    public:

        // iPackable interface:
        void pack(iWritable* out) const;
        void unpack(iReadable* in);

    private:

        class tLayerCNN* m_layers;   // an array of layers
        u32 m_numLayers;             // number of layers

        f64 m_randWeightMin;   // used for resetWeights()
        f64 m_randWeightMax;   // ...
};


}    // namespace ml
}    // namespace rho


#endif // __rho_ml_tCNN_h__
