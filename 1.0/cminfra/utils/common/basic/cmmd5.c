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
 * File name: ucmmd5.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This code provides APIs for MD5 and internally uses
 *              openssl library.
 * 
 */

#include "cmincld.h"
#include "openssl/evp.h"
#include "ucmmd5.h"
#include <openssl/evp.h>

#ifdef UCMMISC_DEBUG
#define UCMMISC_Printf(x) printf("%s:%d#");printf x
#else
#define UCMMISC_Printf(x)
#endif
int32_t cm_openssl_compute_hash(unsigned char *pInData, int32_t lInDataLen,
                           unsigned char  ucDgstType,
                           unsigned char *pDgstData, int32_t *pDgstDataLen);


void cm_md5_encrypt(unsigned char* inStr, uint32_t uiInStrLen, unsigned char* digest)
{
  EVP_MD_CTX  ctx;
  int32_t iDgstLen=0;

  EVP_DigestInit(&ctx, EVP_md5());
  EVP_DigestUpdate(&ctx, inStr, uiInStrLen);
  EVP_DigestFinal(&ctx, digest, &iDgstLen);
  return;
}  

void cm_md5_encrypt_buf(char* inStr, char* outStr)
{
  EVP_MD_CTX  ctx;
  unsigned char digest[16];
  int32_t iDgstLen=0;
  uint16_t len =0;
  int16_t  i,j;

  len = strlen ((char*)inStr);

  EVP_DigestInit(&ctx, EVP_md5());
  EVP_DigestUpdate(&ctx, inStr, len);
  EVP_DigestFinal(&ctx, digest, &iDgstLen);

  for (i = 0, j = 0; i < 16; i++)
  {
    sprintf((char*)&outStr[j],"%02x", digest[i]);
    j += 2;
  } 
  outStr[NAT_PASSWORD_LEN - 1] = '\0';
  return;
}  

int32_t cm_openssl_compute_hash(unsigned char *pInData, int32_t lInDataLen,
                           unsigned char  ucDgstType,
                           unsigned char *pDgstData, int32_t *pDgstDataLen)
{
  EVP_MD_CTX  ctx;

  /*
   * Get the digest type.
   * If we are supporting, generate the digest
   */
  switch(ucDgstType)
  {
    case CM_MD5:
      EVP_DigestInit(&ctx, EVP_md5());
      break;
    case CM_SHA1:
      EVP_DigestInit(&ctx, EVP_sha1());
      break;
    default:
    UCMMISC_Printf("cm_openssl_compute_hash: The algorithm specified is not supported\n");
      return OF_FAILURE;
  }

  EVP_DigestUpdate(&ctx, pInData, lInDataLen);
  EVP_DigestFinal(&ctx, pDgstData, pDgstDataLen);

  return OF_SUCCESS;
}

void cm_md5_init(UCMMD5Context *ctx)
{
  EVP_DigestInit(ctx, EVP_md5());
}  

void cm_md5_update(UCMMD5Context *ctx,
                unsigned char const *buf,
                unsigned len)
{

  EVP_DigestUpdate(ctx, buf, len);
  return;
}  

void cm_md5_final(unsigned char digest[16],
              UCMMD5Context *ctx)
{
  int32_t iDgstlen=0;

  EVP_DigestFinal(ctx, digest,&iDgstlen );
  return;
}
  

