#if __linux__
#include "linuxImpl.ipp"
#elif __APPLE__
#include "osxImpl.ipp"
#endif


#include <rho/img/tImageCapFactory.h>
#include <rho/eRho.h>

#include <cmath>
#include <vector>

using std::vector;


namespace rho
{
namespace img
{


static
vector<tImageCapParams> pruneInput(tImageCapParams params,
                                   vector<tImageCapParams> possibleParams)
{
    vector<tImageCapParams> v;
    for (size_t i = 0; i < possibleParams.size(); i++)
    {
        tImageCapParams p = possibleParams[i];
        if (p.deviceIndex == params.deviceIndex &&
            p.inputIndex == params.inputIndex)
        {
            v.push_back(p);
        }
    }
    return v;
}


static
vector<tImageCapParams> pruneCaptureFmt(tImageCapParams params,
                                        vector<tImageCapParams> possibleParams)
{
    vector<tImageCapParams> v;
    for (size_t i = 0; i < possibleParams.size(); i++)
    {
        tImageCapParams p = possibleParams[i];
        if (p.captureFormat == params.captureFormat)
        {
            v.push_back(p);
        }
    }
    return v;
}


static
double sizeDiff(tImageCapParams a, tImageCapParams b)
{
    return hypot(a.imageHeight-b.imageHeight, a.imageWidth-b.imageWidth);
}


static
tImageCapParams findClosestSize(tImageCapParams params,
                                 vector<tImageCapParams> possibleParams)
{
    if (possibleParams.size() == 0)
        throw eRuntimeError("Cannot coerce image capture params to anything.");

    tImageCapParams best = possibleParams[0];

    for (size_t i = 1; i < possibleParams.size(); i++)
    {
        tImageCapParams p = possibleParams[i];
        if (sizeDiff(best, params) > sizeDiff(p, params))
            best = p;
    }

    return best;
}


static
vector<tImageCapParams> pruneSize(tImageCapParams params,
                                  vector<tImageCapParams> possibleParams)
{
    vector<tImageCapParams> v;
    for (size_t i = 0; i < possibleParams.size(); i++)
    {
        tImageCapParams p = possibleParams[i];
        if (p.imageWidth == params.imageWidth &&
            p.imageHeight == params.imageHeight)
        {
            v.push_back(p);
        }
    }
    return v;
}


static
tImageCapParams findClosestFramerate(tImageCapParams params,
                                     vector<tImageCapParams> possibleParams)
{
    if (possibleParams.size() == 0)
        throw eRuntimeError("Cannot coerce image capture params to anything.");

    double idealRatio = params.frameIntervalNumerator;
    idealRatio /= params.frameIntervalDenominator;

    tImageCapParams best = possibleParams[0];
    double bestRatio = best.frameIntervalNumerator;
    bestRatio /= best.frameIntervalDenominator;

    for (size_t i = 1; i < possibleParams.size(); i++)
    {
        tImageCapParams p = possibleParams[i];
        double thisRatio = p.frameIntervalNumerator;
        thisRatio /= p.frameIntervalDenominator;

        if (std::fabs(bestRatio-idealRatio) > std::fabs(thisRatio-idealRatio))
        {
            best = p;
            bestRatio = thisRatio;
        }
    }

    return best;
}


refc<iImageCapParamsEnumerator> tImageCapFactory::getImageCapParamsEnumerator()
{
    return refc<iImageCapParamsEnumerator>(new tImageCapParamsEnumerator());
}


refc<iImageCap> tImageCapFactory::getImageCap(
        const tImageCapParams& params,
        bool allowCoercion)
{
    if (!allowCoercion)
        return refc<iImageCap>(new tImageCap(params));

    tImageCapParams p = params;

    refc<iImageCapParamsEnumerator> enumerator = getImageCapParamsEnumerator();
    vector<tImageCapParams> allPossibleParams;
    for (int i = 0; i < enumerator->size(); i++)
        allPossibleParams.push_back((*enumerator)[i]);

    if (enumerator->size() == 0)
        throw eRuntimeError("No params are possible, so this is hopeless.");

    tImageCapParams firstOption = (*enumerator)[0];

    vector<tImageCapParams> possibleParams1, possibleParams2, possibleParams3;

    possibleParams1 = pruneInput(p, allPossibleParams);
    if (possibleParams1.size() == 0)
    {
        p.deviceIndex = firstOption.deviceIndex;
        p.inputIndex = firstOption.inputIndex;
        p.inputDescription = firstOption.inputDescription;
        possibleParams1 = pruneInput(p, allPossibleParams);
        firstOption = possibleParams1[0];
    }

    possibleParams2 = pruneCaptureFmt(p, possibleParams1);
    if (possibleParams2.size() == 0)
    {
        p.captureFormat = firstOption.captureFormat;
        p.captureFormatDescription = firstOption.captureFormatDescription;
        possibleParams2 = pruneCaptureFmt(p, possibleParams1);
        firstOption = possibleParams2[0];
    }

    tImageCapParams correctSize = findClosestSize(p, possibleParams2);
    p.imageHeight = correctSize.imageHeight;
    p.imageWidth = correctSize.imageWidth;
    possibleParams3 = pruneSize(p, possibleParams2);

    tImageCapParams finalParams = findClosestFramerate(p, possibleParams3);

    finalParams.displayFormat = params.displayFormat;
    finalParams.displayFormatDescription = params.displayFormatDescription;

    return refc<iImageCap>(new tImageCap(finalParams));
}


}    // nanespace img
}    // namespace rho
