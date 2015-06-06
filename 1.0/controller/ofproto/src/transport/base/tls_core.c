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

/*!\file cntlr_tls.c
 *\author 
 *\date May, 2013  
 * <b>Description:</b>\n 
 * This file contains SSL transport layer functionality definitions
 */

/* INCLUDE_START ************************************************************/
#include "controller.h"
#include "cntlr_tls.h"
#include "cntlr_event.h"
#include "cntrl_queue.h"
#include "cntlr_transprt.h"
#include "dprmldef.h"
#include "cntrucmssl.h"

/* INCLUDE_END **************************************************************/

extern struct   mchash_table* dprm_datapath_table_g;

SSL_CTX *ctx;  /* Only one SSL_CTX context for the entire Server */ 

char ofssl_CipherSuite[OF_SSL_MAX_CIPHERS][OF_SSL_CIPHERLIST_MAX_LEN] =
                                     {
                                       "DES-CBC-MD5",
                                       "DES-CBC-SHA",
                                       "DES-CBC3-MD5",
                                       "DES-CBC3-SHA",
                                       "AES128-SHA",
                                       "AES256-SHA",
                                       "ALL"
                                     };

int32_t  ofssl_SSLOptions[OF_SSL_MAX_OPTIONS] = {
                   SSL_OP_EPHEMERAL_RSA,
                   SSL_OP_CIPHER_SERVER_PREFERENCE,
                   SSL_OP_SINGLE_DH_USE,
                   SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION,
                   SSL_OP_ALL
                 };

BIO *bio_err = NULL;
int32_t verify_depth = 1;
int32_t verify_error = 1;

int32_t
of_ssl_init()
{
  int32_t     ret;
  
  of_ssl_init_library();
  ret = of_ssl_create_newcontext(&ctx, TRUE);
  if(ret == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): of_ssl_create_newcontext Failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

  ret = of_ssl_set_ctx_options(ctx, OF_SSL_CIPHER_SUITE, 0x40);
  if(ret == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): of_ssl_set_ctx_options Failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

  ret = of_ssl_set_ctx_options(ctx, OF_SSL_PEER_CERT_REQUIRED, 1);
  if(ret == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): of_ssl_set_ctx_options Failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

  ret = of_ssl_set_ctx_options(ctx, OF_SSL_SET_CERT_VERIFICATION_DEPTH, 1);
  if(ret == OF_FAILURE)
  {
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): of_ssl_set_ctx_options Failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }

  of_ssl_set_ctx_options(ctx, OF_SSL_SESSION_CACHING, TRUE);
  of_ssl_set_ctx_options(ctx, OF_SSL_SESSION_CACHE_SIZE, 128);

  return OF_SUCCESS;
}  

void
of_ssl_init_library(void)
{
  /* Global openssl system initialization */
  if(!bio_err)
  {
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
  }
  /* An error write context */
  bio_err = BIO_new_fp (stderr, BIO_NOCLOSE);
}

int32_t
of_ssl_create_newcontext(SSL_CTX **ctx, unsigned char bstate)
{
  int32_t        off = 0;  
  SSL_METHOD *method=NULL;
  SSL_CTX    *ctxlocal = NULL;
  
  method = TLSv1_server_method(); /* Openflow supports only TLS method */
  ctxlocal = SSL_CTX_new(method);
  if(ctxlocal == NULL)
  {
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): Create SSL Context Failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }
  off = (SSL_OP_NO_SSLv2 |SSL_OP_NO_SSLv3);
  SSL_CTX_set_options(ctxlocal,off);

  /* bstate is set to one only for debugging purposes */
  if(bstate == TRUE) 
    SSL_CTX_set_info_callback(ctxlocal,of_apps_sslinfo_callback);

  *ctx = ctxlocal;
  return OF_SUCCESS;
} 

int32_t
of_ssl_get_sslobject_for_client(int32_t clientFd,SSL_CTX *ctx,SSL **ssl)
{
  SSL *sslx = NULL;

  sslx = (SSL *)SSL_new(ctx);  /* Create one SSL object to handle the connection */
  if(sslx == NULL)
  {
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): SSL_new() failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }
  if(SSL_set_fd(sslx, clientFd) == 0)
  {
    SSL_free(sslx);
    OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
       "%s(%d): SSL_new() failed\r\n",__FUNCTION__,__LINE__);
    return OF_FAILURE;
  }
  *ssl = sslx;
  return OF_SUCCESS;
}

