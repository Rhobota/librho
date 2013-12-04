#include <rho/ml/iLearner.h>

#include <rho/ml/tANN.h>
#include <rho/ml/tCNN.h>


namespace rho
{
namespace ml
{


iLearner* iLearner::newDeserializedLearner(const std::vector<u8>& serializedLearner)
{
    iLearner* learner = NULL;

    tByteReadable in(serializedLearner);

    u32 type; rho::unpack(&in, type);

    switch (type)
    {
        case 1:
            learner = new tANN(&in);
            break;

        case 2:
            learner = new tCNN(&in);
            break;

        default:
            throw eRuntimeError("The serialized vector is of an unknown type of learner.");
            break;
    }

    return learner;
}


void iLearner::serializeLearner(iLearner* learner, std::vector<u8>& serializedLearner)
{
    if (learner == NULL)
        throw eNullPointer("learner must not be NULL");

    tByteWritable out;

    tANN* ann = dynamic_cast<tANN*>(learner);
    tCNN* cnn = dynamic_cast<tCNN*>(learner);

    if (ann)
    {
        rho::pack(&out, (u32)1);
        ann->pack(&out);
    }

    else if (cnn)
    {
        rho::pack(&out, (u32)2);
        cnn->pack(&out);
    }

    else
    {
        throw eInvalidArgument("Cannot serialize the given learner which has an unknown type.");
    }

    serializedLearner = out.getBuf();
}


}   // namespace ml
}   // namespace rho
