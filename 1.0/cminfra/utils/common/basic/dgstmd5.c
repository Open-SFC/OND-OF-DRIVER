/* DIGEST-MD5 SASL plugin
 * Ken Murchison
 * Rob Siemborski
 * Tim Martin
 * Alexey Melnikov 
 * $Id: dgstmd5.c,v 1.1.2.1 2013/09/26 10:28:57 b38863 Exp $
 */
/* 
 * Copyright (c) 1998-2003 Carnegie Mellon University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The name "Carnegie Mellon University" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For permission or any other legal
 *    details, please contact  
 *      Office of Technology Transfer
 *      Carnegie Mellon University
 *      5000 Forbes Avenue
 *      Pittsburgh, PA  15213-3890
 *      (412) 268-4387, fax: (412) 268-7395
 *      tech-transfer@andrew.cmu.edu
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Computing Services
 *     at Carnegie Mellon University (http://www.cmu.edu/computing/)."
 *
 * CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "cmincld.h"
#include "openssl/evp.h"
#include "ucmmd5.h"
#include "dgstmd5.h"

#define bool int

#define MD5BLKLEN 64		/* MD5 block length */
#define MD5DIGLEN 16		/* MD5 digest length */
/*****************************  Common Section  *****************************/

/* defines */
#define HASHLEN 16
typedef unsigned char HASH[HASHLEN + 1];
#define HASHHEXLEN 32
typedef unsigned char HASHHEX[HASHHEXLEN + 1];

static const unsigned char *COLON = ":";

static void CvtHex(HASH Bin, HASHHEX Hex);
static void DigestCalcSecret(unsigned char *pszUserName,
			                 unsigned char *pszRealm,
			                 unsigned char *password_p,
			                 int PasswordLen,
			                 HASH H);
static void DigestCalcHA1(unsigned char *pszUserName,
			              unsigned char *pszRealm,
			              unsigned char *pszPassword,
                          uint32         len,
			              unsigned char *pszAuthorization_id,
			              unsigned char *pszNonce,
			              unsigned char *pszCNonce,
			              HASHHEX SessionKey
                        );
void DigestCalcResponse(HASHHEX HA1HEX,	/* HEX(H(A1)) */
		           unsigned char *pszNonce,	/* nonce from server */
		           unsigned char *cNonceValue,	/* 8 hex digits */
		           unsigned char *pszCNonce,	/* client nonce */
		           unsigned char *pszQop,	/* qop-value: "", "auth",
						 * "auth-int" */
		           unsigned char *pszDigestUri,	/* requested URL */
		           unsigned char *pszMethod,
		           HASHHEX Response	/* request-digest or response-digest */
                  );
void unicode_to_utf8(const unsigned char *in, int len, unsigned char *out, int *poutlen, int maxlen);
int utf8_to_unicode(const unsigned char *in, unsigned char *out, unsigned short maxlen);

void  AsciiToUnicode(unsigned char *pDest,unsigned char *pSrc);

static int asciiToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen);
                        

static void CvtHex(HASH Bin, HASHHEX Hex)
{
    unsigned short  i;
    unsigned char   j;
    
    for (i = 0; i < HASHLEN; i++) {
	j = (Bin[i] >> 4) & 0xf;
	if (j <= 9)
	    Hex[i * 2] = (j + '0');
	else
	    Hex[i * 2] = (j + 'a' - 10);
	j = Bin[i] & 0xf;
	if (j <= 9)
	    Hex[i * 2 + 1] = (j + '0');
	else
	    Hex[i * 2 + 1] = (j + 'a' - 10);
    }
    Hex[HASHHEXLEN] = '\0';
}

/*
 * calculate request-digest/response-digest as per HTTP Digest spec
 */