int32_t
of_ssl_set_ctx_options(SSL_CTX *ctx, uint32_t option, uint32_t value)
{
  switch(option)
  {
    case OF_SSL_CIPHER_SUITE:
    {
      int32_t  bitoffset = 1;
      char CipherList[OF_SSL_CIPHERLIST_MAX_LEN] = "\0";
      int32_t  index,valtest;

      valtest = ((1 << OF_SSL_MAX_CIPHERS) - 1);
      if(value > valtest)
        return OF_FAILURE;
      for(index = 0; (index < OF_SSL_MAX_CIPHERS); /* sizeof(bitoffset)*/index++)
      {
        if((bitoffset << index) & value)
        {
          if(strcmp(CipherList,"\0") != 0)
          {
             strncat(CipherList, ":", 1);
          }
          strncat(CipherList, ofssl_CipherSuite[index],strlen(ofssl_CipherSuite[index]));
        }
      }
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
           "%s(%d): Cipher List = %s\r\n",__FUNCTION__,__LINE__, CipherList);
      if(!(SSL_CTX_set_cipher_list(ctx, CipherList)))
      {
        OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
           "%s(%d): Failed to set the Cipher Suite\r\n",__FUNCTION__,__LINE__);
        return OF_FAILURE;
      }
      return OF_SUCCESS;
   }

   case OF_SSL_PEER_CERT_REQUIRED:
   {
     if(value)
       SSL_CTX_set_verify(ctx,(SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT
                                  |SSL_VERIFY_CLIENT_ONCE),/* of_peer_cert_verify_callback*/NULL);
     else
       SSL_CTX_set_verify(ctx,SSL_VERIFY_NONE,NULL);
     return OF_SUCCESS;
   }
  
   case OF_SSL_SET_CERT_VERIFICATION_DEPTH:
   {
     if(value)
       SSL_CTX_set_verify_depth(ctx,value);
       verify_depth = value; /* it seems that it can not be set per ctx in new openssl versions */
     return OF_SUCCESS;
   }

   case OF_SSL_SESSION_CACHING:
   {
     if(value)
     {
       /* Enable Caching of sessions */
       SSL_CTX_set_session_cache_mode(ctx,SSL_SESS_CACHE_BOTH);
       return OF_SUCCESS;
     }
     else
     {
       SSL_CTX_set_session_cache_mode(ctx,SSL_SESS_CACHE_OFF);
       return OF_SUCCESS;
     }
   }        

   case OF_SSL_SESSION_CACHE_SIZE:
   {
      /* Set the cache size */
      if(!value)
      {
        SSL_CTX_sess_set_cache_size(ctx, value);
        return OF_SUCCESS;
      }
      else
      {
        /* Trying to set cache size of zero */
        SSL_CTX_set_session_cache_mode(ctx,SSL_SESS_CACHE_OFF);
        return OF_SUCCESS;
      }
   } 
   default:
      return OF_FAILURE;
  }
}

