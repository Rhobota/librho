#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

#include "iaes_asm_interface.h"
#include <wmmintrin.h>

inline
__m128i AES_128_ASSIST (__m128i temp1, __m128i temp2)
{
    __m128i temp3;
    temp2 = _mm_shuffle_epi32 (temp2 ,0xff);
    temp3 = _mm_slli_si128 (temp1, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp1 = _mm_xor_si128 (temp1, temp2);
    return temp1;
}

void AES_128_Key_Expansion (unsigned char *userkey, unsigned char *key)
{
    __m128i temp1, temp2;
    __m128i *Key_Schedule = (__m128i*)key;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    Key_Schedule[0] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1 ,0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[1] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[2] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[3] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[4] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x10);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[5] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x20);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[6] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x40);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[7] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x80);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[8] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x1b);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[9] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x36);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[10] = temp1;
}

inline
void KEY_192_ASSIST(__m128i* temp1, __m128i * temp2, __m128i * temp3)
{
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32 (*temp2, 0x55);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
    *temp2 = _mm_shuffle_epi32(*temp1, 0xff);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, *temp2);
}

void AES_192_Key_Expansion (unsigned char *userkey, unsigned char *key)
{
    __m128i temp1, temp2, temp3;
    __m128i *Key_Schedule = (__m128i*)key;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16));
    Key_Schedule[0]=temp1;
    Key_Schedule[1]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x1);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[1] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[1], (__m128d)temp1,0);
    Key_Schedule[2] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x2);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[3]=temp1;
    Key_Schedule[4]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x4);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[4] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[4], (__m128d)temp1,0);
    Key_Schedule[5] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x8); KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[6]=temp1;
    Key_Schedule[7]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x10);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[7] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[7], (__m128d)temp1,0);
    Key_Schedule[8] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x20);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[9]=temp1;
    Key_Schedule[10]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x40);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[10] = (__m128i)_mm_shuffle_pd((__m128d)Key_Schedule[10], (__m128d)temp1,0);
    Key_Schedule[11] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x80);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    Key_Schedule[12]=temp1;
}

inline
void KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2)
{
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32(*temp2, 0xff);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
}

inline
void KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3)
{
    __m128i temp2,temp4;
    temp4 = _mm_aeskeygenassist_si128 (*temp1, 0x0);
    temp2 = _mm_shuffle_epi32(temp4, 0xaa);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, temp2);
}

void AES_256_Key_Expansion (unsigned char *userkey, unsigned char *key)
{
    __m128i temp1, temp2, temp3;
    __m128i *Key_Schedule = (__m128i*)key;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16));
    Key_Schedule[0] = temp1;
    Key_Schedule[1] = temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x01);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[2]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[3]=temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x02);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[4]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[5]=temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x04);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[6]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[7]=temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x08);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[8]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[9]=temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x10);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[10]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[11]=temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x20);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[12]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    Key_Schedule[13]=temp3;
    temp2 = _mm_aeskeygenassist_si128(temp3, 0x40);
    KEY_256_ASSIST_1(&temp1, &temp2);
    Key_Schedule[14]=temp1;
}

void AES_ECB_encrypt(unsigned char *in, // pointer to the PLAINTEXT
                     unsigned char *out,      // pointer to the CIPHERTEXT buffer
                     size_t length,           // text length in bytes
                     unsigned char *key,         // pointer to the expanded key schedule
                     size_t number_of_rounds) // number of AES rounds 10,12 or 14
{
    __m128i tmp;
    size_t i,j;
    if (length%16)
        length = length/16+1;
    else
        length = length/16;
    for (i=0; i < length; i++)
    {
        tmp = _mm_loadu_si128 (&((__m128i*)in)[i]);
        tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
        for (j=1; j <number_of_rounds; j++)
        {
            tmp = _mm_aesenc_si128 (tmp,((__m128i*)key)[j]);
        }
        tmp = _mm_aesenclast_si128 (tmp,((__m128i*)key)[j]);
        _mm_storeu_si128 (&((__m128i*)out)[i],tmp);
    }
}

void AES_ECB_decrypt(unsigned char *in, //pointer to the CIPHERTEXT
                     unsigned char *out,      //pointer to the DECRYPTED TEXT buffer
                     size_t length,           //text length in bytes
                     unsigned char *key,         //pointer to the expanded key schedule
                     size_t number_of_rounds) //number of AES rounds 10,12 or 14
{
    __m128i tmp;
    size_t i,j;
    if (length%16)
        length = length/16+1;
    else
        length = length/16;
    for (i=0; i < length; i++)
    {
        tmp = _mm_loadu_si128 (&((__m128i*)in)[i]);
        tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
        for (j=1; j <number_of_rounds; j++)
        {
            tmp = _mm_aesdec_si128 (tmp,((__m128i*)key)[j]);
        }
        tmp = _mm_aesdeclast_si128 (tmp,((__m128i*)key)[j]);
        _mm_storeu_si128 (&((__m128i*)out)[i],tmp);
    }
}