void
DigestCalcResponse(HASHHEX HA1HEX,	/* HEX(H(A1)) */
		           unsigned char *pszNonce,	/* nonce from server */
		           unsigned char *cNonceValue,	/* 8 hex digits */
		           unsigned char *pszCNonce,	/* client nonce */
		           unsigned char *pszQop,	/* qop-value: "", "auth",
						 * "auth-int" */
		           unsigned char *pszDigestUri,	/* requested URL */
		           unsigned char *pszMethod,
		           HASHHEX Response	/* request-digest or response-digest */
    )
{
    UCMMD5_CTX      Md5Ctx;
    HASH            HA2;
    HASH            RespHash;
    HASHHEX         HA2Hex;
    
    /* calculate H(A2) */
    cm_md5_init(&Md5Ctx);
    
    if (pszMethod != NULL) {
	cm_md5_update(&Md5Ctx, pszMethod, strlen((char *) pszMethod));
    }
    cm_md5_update(&Md5Ctx, (unsigned char *) COLON, 1);
    cm_md5_update(&Md5Ctx, pszDigestUri, strlen((char *) pszDigestUri));
    cm_md5_final(HA2, &Md5Ctx);
    CvtHex(HA2, HA2Hex);
    
    /* calculate response */
    cm_md5_init(&Md5Ctx);
    cm_md5_update(&Md5Ctx, HA1HEX, HASHHEXLEN);
    cm_md5_update(&Md5Ctx, COLON, 1);
    cm_md5_update(&Md5Ctx, pszNonce, strlen((char *) pszNonce));
    cm_md5_update(&Md5Ctx, COLON, 1);
    if (*pszQop) {
/*	sprintf(ncvalue, "%08x", pszNonceCount); */
	cm_md5_update(&Md5Ctx, cNonceValue, strlen(cNonceValue));
	cm_md5_update(&Md5Ctx, COLON, 1);
	cm_md5_update(&Md5Ctx, pszCNonce, strlen((char *) pszCNonce));
	cm_md5_update(&Md5Ctx, COLON, 1);
	cm_md5_update(&Md5Ctx, pszQop, strlen((char *) pszQop));
	cm_md5_update(&Md5Ctx, COLON, 1);
    }
    cm_md5_update(&Md5Ctx, HA2Hex, HASHHEXLEN);
    cm_md5_final(RespHash, &Md5Ctx);
    CvtHex(RespHash, Response);
}

static void DigestCalcSecret(unsigned char *pszUserName,
			                 unsigned char *pszRealm,
			                 unsigned char *password_p,
			                 int PasswordLen,
			                 HASH H)
{
    UCMMD5_CTX     Md5Ctx;
    int32_t        lOutlen = 0; 
    int32_t        lInlen = 0;

    unsigned char *pUser,*pRealm,*pPass;
   /* unsigned char cRealm[] = {}; */
    

    cm_md5_init(&Md5Ctx);

    if((pUser = of_calloc(of_strlen(pszUserName), 2)) == NULL)
       return;
  
    lInlen = of_strlen(pszUserName);		 
    lOutlen = lInlen *2;;
    asciiToUTF8(pUser,&lOutlen, pszUserName, &lInlen);
    cm_md5_update(&Md5Ctx, pUser, lOutlen);
    cm_md5_update(&Md5Ctx, COLON, 1);
    
    if((pRealm = of_calloc(of_strlen(pszRealm), 2)) == NULL)
       {
          of_free(pUser); 
          return;
    	}

    lInlen = of_strlen(pszRealm);		 
    lOutlen = lInlen*2;
    asciiToUTF8(pRealm,&lOutlen,pszRealm,&lInlen);
    cm_md5_update(&Md5Ctx, pRealm, lOutlen);
    cm_md5_update(&Md5Ctx, COLON, 1);
    
    if((pPass = of_calloc(of_strlen(password_p), 2)) == NULL)
       {
          of_free(pUser); 
          of_free(pRealm); 
          return;
    	}
    lInlen = of_strlen(password_p);		 
    lOutlen = lInlen*2;
    asciiToUTF8(pPass,&lOutlen,password_p,&lInlen);
    cm_md5_update(&Md5Ctx, pPass, lOutlen);

    of_free(pUser); 
    of_free(pRealm); 
    of_free(pPass); 

    cm_md5_final(H, &Md5Ctx);
}

