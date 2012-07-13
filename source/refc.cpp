#include "rho/sync/tMutex.h"

#include "rho/sync/tAtomicInt.h"

#include <map>


std::map<void*, rho::sync::au32*> rho::gAllKnownRefcObjectsMap;

rho::sync::tMutex                 rho::gAllKnownRefcObjectsSync;
