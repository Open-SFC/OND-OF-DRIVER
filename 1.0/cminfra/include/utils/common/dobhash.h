/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: dobhash.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: Dob Hash algorithms.
 * 
 */

#ifndef _DOBHASH_H_
#define _DOBHASH_H_

#define CM_DOBBS_MIX(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/* Hash computation for a string/array 
 */
#define CM_DOBBS_HASH( str_p, length, initval, hashmask, hash) \
{ \
\
  register uint32_t temp_a, temp_b, temp_c,temp_len;  \
  unsigned char *k = str_p; \
\
  /* Set up the internal state */ \
  temp_len = length;\
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/\
\
  /*---------------------------------------- handle most of the key */\
  while (temp_len >= 12) \
  { \
    temp_a += (k[0] +((uint32_t)k[1]<<8) +((uint32_t)k[2]<<16) +\
              ((uint32_t)k[3]<<24));\
    temp_b += (k[4] +((uint32_t)k[5]<<8) +((uint32_t)k[6]<<16) +\
              ((uint32_t)k[7]<<24));\
    temp_c += (k[8] +((uint32_t)k[9]<<8) +((uint32_t)k[10]<<16) +\
              ((uint32_t)k[11]<<24));\
    CM_DOBBS_MIX(temp_a,temp_b,temp_c);\
    k += 12; temp_len -= 12;\
  }\
\
  /*------------------------------------- handle the last 11 bytes */\
  temp_c += length;\
  switch(temp_len)              /* all the case statements fall through */\
  {\
    case 11: temp_c+=((uint32_t)k[10]<<24);\
    case 10: temp_c+=((uint32_t)k[9]<<16);\
    case 9 : temp_c+=((uint32_t)k[8]<<8);\
             /* the first byte of c is reserved for the length */\
    case 8 : temp_b+=((uint32_t)k[7]<<24);\
    case 7 : temp_b+=((uint32_t)k[6]<<16);\
    case 6 : temp_b+=((uint32_t)k[5]<<8);\
    case 5 : temp_b+=k[4];\
    case 4 : temp_a+=((uint32_t)k[3]<<24);\
    case 3 : temp_a+=((uint32_t)k[2]<<16);\
    case 2 : temp_a+=((uint32_t)k[1]<<8);\
    case 1 : temp_a+=k[0];\
             /* case 0: nothing left to add */\
  }\
  CM_DOBBS_MIX(temp_a,temp_b,temp_c);\
  /*-------------------------------------------- report the result */\
  hash = temp_c & (hashmask-1);\
}

/* Hash computation for 2 words */
#define CM_DOBBS_WORDS2(word_a, word_b, initval, \
                         hashmask, hash) \
{ \
  register uint32_t temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
\
  temp_a += word_a; \
  temp_b += word_b; \
\
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  hash = temp_c & (hashmask-1); \
}

#define CM_DOBBS_WORDS3(word_a, word_b, word_c, initval, \
                         hashmask, hash) \
{ \
  register uint32_t temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
\
  temp_a += word_a; \
  temp_b += word_b; \
  temp_c += word_c; \
\
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  hash = temp_c & (hashmask-1); \
}

#define CM_DOBBS_WORDS4(word_a, word_b, word_c, word_d, initval, \
                         hashmask, hash) \
{ \
  register uint32_t temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
\
  temp_a += word_a; \
  temp_b += word_b; \
  temp_c += word_c; \
\
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  temp_a += word_d; \
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  hash = temp_c & (hashmask-1); \
}

#define CM_DOBBS_WORDS5(word_a, word_b, word_c, word_d, word_e, initval, \
                         hashmask, hash) \
{ \
  register uint32_t temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
\
  temp_a += word_a; \
  temp_b += word_b; \
  temp_c += word_c; \
\
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  temp_a += word_d; \
  temp_b += word_e; \
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  hash = temp_c & (hashmask-1); \
}

#define CM_DOBBS_WORDS6(word_a, word_b, word_c, word_d, word_e, word_f, \
                         initval, hashmask, hash) \
{ \
  register uint32_t temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
\
  temp_a += word_a; \
  temp_b += word_b; \
  temp_c += word_c; \
\
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  temp_a += word_d; \
  temp_b += word_e; \
  temp_c += word_f; \
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  hash = temp_c & (hashmask-1); \
}

#define CM_DOBBS_WORDS7(word_a, word_b, word_c, word_d, word_e, word_f, \
                         word_g, initval, hashmask, hash) \
{ \
  register uint32_t temp_a, temp_b, temp_c; \
  temp_a = temp_b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */\
  temp_c = initval;         /* random value*/ \
\
  temp_a += word_a; \
  temp_b += word_b; \
  temp_c += word_c; \
\
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  temp_a += word_d; \
  temp_b += word_e; \
  temp_c += word_f; \
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  temp_a += word_g; \
  CM_DOBBS_MIX(temp_a, temp_b, temp_c); \
\
  hash = temp_c & (hashmask-1); \
}

#endif /* _DOBHASH_H_ */