int32_t
of_ssl_set_certificates(SSL_CTX *ctx, uint32_t option,char *pvalue1,char *pvalue2)
{
  switch(option)
  {
       case OF_SSL_SET_CACERT:
            if((!SSL_CTX_load_verify_locations(ctx, pvalue1, pvalue2)))
            {
              OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                "%s(%d): Load CA Certificates Failed\r\n",__FUNCTION__,__LINE__);
              ERR_print_errors(bio_err);
              return OF_FAILURE;
            }
            break;
       case OF_SSL_SET_PRIVATE_KEY:
            if(SSL_CTX_use_PrivateKey_file(ctx, pvalue1, SSL_FILETYPE_PEM)<= 0)
            {
              OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
                "%s(%d): Unable to Set Private Key\r\n",__FUNCTION__,__LINE__);
              ERR_print_errors(bio_err);
              return OF_FAILURE;
            }
            break;
       case OF_SSL_SET_CERTIFICATE:
            if(SSL_CTX_use_certificate_file(ctx, pvalue1, SSL_FILETYPE_PEM) <= 0)
            {
              OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                "%s(%d): Unable to Set Server Certficate\r\n",__FUNCTION__,__LINE__);
              ERR_print_errors(bio_err);
              return OF_FAILURE;
            }
            /* Assumes that Private key is already set */
            if(!SSL_CTX_check_private_key(ctx))
            {
              OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
                "%s(%d): Private Key does not match the Public Key Certificate\r\n",__FUNCTION__,__LINE__);
              return OF_FAILURE;
            }
            break;
       case OF_SSL_SET_PRIVATE_KEY_PASS:
            {
              SSL_CTX_set_default_passwd_cb(ctx, of_ssl_pem_passwd_cb);
              SSL_CTX_set_default_passwd_cb_userdata(ctx, pvalue1);
              break;
            } 
       default:
            {
              OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_WARN,
                "%s(%d): Invalid Option = %u\r\n",__FUNCTION__,__LINE__,option);
              return OF_FAILURE;
            }
  }
  return OF_SUCCESS;
}

void
of_apps_sslinfo_callback(SSL *s,int32_t where,int32_t ret)
 {
    char *str;
    int32_t w;

    w=where& ~SSL_ST_MASK;

    if (w & SSL_ST_CONNECT) str="SSL_connect";
    else if (w & SSL_ST_ACCEPT) str="SSL_accept";
    else str="undefined";

    if(where & SSL_CB_LOOP)
    {
      BIO_printf(bio_err,"%s:%s\n",str,SSL_state_string_long(s));
    }
    else if (where & SSL_CB_ALERT)
    {
      str=(where & SSL_CB_READ)?"read":"write";
      BIO_printf(bio_err,"SSL3 alert %s:%s:%s\n",
                        str,
                        SSL_alert_type_string_long(ret),
                        SSL_alert_desc_string_long(ret));
    }
    else if (where & SSL_CB_EXIT)
    {
      if(ret == 0)
        BIO_printf(bio_err,"%s:failed in %s\n",
                                str,SSL_state_string_long(s));
      else if (ret < 0)
      {
        BIO_printf(bio_err,"%s:error in %s\n",
                                str,SSL_state_string_long(s));
      }
    }
 }


int32_t
of_ssl_pem_passwd_cb(char *buf, int32_t size, int32_t rwflag, void *password)
{
   strncpy(buf, (char *)(password), size);
   buf[size-1]  = '\0';
   return(strlen(buf));
}

int32_t
of_ssl_verify_peer_subjectname(SSL *ssl, uint64_t *pdatapath_id)
{
  X509     *peer;
  int32_t  index = OF_FAILURE;
  char     buf[DPRM_MAX_X509_SUBJECT_LEN];
  uint32_t hashkey;
  struct   dprm_datapath_entry *datapath_info_p = NULL;
  uchar8_t offset;
  
  peer = SSL_get_peer_certificate(ssl);
  
  X509_NAME_oneline(X509_get_subject_name(peer), buf, DPRM_MAX_X509_SUBJECT_LEN);

  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,"Peer Subject Name = %s\r\n",buf);

  offset = DPNODE_DPTBL_OFFSET;
  CNTLR_RCU_READ_LOCK_TAKE();
  MCHASH_TABLE_SCAN(dprm_datapath_table_g,hashkey)
  {
    MCHASH_BUCKET_SCAN(dprm_datapath_table_g,hashkey,datapath_info_p,struct dprm_datapath_entry*,offset)
    {
      if(datapath_info_p)
      {
        if(!strcasecmp(buf, datapath_info_p->expected_subject_name_for_authentication))
        {
          *pdatapath_id = datapath_info_p->dpid;
          CNTLR_RCU_READ_LOCK_RELEASE();
          OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
                     "Matching Datapath Found with Peer Cert Subject Name\r\n");
          return OF_SUCCESS;
        }
      }
    }
  }
  CNTLR_RCU_READ_LOCK_RELEASE();

  OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_DEBUG,
      "No Matching Datapath found for a received Peer Cert Sub Name\r\n");
  return OF_FAILURE;
}  

