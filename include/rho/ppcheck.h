#ifndef __rho_ppcheck_h__
#define __rho_ppcheck_h__


#ifndef _FILE_OFFSET_BITS
#error You must define macro _FILE_OFFSET_BITS=64.
#endif

#if _FILE_OFFSET_BITS != 64
#error Macro _FILE_OFFSET_BITS has incorrect value. Should equal 64.
#endif


#endif   // __rho_ppcheck_h__