void AES_CBC_encrypt(unsigned char *in,
                     unsigned char *out,
                     unsigned char ivec[16],
                     size_t length,
                     unsigned char *key,
                     size_t number_of_rounds)
{
    __m128i feedback,data;
    size_t i,j;
    if (length%16)
        length = length/16+1;
    else
        length /=16;
    feedback = _mm_loadu_si128((__m128i*)ivec);
    for (i=0; i < length; i++)
    {
        data = _mm_loadu_si128 (&((__m128i*)in)[i]);
        feedback = _mm_xor_si128 (data,feedback);
        feedback = _mm_xor_si128 (feedback,((__m128i*)key)[0]);
        for (j=1; j <number_of_rounds; j++)
        {
            feedback = _mm_aesenc_si128 (feedback,((__m128i*)key)[j]);
        }
        feedback = _mm_aesenclast_si128 (feedback,((__m128i*)key)[j]);
        _mm_storeu_si128 (&((__m128i*)out)[i],feedback);
    }
}

void AES_CBC_decrypt(unsigned char *in,
                     unsigned char *out,
                     unsigned char ivec[16],
                     size_t length,
                     unsigned char *key,
                     size_t number_of_rounds)
{
    __m128i data,feedback,last_in;
    size_t i,j;
    if (length%16)
        length = length/16+1;
    else
        length /=16;
    feedback = _mm_loadu_si128((__m128i*)ivec);
    for (i=0; i < length; i++)
    {
        last_in=_mm_loadu_si128 (&((__m128i*)in)[i]);
        data = _mm_xor_si128 (last_in,((__m128i*)key)[0]);
        for (j=1; j <number_of_rounds; j++)
        {
            data = _mm_aesdec_si128 (data,((__m128i*)key)[j]);
        }
        data = _mm_aesdeclast_si128 (data,((__m128i*)key)[j]);
        data = _mm_xor_si128 (data,feedback);
        _mm_storeu_si128 (&((__m128i*)out)[i],data);
        feedback=last_in;
    }
}

void iEncExpandKey128(UCHAR *key, UCHAR *expanded_key)
{
    AES_128_Key_Expansion(key, expanded_key);
}

void iEncExpandKey192(UCHAR *key, UCHAR *expanded_key)
{
    AES_192_Key_Expansion(key, expanded_key);
}

void iEncExpandKey256(UCHAR *key, UCHAR *expanded_key)
{
    AES_256_Key_Expansion(key, expanded_key);
}

void iDecExpandKey128(UCHAR *key, UCHAR *expanded_key)
{
    AES_128_Key_Expansion(key, expanded_key);
}

void iDecExpandKey192(UCHAR *key, UCHAR *expanded_key)
{
    AES_192_Key_Expansion(key, expanded_key);
}

void iDecExpandKey256(UCHAR *key, UCHAR *expanded_key)
{
    AES_256_Key_Expansion(key, expanded_key);
}

void iEnc128(sAesData *data)
{
    AES_ECB_encrypt(data->in_block,
                    data->out_block,
                    data->num_blocks*16,
                    data->expanded_key,
                    10);
}

void iDec128(sAesData *data)
{
}

void iEnc192(sAesData *data)
{
    AES_ECB_encrypt(data->in_block,
                    data->out_block,
                    data->num_blocks*16,
                    data->expanded_key,
                    12);
}

void iDec192(sAesData *data)
{
}

void iEnc256(sAesData *data)
{
    AES_ECB_encrypt(data->in_block,
                    data->out_block,
                    data->num_blocks*16,
                    data->expanded_key,
                    14);
}

void iDec256(sAesData *data)
{
}

void iEnc128_CBC(sAesData *data)
{
    AES_CBC_encrypt(data->in_block,
                    data->out_block,
                    data->iv,
                    data->num_blocks*16,
                    data->expanded_key,
                    10);
}

void iDec128_CBC(sAesData *data)
{
}

void iEnc192_CBC(sAesData *data)
{
    AES_CBC_encrypt(data->in_block,
                    data->out_block,
                    data->iv,
                    data->num_blocks*16,
                    data->expanded_key,
                    12);
}

void iDec192_CBC(sAesData *data)
{
}

void iEnc256_CBC(sAesData *data)
{
    AES_CBC_encrypt(data->in_block,
                    data->out_block,
                    data->iv,
                    data->num_blocks*16,
                    data->expanded_key,
                    14);
}

void iDec256_CBC(sAesData *data)
{
}

#pragma GCC diagnostic pop
