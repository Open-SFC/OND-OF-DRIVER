/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

/*!\file of_cntlr_tls.h
 *\author 
 *\date May, 2013  
 * <b>Description:</b>\n 
 * This file contains SSL transport layer functionality definitions
 */
#ifndef _CNTLR_TLS_H
#define _CNTLR_TLS_H

#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#define OF_SSL_CIPHER_SUITE                1
#define OF_SSL_OPERATION                   2
#define OF_SSL_PEER_CERT_REQUIRED          3
#define OF_SSL_SET_CERT_VERIFICATION_DEPTH 4
#define OF_SSL_SESSION_CACHING             5
#define OF_SSL_SESSION_CACHE_SIZE          6
 
#define OF_SSL_SET_CACERT                  1
#define OF_SSL_SET_PRIVATE_KEY             2
#define OF_SSL_SET_CERTIFICATE             3
#define OF_SSL_SET_PRIVATE_KEY_PASS        4 

#define OF_SSL_CIPHERLIST_MAX_LEN          256
#define OF_SSL_MAX_CIPHERS                 7
#define OF_SSL_MAX_OPTIONS                 5

#define OF_SSL_MAX_NIDS                    7

#define NID_countryName                 14   /*"C"*/
#define NID_stateOrProvinceName         16   /*"ST"*/
#define NID_localityName                15   /*"L"*/ 
#define NID_organizationName            17   /*"O"*/
#define NID_organizationalUnitName      18   /*"OU"*/
#define NID_commonName                  13   /*"CN"*/
#define NID_pkcs9_emailAddress          48   /*"emailAddress"*/

#define NID_issuer_alt_name             86   /*"X509v3 Issuer Alternative Name"*/
#define NID_subject_alt_name            85   /*"X509v3 Subject Alternative Name"*/

typedef struct of_subjectname_fields_s
{
  int nid;
  char *pfield;
}of_subjectname_fields_t;

int of_ssl_init();
void of_ssl_init_library();

int  of_ssl_create_newcontext(SSL_CTX **ctx, unsigned char state);
int  of_ssl_set_ctx_options(SSL_CTX *ctx,unsigned int option,unsigned int value);
int  of_ssl_set_certificates(SSL_CTX *ctx,unsigned int option,char *pvalue1,char *pvalue2);

int  of_ssl_set_server_listen(int *tcpsocket,unsigned int localip,unsigned short localport);
int  of_ssl_accept_client(int iSockFd,unsigned int *pRmtIp,unsigned short *pusRmtPort);
int  of_ssl_get_sslobject_for_client(int clientFd,SSL_CTX *ctx,SSL **ssl);

int  of_peer_cert_verify_callback(int ok,X509_STORE_CTX *ctx);
void of_apps_sslinfo_callback(SSL *s,int where,int ret);

int of_ssl_pem_passwd_cb(char *buf,int size,int rwflag,void *password);
int of_ssl_shutdown_connection(SSL *ssl, SSL_CTX *ctx);
int        
of_ssl_verify_peer_subjectname(SSL *ssl, uint64_t *pdatapath_id);

#endif