/* calculate HEX(H(A1)) as per spec */
static void DigestCalcHA1(unsigned char *pszUserName,
			              unsigned char *pszRealm,
			              unsigned char *pszPassword,
                          uint32         len,
			              unsigned char *pszAuthorization_id,
			              unsigned char *pszNonce,
			              unsigned char *pszCNonce,
			              HASHHEX HA1HEX 
                        )
{
    UCMMD5_CTX      Md5Ctx;
    HASH            H;
    HASH            HA1;
    
    DigestCalcSecret(pszUserName,
		             pszRealm,
		             pszPassword,
		             len,
		             H);
    
    /* calculate the session key */
    cm_md5_init(&Md5Ctx);
    cm_md5_update(&Md5Ctx, H, HASHLEN);
    cm_md5_update(&Md5Ctx, COLON, 1);
    cm_md5_update(&Md5Ctx, pszNonce, strlen((char *) pszNonce));
    cm_md5_update(&Md5Ctx, COLON, 1);
    cm_md5_update(&Md5Ctx, pszCNonce, strlen((char *) pszCNonce));
    if (pszAuthorization_id != NULL) {
	cm_md5_update(&Md5Ctx, COLON, 1);
	cm_md5_update(&Md5Ctx, pszAuthorization_id, 
			 strlen((char *) pszAuthorization_id));
    }
    cm_md5_final(HA1, &Md5Ctx);
    
    CvtHex(HA1, HA1HEX);
     
}

void CalculateResponse( unsigned char *passwd,
                         uint32         len,  
    		         unsigned char *realm,
    		         unsigned char *nonce,
    		         unsigned char *ncvalue,
    		         unsigned char *cnonce,
    		         char *qop,
    		         unsigned char *digesturi,
    		         unsigned char *authorization_id,
                         unsigned char  *result,
                         unsigned char  *pusername
			)
{
    HASHHEX         HA1HEX;
    HASHHEX         Response;
    
    /* Verifing that all parameters was defined */
    if(!pusername || !cnonce || !nonce || !ncvalue || !digesturi || !passwd)
    {
	  return;
    }
    printf("Username = %s\r\n",pusername);
    
    if (realm == NULL) {

	realm = (unsigned char *) "";
    }
    
    if (qop == NULL) {
	/* default to a qop of just authentication */
	qop = "auth";
    }
    
    DigestCalcHA1(pusername,
		          realm,
		          passwd,
                  len,
		          authorization_id,
		          nonce,
		          cnonce,
		          HA1HEX);
    
     DigestCalcResponse(HA1HEX,/* HEX(H(A1)) */
		       nonce,	/* nonce from server */
		       ncvalue,	/* 8 hex digits */
		       cnonce,	/* client nonce */
		       (unsigned char *) qop,	/* qop-value: "", "auth",
						 * "auth-int" */
		       digesturi,	/* requested URL */
		       (unsigned char *) "AUTHENTICATE",
		       Response	/* request-digest or response-digest */
	);
    
    memcpy(result, Response, HASHHEXLEN);
    
    return;
}


/*
 *  utf8_to_unicode -- converts a UTF-8 string to Unicode
 *
 *   input:  in = pointer to the input UTF-8 zero-terminated string
 *           out = pointer to where the resulting Unicode string should be saved
 *           maxlen = max number of bytes to save
 *
 *   return: size of Unicode string (in wchars)
 */
int utf8_to_unicode(const unsigned char *in, unsigned char *out, unsigned short maxlen)
{
	int n;
	int len;
	int size;
	int need = 0;
	unsigned char ch;
	unsigned short uc = 0;

	len = strlen(in);
	size = 0;

	for(n = 0; n < len; ++n) {
		ch = in[n];
		if(need) {
			if((ch&0xc0) == 0x80) {
				uc = (uc << 6) | (ch & 0x3f);
				need--;
				if(!need) {
					if(size + 2 > maxlen)
						return size;

					out[size++] = ((uc >> 8) & 0xff);
					out[size++] = (uc & 0xff);
				}
			}
			else {
				need = 0;
			}
		}
		else {
			if(ch < 128) {
				if(size + 2 > maxlen)
					return size;

				out[size++] = 0;
				out[size++] = ch;
			}
			else if( (ch&0xe0) == 0xc0 ) {
				uc = ch & 0x1f;
				need = 1;
			}
			else if( (ch&0xf0) == 0xe0 ) {
				uc = ch & 0x0f;
				need = 2;
			}
		}
	}

	return (size >> 1);
}

/*
 *  unicode_to_utf8 -- converts a Unicode string to UTF-8
 *
 *   input:  in = pointer to the input Unicode string
 *           len = length of Unicode string (in wchars)
 *           out = pointer to where the resulting UTF-8 zero-terminated string should be saved
 *           maxlen = max number of bytes to save
 *
 *   return: none
 */
