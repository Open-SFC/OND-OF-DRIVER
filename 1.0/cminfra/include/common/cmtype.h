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
 * File name: ucmtype.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

#ifndef CM_TYPE_H
#define CM_TYPE_H
#if 0
typedef uint32_t   T_ULONG;
typedef uint64_t   T_UINT64;
typedef int64_t   T_INT64;
typedef uint32_t ULONG;
//typedef unsigned int T_UINT32;
/* Actually, T_UINT32 variable says that we have to use a storage variable which
   can store the value of 32 bits. But, on 64 bit platfrom , if this T_UINT32 is defined
 'unsigned long', then it'll occupy 64 bits in stack. But, while framming and 
  and receiving buffers we assumed as 32 bits and used of_mbuf_put_32 and of_mbuf_get_32.
  Here while getting the data we have to move only 32 bits. But, some places,
  it is used as 'sizeof(T_UINT32)'. Due to this we are moving 64bits due to
  which we are accessing wrong fields.
  Also, we are not really making use of 64 bit value. Because, we are just copying
  32bits .
  On 32 bit platform both 'unsigned long' and 'unsigned int' are occupying only 4 bytes.
  Hence, whether we use explicit '4' or sizeof(T_UINT32) , it does not cause any issue.
  Current issue happenned because in UCM this was defined as 'unsigned int'. 
  In SPUTM core this is defined as 'unsigned long'.
  Solution:
   As the T_UINT32 storage name specifies 32 bits, in case of 64bits platform
  at compile time this should be defined as 'unsigned int'. 
  In 32 bit platform, this can be left as 'unsigned long'.
  However, MGMT card should be tested once , by changing T_UINT32 as 
  'unsigned int' in both UCM and SPUTM core.
  For p4080, it does not matter as it occupies, 4 bytes. 
  */ 
#ifndef CM_MGMT_CARD_SUPPORT
typedef uint32_t T_UINT32;
#else
typedef uint32_t T_UINT32;
#endif
typedef int32_t T_INT32;
typedef int16_t T_INT16;
typedef uint16_t T_UINT16;
typedef char T_CHAR8;
typedef unsigned char T_UCHAR8;
typedef unsigned char T_BOOL8;
typedef void T_VOID;
typedef uint32_t ipaddr;

/* IP Network address */
typedef uint32_t UCMIPAddr_t; /*Need to support IPv6*/
typedef uint8_t T_UINT8;
typedef int8_t T_INT8;

#ifdef OF_XML_SUPPORT
typedef int BOOL;
#endif

#endif
/*SOWJI KSPACE*/
#define CM_TICKS2SEC 100 /*HZ*/
#define CM_USED_BY_NON_INTR 0x02
/* UCM True and False  */
//#define TRUE 1
//#define FALSE 0

//#define NULLSTRING ""
/* UCM Success Error */
//#define OF_SUCCESS 0
//#define OF_FAILURE (-1)

/* I/O */
/*
#define printf printf
#define fprintf fprintf
#define sprintf sprintf
#define exit exit

#define fputs fputs
#define fgetc fgetc
#define perror perror

#define sizeof sizeof

// SIGNALS
#define signal signal
#define sigaddset sigaddset
#define sigaction sigaction
#define sigfillset sigfillset
#define sigsetjmp sigsetjmp
#define alarm alarm

#define sscanf                       sscanf
#define snprintf                      snprintf
#define isspace                       isspace
#define toupper                       toupper
#define tolower                       tolower
#define isprint                       isprint

#define isspace isspace
#ifndef __KERNEL__
#define isalpha isalpha
#define isdigit isdigit
#define isalnum isalnum
#define tolower tolower
#define toupper toupper
#define isupper isupper
#define islower islower
#endif

*/
#endif