int32_t of_peer_cert_verify_callback(int32_t ok, X509_STORE_CTX *ctx)
{
  char buf[256];
  X509 *err_cert;
  int32_t   err,depth;

  err_cert = X509_STORE_CTX_get_current_cert(ctx);
  err =	X509_STORE_CTX_get_error(ctx);
  depth = X509_STORE_CTX_get_error_depth(ctx);
  X509_NAME_oneline(X509_get_subject_name(err_cert),buf,256);
  BIO_printf(bio_err,"depth=%d %s\n",depth,buf);
  if (!ok)
  {
    BIO_printf(bio_err,"verify error:num=%d:%s\n",err,X509_verify_cert_error_string(err));
    if(verify_depth >= depth)
    {
      ok=1;
      verify_error=X509_V_OK;
    }
    else
    {
      ok=0;
      verify_error=X509_V_ERR_CERT_CHAIN_TOO_LONG;
    }
  }
  switch (ctx->error)
  {
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
        	X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert),buf,256);
		BIO_printf(bio_err,"issuer= %s\n",buf);
		break;
    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
		BIO_printf(bio_err,"notBefore=");
		ASN1_UTCTIME_print(bio_err,X509_get_notBefore(ctx->current_cert));
		BIO_printf(bio_err,"\n");
		break;
    case X509_V_ERR_CERT_HAS_EXPIRED:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
		BIO_printf(bio_err,"notAfter=");
		ASN1_UTCTIME_print(bio_err,X509_get_notAfter(ctx->current_cert));
		BIO_printf(bio_err,"\n");
		break;
  }
  BIO_printf(bio_err,"verify return:%d\n",ok);
  return(ok);
}

int32_t
of_ssl_shutdown_connection(SSL *ssl, SSL_CTX *ctx)
{
  if(ssl)
  {
    SSL_shutdown(ssl);
  }

  if(ctx)
  {
    SSL_CTX_free(ctx);
  }
  return OF_SUCCESS;
}

int32_t
of_ssl_load_certificates(struct cntlr_ucm_certs_info *pcert_info)
{
  char  *pCApath  = NULL;
  int32_t   ret;

  if(strcmp(pcert_info->priv_key_pwd, ""))
  {
    ret = of_ssl_set_certificates(ctx, OF_SSL_SET_PRIVATE_KEY_PASS,
                                  pcert_info->priv_key_pwd, NULL);
    if(ret == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
          "of_ssl_set_certificates Failed - Priv Key Password\r\n");
      return OF_FAILURE;
    }
  }

  if(strcmp(pcert_info->ca_cert_file, ""))
  {
    ret = of_ssl_set_certificates(ctx, OF_SSL_SET_CACERT,
                                  pcert_info->ca_cert_file, pCApath);
    if(ret == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
         "of_ssl_set_certificates Failed - CA Cert\r\n");
      return OF_FAILURE;
    }
  }

  if(strcmp(pcert_info->priv_key_file, ""))
  {
    ret = of_ssl_set_certificates(ctx, OF_SSL_SET_PRIVATE_KEY,
                                  pcert_info->priv_key_file, NULL);
    if(ret == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
         "of_ssl_set_certificates Failed - Private Key\r\n");
      return OF_FAILURE;
    }
  }

  if(strcmp(pcert_info->self_cert_file, ""))
  {
    ret = of_ssl_set_certificates(ctx, OF_SSL_SET_CERTIFICATE,
                                  pcert_info->self_cert_file, NULL);
    if(ret == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
         "of_ssl_set_certificates Failed - Self Cert\r\n");
      return OF_FAILURE;
    }
  }
  return OF_SUCCESS;
}

int32_t
of_ssl_load_ca_certificate(struct cntlr_ucm_cacert_info *pca_cert_info)
{
  char  *pCApath  = NULL;
  int32_t   ret;

  if(strcmp(pca_cert_info->ca_cert_file, ""))
  {
    ret = of_ssl_set_certificates(ctx, OF_SSL_SET_CACERT,
                                  pca_cert_info->ca_cert_file, pCApath);
    if(ret == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_XPRT_PROTO, OF_LOG_ERROR,
         "of_ssl_set_certificates Failed - Adl. CA Cert\r\n");
      return OF_FAILURE;
    }
  }
  return OF_SUCCESS;
}
