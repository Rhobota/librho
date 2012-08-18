#include <rho/sync/tMutex.h>
#include <rho/sync/tAtomicInt.h>

#include <map>


namespace rho
{


    std::map<void*, rho::sync::au32*> gAllKnownRefcObjectsMap;

    rho::sync::tMutex                 gAllKnownRefcObjectsSync;


}   // namespace rho