void unicode_to_utf8(const unsigned char *in, int len, unsigned char *out, int *poutlen, int maxlen)
{
	int n, size;
	unsigned short uc;
	unsigned char hi, lo, b;
 
	size = 0;
 
	for(n = 0; n < len; ++n) {
		lo = in[n*2+1];
		hi = in[n*2];
		printf("lo = %c, hi = %c\r\n",lo,hi);
		uc = ((unsigned short)hi << 8) + lo;
		if(!hi && lo < 0x80) {
			if(size + 1 >= maxlen)
				break;
			out[size++] = lo;
		}
		else {
			b = (hi << 2) | (lo >> 6);
			if(hi < 0x08) {
				if(size + 1 >= maxlen)
					break;
				out[size++] = 0xc0 | b;
			}
			else {
				if(size + 2 >= maxlen)
					break;
				out[size++] = 0xe0 | (hi >> 4);
				out[size++] = 0x80 | (b & 0x3f);
			}
			if(size + 1 >= maxlen)
				break;
			out[size++] = 0x80 | (lo & 0x3f);
		}
	}
 
	out[size] = 0;
	*poutlen = size -1;
}

void  AsciiToUnicode(unsigned char *pDest,unsigned char *pSrc)
{
    int32_t jj;
	
    for(jj=0;jj<of_strlen(pSrc);jj++)
    {
      pDest[jj*2] = pSrc[jj];
    }
   return; 
}

static int asciiToUTF8(unsigned char* out, int *outlen,
	               const unsigned char* in, int *inlen)
{
    unsigned char* outstart = out;
    const unsigned char* base = in;
    const unsigned char* processed = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    unsigned int c;
    int bits;

    inend = in + (*inlen);
    while ((in < inend) && (out - outstart + 4 < *outlen))
    {
	c= *in++;

	/* assertion: c is a single UTF-4 value */
        if (out >= outend)
	    break;
        if (c < 0x80)
       	{  *out++=  c; 
 	   bits= -6; 
	}
        else
       	{ 
	    *outlen = out - outstart;
	    *inlen = processed - base;
	    return(-1);
	}
 
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlen = processed - base;
    return(0);
}

char *hmacmd5 (char *text,unsigned long tl,char *key,unsigned long kl)
{
  int i,j;
  static char hshbuf[2*MD5DIGLEN + 1];
  char *s;
  UCMMD5_CTX      ctx;
  char *hex = "0123456789abcdef";
  unsigned char digest[MD5DIGLEN],k_ipad[MD5BLKLEN+1],k_opad[MD5BLKLEN+1];
  
  if (kl > MD5BLKLEN) {		/* key longer than pad length? */
    cm_md5_init(&ctx);		/* yes, set key as MD5(key) */
    cm_md5_update(&ctx,(unsigned char *) key,kl);
    cm_md5_final(digest,&ctx);
    key = (char *) digest;
    kl = MD5DIGLEN;
  }
  of_memcpy (k_ipad,key,kl);	/* store key in pads */
  of_memset (k_ipad+kl,0,(MD5BLKLEN+1)-kl);
  of_memcpy (k_opad,k_ipad,MD5BLKLEN+1);
				/* XOR key with ipad and opad values */
  for (i = 0; i < MD5BLKLEN; i++) {
    k_ipad[i] ^= 0x36;
    k_opad[i] ^= 0x5c;
  }
  cm_md5_init(&ctx);		/* inner MD5: hash ipad and text */
  cm_md5_update(&ctx,k_ipad,MD5BLKLEN);
  cm_md5_update(&ctx,(unsigned char *) text,tl);
  cm_md5_final(digest,&ctx);
  cm_md5_init(&ctx);		/* outer MD5: hash opad and inner results */
  cm_md5_update(&ctx,k_opad,MD5BLKLEN);
  cm_md5_update(&ctx,digest,MD5DIGLEN);
  cm_md5_final(digest,&ctx);
				/* convert to printable hex */
  for (i = 0, s = hshbuf; i < MD5DIGLEN; i++) {
    *s++ = hex[(j = digest[i]) >> 4];
    *s++ = hex[j & 0xf];
  }
  *s = '\0';			/* tie off hash text */
  return hshbuf;
}

