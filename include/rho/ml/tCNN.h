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
         * ...
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
