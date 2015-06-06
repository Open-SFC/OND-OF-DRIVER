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
 * File name: sslcbk.c
 * Author: Narayana Rao K V V <B38869@freescale.com>
 * Date:   05/31/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "cntrucmssl.h"
#include "lxoswrp.h"

int32_t of_ssl_appl_ucmcbk_init (void);

int32_t of_ssl_modcerts (void * config_trans_p,
                      struct cm_array_of_iv_pairs * pMandParams,
                      struct cm_array_of_iv_pairs * pOptParams,
                      struct cm_app_result ** pResult);

int32_t of_ssl_delcerts (void * pConfigTransction,
                      struct cm_array_of_iv_pairs * pKeysArr,
                      struct cm_app_result ** pResult);

int32_t of_ssl_verifycfg (struct cm_array_of_iv_pairs *pMandParams,
                          uint32_t command_id,
                          struct cm_app_result **ppResult);

struct cm_dm_app_callbacks of_ssl_ucm_callbacks = {
       NULL,
       NULL,
       of_ssl_modcerts,
       of_ssl_delcerts,
       NULL,
       NULL,
       NULL,
       NULL,
       of_ssl_verifycfg,
       NULL
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * SSL  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_ssl_appl_ucmcbk_init (void)
{
  int32_t ret;
  CM_CBK_DEBUG_PRINT ("Entry");

  ret = cm_register_app_callbacks (CM_ON_DIRECTOR_SSL_APPL_ID, &of_ssl_ucm_callbacks);
  return OF_SUCCESS;
}

int32_t of_ssl_modcerts(void * config_trans_p,
                     struct cm_array_of_iv_pairs * pMandParams,
                     struct cm_array_of_iv_pairs * pOptParams,
                     struct cm_app_result ** pResult)
{
  struct cm_app_result *of_domain_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct cntlr_ucm_certs_info cert_info={};


  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&cert_info, 0, sizeof (cert_info));

  return_value = of_ssl_certs_ucm_setoptparams (pOptParams, &cert_info,
                                                &of_domain_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    //FillAppResultStruct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
    *pResult = of_domain_result;
    return OF_FAILURE;
  }

  if(!strcmp(cert_info.ca_cert_file, ""))
  {
    CM_CBK_DEBUG_PRINT ("CA CERT is not configured");
    //FillAppResultStruct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
    *pResult = of_domain_result;
    return OF_FAILURE;
  }
  if(!strcmp(cert_info.self_cert_file, ""))
  {
    CM_CBK_DEBUG_PRINT ("SELF CERT is not configured");
    //FillAppResultStruct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
    *pResult = of_domain_result;
    return OF_FAILURE;
  }
  if(!strcmp(cert_info.priv_key_file, ""))
  {
    CM_CBK_DEBUG_PRINT ("Private Key is not configured");
    //FillAppResultStruct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
    *pResult = of_domain_result;
    return OF_FAILURE;
  }
  
  return_value = of_ssl_load_certificates(&cert_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_ssl_load_certificates failed");
    //FillAppResultStruct (&of_domain_result, NULL, CM_GLU_VLAN_FAILED);
    *pResult = of_domain_result;
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Succesfully added SSL Certificates");
  return OF_SUCCESS;
}

int32_t of_ssl_delcerts(void * pConfigTransction,
                     struct cm_array_of_iv_pairs * pKeysArr,
                     struct cm_app_result ** pResult)
{
  CM_CBK_DEBUG_PRINT ("Delete Certificates is not supported");
  return OF_FAILURE;
}


int32_t of_ssl_verifycfg (struct cm_array_of_iv_pairs *pParams,
                          uint32_t command_id,
                          struct cm_app_result **ppResult)
{
  uint32_t ii;
  struct cm_app_result *of_ssl_result = NULL;
  uint8_t switch_type, uiTableCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (ii = 0; ii < pParams->count_ui; ii++)
  {
    switch (pParams->iv_pairs[ii].id_ui)
    {
      case CM_DM_SSL_CACERT_ID:
        if (pParams->iv_pairs[ii].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("CA CERTIFICATE FILE NAME is NULL");
          //FillAppResultStruct (&of_ssl_result, NULL, 
            //                   CM_GLU_SSL_ERROR_NO_CA_CERT_FILE_NAME);
          *ppResult = of_ssl_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_SSL_SELFCERT_ID:
        if (pParams->iv_pairs[ii].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("SELF CERTIFICATE FILE NAME is NULL");
          //FillAppResultStruct (&of_ssl_result, NULL,
                               //CM_GLU_SSL_ERROR_NO_SELF_CERT_FILE_NAME);
          *ppResult = of_ssl_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_SSL_PRIVKEY_ID:
        if (pParams->iv_pairs[ii].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("PRIVATE KEY CERTIFICATE FILE NAME is NULL");
          //FillAppResultStruct (&of_ssl_result, NULL,
            //                   CM_GLU_SSL_ERROR_NO_PRIV_KEY_FILE_NAME);
          *ppResult = of_ssl_result;
          return OF_FAILURE;
        }
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pParams);
  return OF_SUCCESS;
}

int32_t of_ssl_certs_ucm_setoptparams(struct cm_array_of_iv_pairs *pOptParams,
                           struct cntlr_ucm_certs_info *of_certs,
                           struct cm_app_result ** ppResult)
{
  uint32_t ii;

  CM_CBK_DEBUG_PRINT ("Entered");


  for (ii = 0; ii < pOptParams->count_ui; ii++)
  {
    switch (pOptParams->iv_pairs[ii].id_ui)
    {
      case CM_DM_SSL_CACERT_ID:
      {
        of_strncpy(of_certs->ca_cert_file,
                   (char *) pOptParams->iv_pairs[ii].value_p,
                   pOptParams->iv_pairs[ii].value_length);
        break;
      }

      case CM_DM_SSL_SELFCERT_ID:
      {
        of_strncpy(of_certs->self_cert_file,
                   (char *) pOptParams->iv_pairs[ii].value_p,
                   pOptParams->iv_pairs[ii].value_length);
        break;
      }

      case CM_DM_SSL_PRIVKEY_ID:
      {
        of_strncpy(of_certs->priv_key_file,
                   (char *) pOptParams->iv_pairs[ii].value_p,
                   pOptParams->iv_pairs[ii].value_length);
        break;
      }

      case CM_DM_SSL_PRIVKEYPWD_ID:
      {
        of_strncpy(of_certs->priv_key_pwd,
                   (char *) pOptParams->iv_pairs[ii].value_p,
                   pOptParams->iv_pairs[ii].value_length);
        break;
      }
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
