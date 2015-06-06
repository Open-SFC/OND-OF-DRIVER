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
 * File name: ucmmd5.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the data structure definitions
 *              and function prototypes for MD5 message-digest
 *              encryption algorithm.
 * 
 */

#ifndef _MD5_H_
#define _MD5_H_

typedef unsigned long uint32;
#if 0
struct UCMMD5Context {
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
};
#endif

#define  UCMMD5Context EVP_MD_CTX 
#define  NAT_PASSWORD_LEN 33 

typedef struct UCMMD5Context UCMMD5Context_t;

void byteReverse (unsigned char *buf, unsigned longs);

void cm_md5_encrypt_buf(char* inStr, char* outStr);
void cm_md5_encrypt(unsigned char* inStr, uint32_t uiInStrLen, unsigned char* digest);
void cm_md5_init(UCMMD5Context *context);
void cm_md5_update(UCMMD5Context *context, unsigned char const *buf,
               unsigned len);
void cm_md5_final(unsigned char digest[16], UCMMD5Context *context);
void MD5Transform (uint32 buf[4], uint32 const in[16]);
/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
#define  UCMMD5_CTX EVP_MD_CTX

#define CM_MD5   1
#define CM_SHA1  2

#endif  /*#ifndef _MD5_H_*/
