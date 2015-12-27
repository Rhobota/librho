/*
 * Copyright (c) 2010, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of Intel Corporation nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Modified by Ryan Henning on April 6, 2015.
 */

#include "iaesni.h"

#include <string.h>


#define AES_INSTRCTIONS_CPUID_BIT (1<<25)


#if IS_x86_FAM
static
void __cpuid(unsigned int where[4], unsigned int leaf)
{
    asm volatile("cpuid":"=a"(*where),"=b"(*(where+1)), "=c"(*(where+2)),"=d"(*(where+3)):"a"(leaf));
    return;
}
#endif


/*
 * check_for_aes_instructions()
 *   return true if support AES-NI and false if don't support AES-NI
 */
bool check_for_aes_instructions()
{
#if IS_x86_FAM
    unsigned int cpuid_results[4];

    //
    // The following is commented out because some AMD processors support the AES-NI
    // instructions too!
    //
//     __cpuid(cpuid_results,0);
//
//     if (cpuid_results[0] < 1)
//         return false;
//
//     /*
//      *      MSB         LSB
//      * EBX = 'u' 'n' 'e' 'G'
//      * EDX = 'I' 'e' 'n' 'i'
//      * ECX = 'l' 'e' 't' 'n'
//      */
//
//     if (memcmp((unsigned char *)&cpuid_results[1], "Genu", 4) != 0 ||
//         memcmp((unsigned char *)&cpuid_results[3], "ineI", 4) != 0 ||
//         memcmp((unsigned char *)&cpuid_results[2], "ntel", 4) != 0)
//     {
//         return false;
//     }

    __cpuid(cpuid_results,1);

    if (cpuid_results[2] & AES_INSTRCTIONS_CPUID_BIT)
        return true;
#endif

    return false;
}
