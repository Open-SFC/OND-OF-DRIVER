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
 * File name: jeutil.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/25/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"
#include "jeutil.h"
#include "jever.h"

/********************************************************
 * * * * * *  G l o b a l   S e c t i o n   * * * * * *
 ********************************************************/
#ifdef CM_JE_STATS_SUPPORT
extern int32_t cm_je_global_stats_g[JE_STATS_MAX];
extern int32_t cm_je_session_stats_g[JE_SESSION_STATS_MAX];
extern int32_t cm_je_appl_stats_g[JE_SECAPPL_STATS_MAX];
#endif
uint32_t uiStastsCnt = 0;
extern struct cm_dm_data_element RelativityNode;
extern UCMDllQ_t je_del_stack_list;
/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
  \ingroup JEUTLS
  This API copies JE Local Configuration Session structure into Global Configuration 
  structure. This Global structure will be sent to  Managmenent Engiens.

  <b>Input paramameters: </b> \n
  <b><i>pJECfgSession <i></b> Pointer to Local confgiuration structure
  <b>Output Parameters: </b>
  <b><i>pUCMCfgSession:</i></b> pointer to Global Configuration Structure
  <b><i>puiCfgSessionLen:</i></b> pointer to Total length of copied fields
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_cfgsession 
 ** Description   : This API copies Local Configuration Session structure to Global
 **                  Configuration structure.
 ** Input params  : pJECfgSession - Pointer to Local confgiuration structure
 **  Output Params :  pUCMCfgSession - Pointer to Global Configuration structure
 **                  puiCfgSessionLen - Pointer to Total length of copied fields
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/

int32_t je_copy_cfgsession (struct je_config_session * pJECfgSession,
      struct cm_je_config_session * pUCMCfgSession)
{

   if ((pJECfgSession == NULL) || (pUCMCfgSession == NULL))
   {
      CM_JE_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }

   pUCMCfgSession->session_id = pJECfgSession->session_id;

   pUCMCfgSession->mgmt_engine_id = pJECfgSession->mgmt_engine_id;

   pUCMCfgSession->flags = pJECfgSession->flags;

   of_strncpy (pUCMCfgSession->admin_role, pJECfgSession->admin_role,
         of_strlen(pJECfgSession->admin_role));

   of_strncpy (pUCMCfgSession->admin_name, pJECfgSession->admin_name,
         of_strlen(pJECfgSession->admin_name));

   pUCMCfgSession->tnsprt_channel_p = NULL;

   CM_JE_DEBUG_PRINT ("SessionId=%d MgmtEngine=%d AdminName=%s AdminRole=%s",
         pUCMCfgSession->session_id,
         pUCMCfgSession->mgmt_engine_id,
         pUCMCfgSession->admin_name, pUCMCfgSession->admin_role);

   return OF_SUCCESS;
}
int32_t cm_je_copy_ivpair_arr_from_buffer (char *pRequestData,struct cm_array_of_iv_pairs *nv_pair_array, uint32_t * puiLen)
{
   uint32_t uiNvCnt = 0, uiLen = 0;

   if(nv_pair_array==NULL)
      return OF_FAILURE;

   nv_pair_array->count_ui = of_mbuf_get_32 (pRequestData);
   pRequestData += CM_UINT_OS_SIZE;
   *puiLen+=CM_UINT_OS_SIZE;

   if(nv_pair_array->count_ui > CM_MAX_NVPAIR_COUNT)
   {
      CM_JE_DEBUG_PRINT ("nv pairs count %d",nv_pair_array->count_ui);
      CM_JE_DEBUG_PRINT ("Exceeding Allowed Nv Pair Count");
      return OF_FAILURE;
   }

   /* Allocate Memory for NV Pairs */
   nv_pair_array->iv_pairs = (struct cm_iv_pair *)of_calloc (nv_pair_array->count_ui,sizeof (struct cm_iv_pair));
   if (!(nv_pair_array->iv_pairs))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Nv Pairs");
      return OF_FAILURE;
   }

   /* Copy Each Nv Pair */
   for (uiNvCnt = 0, uiLen = 0;uiNvCnt < nv_pair_array->count_ui;uiNvCnt++, pRequestData += uiLen, *puiLen+=uiLen,uiLen = 0)
   {
      if ((cm_je_copy_ivpair_from_buffer (pRequestData,&(nv_pair_array->iv_pairs[uiNvCnt]),&uiLen)) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Copy Name Value Pairs failed ");
         return OF_FAILURE;
      }
   }
   return OF_SUCCESS;

}

int32_t cm_je_get_ivpair_array_len_from_buffer (char *pRequestData, uint32_t * puiLen)
{
	 uint32_t uiCnt,uiIvCnt = 0, uiLen = 0;

	 CM_JE_DEBUG_PRINT ("entered");
	 uiIvCnt=of_mbuf_get_32(pRequestData);
	 CM_JE_DEBUG_PRINT ("nv pairs count %d",uiIvCnt);
	 pRequestData += CM_UINT_OS_SIZE;
	 *puiLen+=CM_UINT_OS_SIZE;

	 if(uiIvCnt > CM_MAX_NVPAIR_COUNT)
	 {
			CM_JE_DEBUG_PRINT ("Exceeding Allowed Nv Pair Count");
			return OF_FAILURE;
	 }

	 /* Allocate Memory for NV Pairs */

	 /* Copy Each Nv Pair */
	 for (uiCnt = 0, uiLen = 0;uiCnt < uiIvCnt;uiCnt++, pRequestData += uiLen, *puiLen+=uiLen,uiLen = 0)
	 {
			if ((cm_je_get_ivpair_len_from_buffer (pRequestData,&uiLen)) != OF_SUCCESS)
			{
				 CM_JE_DEBUG_PRINT ("Copy Name Value Pairs failed ");
				 return OF_FAILURE;
			}
	 }
	 return OF_SUCCESS;
}

int32_t cm_je_get_ivpair_len_from_buffer (char * pRequestData,uint32_t * puiLen)
{
	 uint32_t value_length;

	 /* Input Requet Buffer */
	 if (pRequestData == NULL)
	 {
			CM_JE_DEBUG_PRINT ("Invalid Input Buff");
			return OF_FAILURE;
	 }


	 /* Copy Length of ID  */
	 pRequestData += CM_UINT_OS_SIZE;
	 *puiLen += CM_UINT_OS_SIZE;

	 /* Copy Value Type */
	 pRequestData += CM_UINT_OS_SIZE;
	 *puiLen += CM_UINT_OS_SIZE;

	 /* Copy Length of Element Value */
	 value_length = of_mbuf_get_32 (pRequestData);
	 pRequestData += CM_UINT_OS_SIZE;
	 *puiLen += CM_UINT_OS_SIZE;

	 /* Allocate Memory for Element Value */
	 if (value_length > 0 )
	 {
			/* Copy Element Value */
			//  CM_JE_DEBUG_PRINT("KeyValue is %s",(char *)pIvPair->value_p);
			pRequestData += value_length;
			*puiLen += value_length;
	 }
	 return OF_SUCCESS;
}

int32_t cm_je_copy_ivpair_from_buffer (char * pRequestData,
      struct cm_iv_pair * pNvPair, uint32_t * puiLen)
{

   /* Input Requet Buffer */
   if (pRequestData == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Input Buff");
      return OF_FAILURE;
   }

   /* Output Name Value Pair Structure */
   if (pNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Copy Length of Element Name */
   pNvPair->id_ui = of_mbuf_get_32 (pRequestData);
   pRequestData += CM_UINT_OS_SIZE;
   *puiLen += CM_UINT_OS_SIZE;

   /* Copy Value Type */
   pNvPair->value_type = of_mbuf_get_32 (pRequestData);
   pRequestData += CM_UINT_OS_SIZE;
   *puiLen += CM_UINT_OS_SIZE;

   /* Copy Length of Element Value */
   pNvPair->value_length = of_mbuf_get_32 (pRequestData);
   pRequestData += CM_UINT_OS_SIZE;
   *puiLen += CM_UINT_OS_SIZE;

   /* Allocate Memory for Element Value */
   if (pNvPair->value_length > 0 )
   {
      pNvPair->value_p = of_calloc (1, pNvPair->value_length + 1);
      if (!(pNvPair->value_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         return OF_FAILURE;
      }

      /* Copy Element Value */
      of_strncpy (pNvPair->value_p, pRequestData, pNvPair->value_length);
      //  CM_JE_DEBUG_PRINT("KeyValue is %s",(char *)pNvPair->value_p);
      pRequestData += pNvPair->value_length;
      *puiLen += pNvPair->value_length;

      CM_JE_PRINT_PTR_IVPAIR (pNvPair);
   }
   return OF_SUCCESS;
}


/**
  \ingroup JEUTLS
  This API copies Name Value Pair from Character Buffer recieved  from  Managmenent
  Engiens. 

  <b>Input paramameters: </b> \n
  <b><i>pRequestData<i></b> Pointer to Character Buffer
  <b>Output Parameters: </b>
  <b><i>pNvPair</i></b> pointer to Name Value Pair
  <b><i>puiLen </i></b> pointer to Total length of copied fields
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_copy_nvpair_from_buffer 
 ** Description   : This API copies Name Value Pair from Character Buffer
 ** Input params  : pRequestData - Pointer to Character Buffer
 **  Output Params :  pNvPair  - Pointer to Name Value Pair
 **                  puiLen   - Pointer to Total length of copied fields
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_copy_nvpair_from_buffer (char * pRequestData,
      struct cm_nv_pair * pNvPair, uint32_t * puiLen)
{

   /* Input Requet Buffer */
   if (pRequestData == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Input Buff");
      return OF_FAILURE;
   }

   /* Output Name Value Pair Structure */
   if (pNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Copy Length of Element Name */
   pNvPair->name_length = of_mbuf_get_32 (pRequestData);
   //pRequestData += CM_UINT32_SIZE;
   //*puiLen += CM_UINT32_SIZE;

   pRequestData += CM_UINT_OS_SIZE;
   *puiLen += CM_UINT_OS_SIZE;
   /* Allocate Memory for Element Name */
   if(pNvPair->name_length)  
   {
      pNvPair->name_p = of_calloc (1, pNvPair->name_length + 1);
      if (!(pNvPair->name_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Name Field");
         pNvPair->name_length = 0;
         return OF_FAILURE;
      }
      /* Copy Element Name */
      of_strncpy (pNvPair->name_p, pRequestData, pNvPair->name_length);
      pRequestData += pNvPair->name_length;
      *puiLen += pNvPair->name_length;
   }

   /* Copy Value Type */
   pNvPair->value_type = of_mbuf_get_32 (pRequestData);
   pRequestData += CM_UINT_OS_SIZE;
   *puiLen += CM_UINT_OS_SIZE;

   /* Copy Length of Element Value */
   pNvPair->value_length = of_mbuf_get_32 (pRequestData);
   pRequestData += CM_UINT_OS_SIZE;
   *puiLen += CM_UINT_OS_SIZE;

   /* Allocate Memory for Element Value */
   if (pNvPair->value_length > 0 )
   {
      pNvPair->value_p = of_calloc (1, pNvPair->value_length + 1);
      if (!(pNvPair->value_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         of_free(pNvPair->name_p);
         pNvPair->name_length = 0;
         pNvPair->value_length = 0;
         return OF_FAILURE;
      }

      /* Copy Element Value */
      of_strncpy (pNvPair->value_p, pRequestData, pNvPair->value_length);
      pRequestData += pNvPair->value_length;
      *puiLen += pNvPair->value_length;

      CM_JE_PRINT_PTR_NVPAIR (pNvPair);
   }
   return OF_SUCCESS;
}


int32_t cm_je_copy_ivpair_arr_to_buffer(struct cm_array_of_iv_pairs *pNvPairAry,char *pUCMTempRespBuf, uint32_t *puiLen)
{
   uint32_t uiNvCnt = 0, uiLen = 0;
   pUCMTempRespBuf = of_mbuf_put_32 (pUCMTempRespBuf, pNvPairAry->count_ui);

   //CM_JE_DEBUG_PRINT("%s()count_ui=%d",__FUNCTION__,pNvPairAry->count_ui);

   for (uiNvCnt = 0, uiLen = 0;uiNvCnt < pNvPairAry->count_ui;uiNvCnt++,pUCMTempRespBuf += uiLen,*puiLen+=uiLen,uiLen=0)
   {
      if (cm_je_ivpair_to_buffer(&pNvPairAry->iv_pairs[uiNvCnt], pUCMTempRespBuf,&uiLen) != OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT("Copy Name Value Pairs into Buffer Failed");
         return OF_FAILURE;
      }
      //CM_JE_DEBUG_PRINT("uiLen=%d",uiLen);
   }
   *puiLen+=CM_UINT_OS_SIZE;
   return OF_SUCCESS;

}
int32_t cm_je_ivpair_to_buffer (struct cm_iv_pair * pInNvPair,
      char * pResponseBuf, uint32_t * puiLen)
{

   /* Input Name Value Pair Structure */
   if (pInNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Output Response Buffer */
   if (pResponseBuf == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Output Buff");
      return OF_FAILURE;
   }

   /* Copy Name Length */
   pResponseBuf = of_mbuf_put_32 (pResponseBuf, pInNvPair->id_ui);
   *puiLen += CM_UINT_OS_SIZE;

   /* Copy Value type */
   pResponseBuf = of_mbuf_put_32 (pResponseBuf, pInNvPair->value_type);
   *puiLen += CM_UINT_OS_SIZE;

   /* Copy Value Length */
   pResponseBuf = of_mbuf_put_32 (pResponseBuf, pInNvPair->value_length);
   *puiLen += CM_UINT_OS_SIZE;

   /* Copy Value */
   if(pInNvPair->value_length)
   {
      of_memcpy (pResponseBuf, pInNvPair->value_p, pInNvPair->value_length);
      pResponseBuf += pInNvPair->value_length;
      *puiLen += pInNvPair->value_length;
   }

   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API copies Name Value Pair to Character Buffer which will be sent to  Managmenent
  Engiens. 

  <b>Input paramameters: </b> \n
  <b><i>pNvPair</i></b> pointer to Name Value Pair
  <b>Output Parameters: </b>
  <b><i>pResponseBuf,<i></b> Pointer to Character Buffer
  <b><i>puiLen </i></b> pointer to Total length of copied fields
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_copy_nvpair_to_buffer 
 ** Description   : This API copies Name Value Pair To Character Buffer
 ** Input params  : pNvPair  - Pointer to Name Value PairRequest
 **  Output Params :  pResponseBuf - Pointer to Character Buffer
 **                  puiLen   - Pointer to Total length of copied fields
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_copy_nvpair_to_buffer (struct cm_nv_pair * pInNvPair,
      char * pResponseBuf, uint32_t * puiLen)
{

   /* Input Name Value Pair Structure */
   if (pInNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Output Response Buffer */
   if (pResponseBuf == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Output Buff");
      return OF_FAILURE;
   }

   /* Copy Name Length */
   pResponseBuf = (char *)of_mbuf_put_32 (pResponseBuf, pInNvPair->name_length);
   //  *puiLen += CM_UINT32_SIZE;
   *puiLen += CM_UINT_OS_SIZE;  

   /* Copy Name */
   of_memcpy (pResponseBuf, pInNvPair->name_p, pInNvPair->name_length);
   pResponseBuf += pInNvPair->name_length;
   *puiLen += pInNvPair->name_length;

   /* Copy Value type */
   pResponseBuf = (char *)of_mbuf_put_32 (pResponseBuf, pInNvPair->value_type);
   //*puiLen += CM_UINT32_SIZE;
   *puiLen += CM_UINT_OS_SIZE;  

   /* Copy Value Length */
   pResponseBuf = (char *)of_mbuf_put_32 (pResponseBuf, pInNvPair->value_length);
   //*puiLen += CM_UINT32_SIZE;
   *puiLen += CM_UINT_OS_SIZE;  

   /* Copy Value */
   if(pInNvPair->value_length)
   {
      of_memcpy (pResponseBuf, pInNvPair->value_p, pInNvPair->value_length);
      pResponseBuf += pInNvPair->value_length;
      *puiLen += pInNvPair->value_length;
   }

   return OF_SUCCESS;
}
/**
  \ingroup JEUTLS
  This API copies Name Value Pair structure into another name value pair structure.

  <b>Input paramameters: </b> \n
  <b><i>pInNvPair</i></b> pointer to Input Name Value Pair structure
  <b>Output Parameters: </b>
  <b><i>pOutNvPair</i></b> pointer to response  Name Value Pair structure
  <b><i>puiLen </i></b> pointer to Total length of copied fields
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_nvpair_to_nvpair
 ** Description   : This API copies One Name Value Pair into another Name Value Pair
 structure
 ** Input params  : pInNvPair  - Pointer to Name Value Pair Request
 **  Output Params :  pOutNvPair - Pointer to Name Value Pair Response
 **                  puiLen   - Pointer to Total length of copied fields
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_nvpair_to_nvpair (struct cm_nv_pair * pInNvPair,
      struct cm_nv_pair * pOutNvPair)
{

   /* Input Name Value Pair Structure */
   if (pInNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Output Name Value Pair Structure */
   if (pOutNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Output Nv Pair");
      return OF_FAILURE;
   }

   /* Copy Name Length */
   pOutNvPair->name_length = pInNvPair->name_length;

   /* Allocate Memory for Name */
   if (pInNvPair->name_length > 0)
   {
      if(pInNvPair->name_length > CM_MAX_REQUEST_LEN)
      {
         CM_JE_DEBUG_PRINT ("Name Length is more than allowed request length");
         return OF_FAILURE;
      }

      pOutNvPair->name_p = (char *) of_calloc (1, pOutNvPair->name_length + 1);
      if (!(pOutNvPair->name_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Name Field");
         return OF_FAILURE;
      }
      /* Copy Each Nv Pair */
      /* Copy Name */
      of_strncpy (pOutNvPair->name_p, pInNvPair->name_p, pOutNvPair->name_length);
   }
   /* Copy Value type */
   pOutNvPair->value_type = pInNvPair->value_type;

   /* Copy Value Length */
   pOutNvPair->value_length = pInNvPair->value_length;

   /* Allocate Memory for Value */
   if (pOutNvPair->value_length > 0)
   {
      if(pInNvPair->value_length > CM_MAX_REQUEST_LEN)
      {
         CM_JE_DEBUG_PRINT ("Value Length is more than allowed request length");
         if(pOutNvPair->name_p)
            of_free(pOutNvPair->name_p);
         return OF_FAILURE;
      }
      pOutNvPair->value_p = (char *) of_calloc (1, pOutNvPair->value_length + 1);
      if (!(pOutNvPair->value_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         if(pOutNvPair->name_p)
            of_free(pOutNvPair->name_p);
         return OF_FAILURE;
      }
      /* Copy Each Nv Pair */
      /* Copy Value */
      of_strncpy (pOutNvPair->value_p, pInNvPair->value_p, pOutNvPair->value_length);
      CM_JE_PRINT_PTR_NVPAIR (pOutNvPair);
   }

   return OF_SUCCESS;
}

int32_t je_copy_ivpair_to_ivpair (struct cm_iv_pair * pInIvPair,
      struct cm_iv_pair * pOutIvPair)
{

   /* Input Name Value Pair Structure */
   if (pInIvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid IV Pair Output ");
      return OF_FAILURE;
   }

   /* Output Name Value Pair Structure */
   if (pOutIvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Output Iv Pair");
      return OF_FAILURE;
   }

   /* Copy Name Length */
   pOutIvPair->id_ui = pInIvPair->id_ui;

   /* Copy Value type */
   pOutIvPair->value_type = pInIvPair->value_type;

   /* Copy Value Length */
   pOutIvPair->value_length = pInIvPair->value_length;

   /* Allocate Memory for Value */
   if (pOutIvPair->value_length > 0)
   {
      if(pInIvPair->value_length > CM_MAX_REQUEST_LEN)
      {
         CM_JE_DEBUG_PRINT ("Value Length is more than allowed request length");
         return OF_FAILURE;
      }
      pOutIvPair->value_p = (char *) of_calloc (1, pOutIvPair->value_length + 1);
      if (!(pOutIvPair->value_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         return OF_FAILURE;
      }
      /* Copy Each Iv Pair */
      /* Copy Value */
      of_strncpy (pOutIvPair->value_p, pInIvPair->value_p, pOutIvPair->value_length);
      //    CM_JE_PRINT_PTR_IVPAIR (pOutIvPair);
   }

   return OF_SUCCESS;
}
/**
  \ingroup JEUTLS
  This API copies Configuration Session into character buffer. This Buffer will be 
  sent to Management Engine as a JE response.

  <b>Input paramameters: </b>\n
  <b><i>pUCMCfgSession</i></b> pointer to Confguration Session.
  <b>Output Parameters: </b>
  <b><i>pUCMRespBuf</i></b> pointer to character buffer
  <b><i>puiLen </i></b> pointer to Total length of copied fields
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_cfgsession_to_buffer 
 ** Description   : This API copies Configuration Session into character Buffer
 ** Input params  : pUCMCfgSession  - Pointer to Configuration Session
 **  Output Params :  pUCMRespBuf - Pointer to character Buffer
 **                  puiLen   - Pointer to Total length of copied fields
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_cfgsession_to_buffer (struct cm_je_config_session * pUCMCfgSession,
      char * pUCMRespBuf,
      uint32_t * puiCfgSessionLen)
{

   if ((pUCMRespBuf == NULL) || (pUCMCfgSession == NULL))
   {
      CM_JE_DEBUG_PRINT ("Invalid input");
      return OF_FAILURE;
   }
   /* Copying structure into buffer at a time */
   //  of_memcpy (pUCMRespBuf, pUCMCfgSession, sizeof (struct cm_je_config_session));
   //  *puiCfgSessionLen += sizeof (struct cm_je_config_session);
   /* Copy field by field into output buffer */
   pUCMRespBuf = (char *)of_mbuf_put_32 (pUCMRespBuf, pUCMCfgSession->session_id);
   pUCMRespBuf = (char *)of_mbuf_put_32 (pUCMRespBuf, pUCMCfgSession->mgmt_engine_id);
   pUCMRespBuf = (char *)of_mbuf_put_32 (pUCMRespBuf, pUCMCfgSession->flags);
   pUCMRespBuf = (char *)of_mbuf_put_32(pUCMRespBuf,of_strlen(pUCMCfgSession->admin_role));

   of_memcpy (pUCMRespBuf, pUCMCfgSession->admin_role,of_strlen(pUCMCfgSession->admin_role));
   pUCMRespBuf += of_strlen(pUCMCfgSession->admin_role);

   pUCMRespBuf = (char *)of_mbuf_put_32(pUCMRespBuf,of_strlen(pUCMCfgSession->admin_name));
   of_memcpy (pUCMRespBuf, pUCMCfgSession->admin_name,of_strlen(pUCMCfgSession->admin_name));
   pUCMRespBuf += of_strlen(pUCMCfgSession->admin_name);

   pUCMCfgSession->tnsprt_channel_p = NULL;
   *puiCfgSessionLen += (5 * CM_UINT_OS_SIZE) + of_strlen(pUCMCfgSession->admin_role) + of_strlen(pUCMCfgSession->admin_name);
   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API converts Array of Name Value Pair into ID Value Pair Array.

  <b>Input paramameters: </b> \n
  <b><i>dm_path_p</i></b> pointer to Data Model Path
  <b><i>pInNvPairArr</i></b> Pointer to input Array of Name Value Pairs
  <b>Output Parameters: </b>
  <b><i>pOutIvPairArr</i></b> pointer to Output Array of ID Value Pair structure
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_nvpairarr_to_ivpairarr 
 ** Description   : This API converts Name Value Pair Array into ID Value Pair Array
 ** Input params  : dm_path_p - Pointer to Data Model Path
 **               : pInNvPairArr - Pointer to input Array of Name Value Pairs
 : iMgmtId - 0 => Don't copy management engine Id, else copy given
 management engine Id.
 ** Output Params :  pOutIvvPairArr - Pointer to Array of ID Value Pairs
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_nvpairarr_to_ivpairarr (char * dm_path_p,
      struct cm_array_of_nv_pair * pInNvPairArr,
      struct cm_array_of_iv_pairs * pOutIvPairArr,
      uint32_t mgmt_engine_id)
{
   uint32_t uiCnt = 0;
   int32_t error_code;
   int32_t iNvCnt=0;
   char aMgmtEng[10];
   struct cm_iv_pair * pOutIvPair;

   /* In case of GetExactRecords, no need to pass pInNvPairArr, so it should be null but 
    need to fill management engine id. so don't verify pInNvPairArr here*/
   if ((!dm_path_p) || (!pOutIvPairArr))
   {
      CM_JE_DEBUG_PRINT ("Invalild Input");
      return OF_FAILURE;
   }

   of_memset(aMgmtEng,'\0',sizeof(aMgmtEng));
   if (pInNvPairArr) 
   {
     pOutIvPairArr->count_ui = pInNvPairArr->count_ui;
     iNvCnt = pInNvPairArr->count_ui;
   }
   if (pOutIvPairArr->count_ui == 0 && mgmt_engine_id == 0 )
   {
      pOutIvPairArr->iv_pairs = NULL;
      CM_JE_DEBUG_PRINT ("Zero Nvpairs to be converted");
      return OF_SUCCESS;
   }

   if(mgmt_engine_id)
   {
      pOutIvPairArr->count_ui = pOutIvPairArr->count_ui + 1;
   }

   pOutIvPairArr->iv_pairs = (struct cm_iv_pair *) of_calloc (pOutIvPairArr->count_ui,
         sizeof (struct cm_iv_pair));
   if (!(pOutIvPairArr->iv_pairs))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Array Of IV Pairs");
      pOutIvPairArr->count_ui = 0;
      return OF_FAILURE;
   }

   for (uiCnt = 0; uiCnt < iNvCnt; uiCnt++)
   {
      if (error_code = je_copy_nvpair_to_ivpair (dm_path_p,
               &(pInNvPairArr->nv_pairs[uiCnt]),
               &(pOutIvPairArr->iv_pairs[uiCnt]))!= OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Copy NvPair To IvPair");
         pOutIvPairArr->count_ui = 0;
         of_free(pOutIvPairArr->iv_pairs);
         pOutIvPairArr->iv_pairs = NULL;
         //     UCMJE_FREE_PTR_IVPAIR_ARRAY (pOutIvPairArr, pOutIvPairArr->count_ui);
         return error_code;
      }
   }

   if(mgmt_engine_id)
   {
      pOutIvPair = &(pOutIvPairArr->iv_pairs[iNvCnt]);
      pOutIvPair->id_ui = CM_MGMT_ENGINE_ID;

      sprintf(aMgmtEng, "%d",mgmt_engine_id);

      pOutIvPair->value_length = of_strlen(aMgmtEng);

      pOutIvPair->value_type = CM_DATA_TYPE_UINT;

      pOutIvPair->value_p = of_calloc (1, pOutIvPair->value_length + 1);


      if (!(pOutIvPair->value_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         pOutIvPairArr->count_ui = 0;
         pOutIvPair->value_length = 0;
         of_free(pOutIvPairArr->iv_pairs);
         pOutIvPairArr->iv_pairs = NULL;
         return OF_FAILURE;
      }
      /* Copy Value */
      of_strcpy (pOutIvPair->value_p,aMgmtEng);
      CM_JE_PRINT_PTR_IVPAIR (pOutIvPair);
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API converts Name Value Pair structure nto ID Value Pair.This API's invokes
  Data Model API using DM Path to get a ID of the Node.

  <b>Input paramameters: </b> \n
  <b><i>dm_path_p</i></b> pointer to Data Model Path
  <b><i>pInNvPair</i></b> Pointer to input Name Value Pair structure
  <b>Output Parameters: </b>
  <b><i>pOutIvPairArr</i></b> pointer to output ID Value Pair structure
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_nvpair_to_ivpair
 ** Description   : This API converts Name Value Pair into ID Value Pair structure
 ** Input params  : dm_path_p - Pointer to Data Model Path
 **               : pInNvPair - Pointer to input Name Value Pair structure
 **  Output Params :  pOutIvvPair - Pointer to Output of ID Value Pair structure
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_nvpair_to_ivpair (char * dm_path_p,
      struct cm_nv_pair * pInNvPair,
      struct cm_iv_pair * pOutIvPair)
{
   int32_t iId;

   /* Input Name Value Pair Structure */
   if (pInNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Output Name Value Pair Structure */
   if (pOutIvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Output Nv Pair");
      return OF_FAILURE;
   }

   /* Get DM ID by Path */
   /* FIX ME */
   if(pInNvPair->name_p== NULL)
   {
      CM_JE_DEBUG_PRINT ("Nv Pair is NULL");
      return OF_FAILURE;
   }

   iId = cm_dm_get_child_id_by_name (dm_path_p, pInNvPair->name_p);
   pOutIvPair->id_ui = iId;
   if (iId <= 0)
   {
      if(!of_strcmp(pInNvPair->name_p,CM_DM_ELEMENT_RELATIVITY_NAME))
      {
         iId= CM_DM_ELEMENT_RELATIVITY_ID;
      }
      else if(!of_strcmp(pInNvPair->name_p , CM_DM_ELEMENT_RELATIVE_KEY_NAME))
      {
         iId= CM_DM_ELEMENT_RELATIVE_KEY_ID;
      }
      else if(!of_strcmp(pInNvPair->name_p , CM_DM_ELEMENT_BPASSIV_KEY_NAME))
      {
         iId= CM_GET_PASSIVE_REC_ID;         
      }
      else
      {
#ifdef CM_JE_SEARCH_CHILD_IN_COMPLETE_DMPATH
         /* Get Node Name from complete path */
         CM_JE_DEBUG_PRINT ("Searching ID again for (%s)", pInNvPair->name_p);
         iId= cmi_dm_get_child_id_by_name_from_dm_path (dm_path_p, pInNvPair->name_p);
#endif
      }

      if (iId <= 0)
      {
         CM_JE_DEBUG_PRINT ("Invalid ID received for (%s)", pInNvPair->name_p);
         return OF_FAILURE;
      }
      pOutIvPair->id_ui= iId;
   }

   /* Copy Value type */
   pOutIvPair->value_type = pInNvPair->value_type;

   /* Copy Value Length */
   pOutIvPair->value_length = pInNvPair->value_length;

   if(pOutIvPair->value_length > 0 )
   {
      /* Allocate Memory for Value */
      pOutIvPair->value_p = of_calloc (1, pOutIvPair->value_length + 1);
      if (!(pOutIvPair->value_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         pOutIvPair->value_length = 0;
         return OF_FAILURE;
      }
      /* Copy Value */
      of_strncpy (pOutIvPair->value_p, pInNvPair->value_p, pOutIvPair->value_length);
      CM_JE_PRINT_PTR_IVPAIR (pOutIvPair);
   } 
   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API Splits Array ID Value Pair Structure into Mandatory, Optional and Key IV Pairs

  <b>Input paramameters: </b> \n
  <b><i>dm_path_p</i></b> pointer to Data Model Path
  <b><i>pInIvPair</i></b> Pointer to Array of ID Value Pair structure
  <b>Output Parameters: </b>
  <b><i>pMandIvPairArr</i></b> pointer to Mandatory Array of ID Value Pair structure
  <b><i>pOptIvPairArr</i></b> pointer to Optional Array of ID Value Pair structure
  <b><i>key_iv_pair_p</i></b> pointer to Key ID Value Pair structure
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_nvpair_to_ivpair
 ** Description   : This API Splits Array ID Value Pair Structure into Mandatory,
 **                  Optional and Key IV Pairs.
 ** Input params  : dm_path_p - Pointer to Data Model Path
 **               : pInIvPair - Pointer to input ID Value Pair structure
 **  Output Params :  pMandIvPairArr - Pointer to Mandatory Array of ID Value Pair structure
 **                  pOptIvPairArr - Pointer to Optional Array of ID Value Pair structure
 **                  key_iv_pair_p - Pointer to Key ID Value Pair structure
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_split_ivpair_arr (char * dm_path_p,
      struct cm_array_of_iv_pairs * pIvPairArr,
      struct cm_array_of_iv_pairs * pMandIvPairArr,
      struct cm_array_of_iv_pairs * pOptIvPairArr,
      struct cm_array_of_iv_pairs * key_iv_pair_arr_p,
      struct cm_result * result_p,
      uint32_t mgmt_engine_id)
{
   uint32_t uiCnt = 0;
   uint32_t uiMandAttribCnt = 0;
   uint32_t uiOptAttribCnt = 0;
   uint32_t uiKeyAttribCnt = 0;
   uint32_t uiMandIndex = 0;
   uint32_t uiKeyIndex = 0;
   uint32_t uiOptIndex = 0;
   struct cm_dm_data_element *pNode = NULL;
   struct cm_dm_data_element *pDMParentNode = NULL;
   struct cm_dm_data_element MgmtEngNode;

   if ((!dm_path_p) || (!pIvPairArr) || (!key_iv_pair_arr_p))
   {
      CM_JE_DEBUG_PRINT ("Invalild Input");
      return OF_FAILURE;
   }
   pDMParentNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path(dm_path_p, of_strlen(dm_path_p));
   if (!pDMParentNode)
   {
      CM_JE_DEBUG_PRINT ("pDMParentNode is NULL");
      UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
            UCMJE_ERROR_GETNODE_BYDMPATH,
            dm_path_p);
      return OF_FAILURE;
   }


   of_memset(&MgmtEngNode,0,sizeof(struct cm_dm_data_element));

   for (uiCnt = 0; uiCnt < pIvPairArr->count_ui; uiCnt++)
   {
      if((!mgmt_engine_id) && (pIvPairArr->iv_pairs[uiCnt].id_ui == CM_MGMT_ENGINE_ID))
      {
         continue;
      }
      pNode =
         (struct cm_dm_data_element *) cmi_dm_get_child_info_by_id (dm_path_p,
               pIvPairArr->iv_pairs
               [uiCnt].id_ui);

      if (pNode == NULL)
      {
         //   pDMNode = (struct cm_dm_data_element *)
         //    cmi_dm_get_dm_node_from_instance_path(dm_path_p, of_strlen(dm_path_p));
#ifdef CM_JE_SEARCH_CHILD_IN_COMPLETE_DMPATH
         /* Get Node Name from complete path */
         pNode=(struct cm_dm_data_element *)cmi_dm_get_child_node_by_id_from_dm_path(dm_path_p, pIvPairArr->iv_pairs[uiCnt].id_ui);
#endif
         if (pNode == NULL)
         {
            if(pIvPairArr->iv_pairs[uiCnt].id_ui == CM_DM_ELEMENT_RELATIVITY_ID)
            {
               pNode=&RelativityNode;
            }
            else if( pIvPairArr->iv_pairs[uiCnt].id_ui == CM_MGMT_ENGINE_ID)
            {
               MgmtEngNode.element_attrib.mandatory_b = TRUE;
               MgmtEngNode.element_attrib.key_b = TRUE;
               pNode = &MgmtEngNode;
            }
            else if( pIvPairArr->iv_pairs[uiCnt].id_ui == CM_DM_ELEMENT_RELATIVE_KEY_ID)
            {
               pNode=(struct cm_dm_data_element *)cmi_dm_get_key_child_element(pDMParentNode);
               if (!pNode)
               {
                  CM_JE_DEBUG_PRINT ("pDParentNode is NULL");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_DMERROR_GET_DM_KEY_CHILD_INFO_FAILED,
                        dm_path_p);
                  return OF_FAILURE;
               }
            }
            else
            {
               CM_JE_DEBUG_PRINT ("cmi_dm_get_child_node_by_id_from_dm_path failed");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_DMGETCHILD_BYID,
                     dm_path_p);
               //  UCMJE_COPY_NVPAIR_TO_NVPAIR(pIvPairArr->iv_pairs[uiCnt],
               //  result_p.JEerror.nv_pair);
               return OF_FAILURE;
            }
         }
#if 0
         if (pDMNode->element_attrib.element_type ==  CM_DMNODE_TYPE_SCALAR_VAR)
         {
            char aTemp[CM_JE_MAX_REFFERENCE_LEN] = { };
            char *pRefEleName = NULL;

            of_strcpy (aTemp, dm_path_p);
            CM_JE_DEBUG_PRINT ("Its a Scalar Variable");
            UCMJE_ELEMENT_NAME_FROM_REFERRENCE (aTemp, pRefEleName);
            CM_JE_DEBUG_PRINT ("child node %s",pRefEleName);
            pNode = cmi_dm_get_child_by_name (pDMNode->parent_p,pRefEleName);
            if (pNode == NULL)
            {
               CM_JE_DEBUG_PRINT ("cmi_dm_get_child_by_name failed for node %s",pRefEleName);
               return OF_FAILURE;
            }
         }
         else
         {
            CM_JE_DEBUG_PRINT ("Get Child Info failed");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_ERROR_DMGETCHILD_BYID,
                  dm_path_p);
            //  UCMJE_COPY_NVPAIR_TO_NVPAIR(pIvPairArr->iv_pairs[uiCnt],
            //  result_p.JEerror.nv_pair);
            return OF_FAILURE;
         }
#endif
      }
      if ((pNode->element_attrib.mandatory_b == TRUE)
            &&( pIvPairArr->iv_pairs[uiCnt].id_ui != CM_DM_ELEMENT_RELATIVE_KEY_ID))
      {
         if(pNode->element_attrib.scalar_group_b == TRUE)
         {
            CM_JE_DEBUG_PRINT ("Grouped scalars doesn't require mandatory as TRUE");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_DMERROR_MANDATORY_GROUPED_SCALAR,
                  dm_path_p);
            return OF_FAILURE;
         }
         uiMandAttribCnt++;
      }
      if(pNode->element_attrib.key_b == TRUE
            &&( pIvPairArr->iv_pairs[uiCnt].id_ui != CM_DM_ELEMENT_RELATIVE_KEY_ID))
      {
         if ((pNode->element_attrib.mandatory_b != TRUE) || (pNode->element_attrib.scalar_group_b == TRUE))
         {
            CM_JE_DEBUG_PRINT ("Key is TRUE but mandatory is FALSE, both should be TRUE in case of Table or FALSE for Grouped scalar");
            UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                  UCMJE_DMERROR_KEY_MANDATORY_NOTMATCHING,
                  dm_path_p);
            return OF_FAILURE;
         }
         uiKeyAttribCnt++;
      }

      if((pNode->element_attrib.mandatory_b != TRUE && pNode->element_attrib.key_b != TRUE) 
            || pIvPairArr->iv_pairs[uiCnt].id_ui == CM_DM_ELEMENT_RELATIVE_KEY_ID)
      {
         uiOptAttribCnt++;
      }
   }

   pMandIvPairArr->count_ui = uiMandAttribCnt;
   pOptIvPairArr->count_ui = uiOptAttribCnt;
   key_iv_pair_arr_p->count_ui = uiKeyAttribCnt;

   if (pMandIvPairArr->count_ui > 0 )
   {
      pMandIvPairArr->iv_pairs = of_calloc (uiMandAttribCnt, sizeof (struct cm_iv_pair));
      if (!pMandIvPairArr->iv_pairs)
      {
         CM_JE_DEBUG_PRINT
            ("Memory Allocatin Failed for Mandatory Array Of IV Pairs");
         return OF_FAILURE;
      }
   }

   if ( pOptIvPairArr->count_ui > 0 )
   {
      pOptIvPairArr->iv_pairs = of_calloc (uiOptAttribCnt, sizeof (struct cm_iv_pair));

      if (!pOptIvPairArr->iv_pairs)
      {
         CM_JE_DEBUG_PRINT
            ("Memory Allocatin Failed for Mandatory Array Of IV Pairs");
         UCMJE_FREE_IVPAIR_PTR(pMandIvPairArr, pMandIvPairArr->count_ui);
         return OF_FAILURE;

      }
   }
   if (key_iv_pair_arr_p->count_ui > 0)
   {
      key_iv_pair_arr_p->iv_pairs = of_calloc (uiKeyAttribCnt, sizeof (struct cm_iv_pair));
      if (!key_iv_pair_arr_p->iv_pairs)
      {
         CM_JE_DEBUG_PRINT
            ("Memory Allocatin Failed for Mandatory Array Of IV Pairs");
         UCMJE_FREE_IVPAIR_PTR(pMandIvPairArr, pMandIvPairArr->count_ui);
         UCMJE_FREE_IVPAIR_PTR(pOptIvPairArr, pOptIvPairArr->count_ui);
         return OF_FAILURE;
      }
   }
   for (uiCnt = 0; uiCnt < pIvPairArr->count_ui; uiCnt++)
   {
      if((!mgmt_engine_id) && (pIvPairArr->iv_pairs[uiCnt].id_ui == CM_MGMT_ENGINE_ID))
      {
         continue;
      }

      pNode = (struct cm_dm_data_element *)
         cmi_dm_get_child_info_by_id (dm_path_p, pIvPairArr->iv_pairs[uiCnt].id_ui);

      if (pNode == NULL)
      {
#ifdef CM_JE_SEARCH_CHILD_IN_COMPLETE_DMPATH
         pNode=(struct cm_dm_data_element *)cmi_dm_get_child_node_by_id_from_dm_path(dm_path_p, pIvPairArr->iv_pairs[uiCnt].id_ui);
#endif
         if (pNode == NULL)
         {
            if(pIvPairArr->iv_pairs[uiCnt].id_ui == CM_DM_ELEMENT_RELATIVITY_ID)
            {
               pNode=&RelativityNode;
            }
            else if( pIvPairArr->iv_pairs[uiCnt].id_ui == CM_MGMT_ENGINE_ID)
            {
               pNode = &MgmtEngNode;
            }
            else if( pIvPairArr->iv_pairs[uiCnt].id_ui == CM_DM_ELEMENT_RELATIVE_KEY_ID)
            {
               pNode=(struct cm_dm_data_element *)cmi_dm_get_key_child_element(pDMParentNode);
               if (!pNode)
               {
                  CM_JE_DEBUG_PRINT ("Key Child Node is NULL");
                  UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                        UCMJE_DMERROR_GET_DM_KEY_CHILD_INFO_FAILED,
                        dm_path_p);
                  return OF_FAILURE;
               }
            }
            else
            {
               CM_JE_DEBUG_PRINT ("cmi_dm_get_child_node_by_id_from_dm_path failed");
               UCMJE_MEMALLOC_AND_COPY_ERROR_INFO_INTO_UCMRESULT (result_p,
                     UCMJE_ERROR_DMGETCHILD_BYID,
                     dm_path_p);
               //  UCMJE_COPY_NVPAIR_TO_NVPAIR(pIvPairArr->iv_pairs[uiCnt],
               //  result_p.JEerror.nv_pair);
               return OF_FAILURE;
            }
         }
      }

      if ((pNode->element_attrib.mandatory_b == TRUE) &&
            ( pIvPairArr->iv_pairs[uiCnt].id_ui != CM_DM_ELEMENT_RELATIVE_KEY_ID))
      {
         pMandIvPairArr->iv_pairs[uiMandIndex].id_ui =
            pIvPairArr->iv_pairs[uiCnt].id_ui;

         /* Copy Value type */
         pMandIvPairArr->iv_pairs[uiMandIndex].value_type =
            pIvPairArr->iv_pairs[uiCnt].value_type;

         /* Copy Value Length */
         pMandIvPairArr->iv_pairs[uiMandIndex].value_length =
            pIvPairArr->iv_pairs[uiCnt].value_length;

         /* Allocate Memory for Value */
         pMandIvPairArr->iv_pairs[uiMandIndex].value_p =
            of_calloc (1, pIvPairArr->iv_pairs[uiCnt].value_length + 1);
         if (!(pMandIvPairArr->iv_pairs[uiMandIndex].value_p))
         {
            CM_JE_DEBUG_PRINT
               ("Memory Allocation Failed for Mandanatory Parameter Value");
            UCMJE_FREE_IVPAIR_PTR(pMandIvPairArr, pMandIvPairArr->count_ui);
            UCMJE_FREE_IVPAIR_PTR(pOptIvPairArr, pOptIvPairArr->count_ui);
            UCMJE_FREE_IVPAIR_PTR(key_iv_pair_arr_p, key_iv_pair_arr_p->count_ui);
            return OF_FAILURE;
         }

         /* Copy Value */
         of_strncpy (pMandIvPairArr->iv_pairs[uiMandIndex].value_p,
               pIvPairArr->iv_pairs[uiCnt].value_p,
               pMandIvPairArr->iv_pairs[uiMandIndex].value_length);
         uiMandIndex++;

         /* Check for Key Element */
         if ((pNode->element_attrib.key_b == TRUE) &&
               ( pIvPairArr->iv_pairs[uiCnt].id_ui != CM_DM_ELEMENT_RELATIVE_KEY_ID))
         {
            key_iv_pair_arr_p->iv_pairs[uiKeyIndex].id_ui = pIvPairArr->iv_pairs[uiCnt].id_ui;

            /* Copy Value type */
            key_iv_pair_arr_p->iv_pairs[uiKeyIndex].value_type = pIvPairArr->iv_pairs[uiCnt].value_type;

            /* Copy Value Length */
            key_iv_pair_arr_p->iv_pairs[uiKeyIndex].value_length = pIvPairArr->iv_pairs[uiCnt].value_length;

            /* Allocate Memory for Value */
            key_iv_pair_arr_p->iv_pairs[uiKeyIndex].value_p =
               of_calloc (1, pIvPairArr->iv_pairs[uiCnt].value_length + 1);

            if (!(key_iv_pair_arr_p->iv_pairs[uiKeyIndex].value_p))
            {
               CM_JE_DEBUG_PRINT
                  ("Memory Allocation Failed for Key Parameter Value");
               UCMJE_FREE_IVPAIR_PTR(pMandIvPairArr, pMandIvPairArr->count_ui);
               UCMJE_FREE_IVPAIR_PTR(pOptIvPairArr, pOptIvPairArr->count_ui);
               UCMJE_FREE_IVPAIR_PTR(key_iv_pair_arr_p, key_iv_pair_arr_p->count_ui);
               return OF_FAILURE;
            }

            /* Copy Value */
            of_strncpy (key_iv_pair_arr_p->iv_pairs[uiKeyIndex].value_p,
                  pIvPairArr->iv_pairs[uiCnt].value_p, key_iv_pair_arr_p->iv_pairs[uiKeyIndex].value_length);
            uiKeyIndex++;
         }
      }
      else
      {
         pOptIvPairArr->iv_pairs[uiOptIndex].id_ui =
            pIvPairArr->iv_pairs[uiCnt].id_ui;

         /* Copy Value type */
         pOptIvPairArr->iv_pairs[uiOptIndex].value_type =
            pIvPairArr->iv_pairs[uiCnt].value_type;

         /* Copy Value Length */
         pOptIvPairArr->iv_pairs[uiOptIndex].value_length =
            pIvPairArr->iv_pairs[uiCnt].value_length;

         /* Allocate Memory for Value */
         pOptIvPairArr->iv_pairs[uiOptIndex].value_p =
            of_calloc (1, pIvPairArr->iv_pairs[uiCnt].value_length + 1);
         if (!(pOptIvPairArr->iv_pairs[uiOptIndex].value_p))
         {
            CM_JE_DEBUG_PRINT
               ("Memory Allocation Failed for Optional Parameter Value");
            UCMJE_FREE_IVPAIR_PTR(pMandIvPairArr, pMandIvPairArr->count_ui);
            UCMJE_FREE_IVPAIR_PTR(pOptIvPairArr, pOptIvPairArr->count_ui);
            UCMJE_FREE_IVPAIR_PTR(key_iv_pair_arr_p, key_iv_pair_arr_p->count_ui);

            return OF_FAILURE;
         }
         /* Copy Value */
         of_strncpy (pOptIvPairArr->iv_pairs[uiCnt - uiMandIndex].value_p,
               pIvPairArr->iv_pairs[uiCnt].value_p,
               pOptIvPairArr->iv_pairs[uiCnt - uiMandIndex].value_length);

         uiOptIndex++;
      }
   }
   return OF_SUCCESS;
}

int32_t je_get_key_nvpair(char * dm_path_p,
      struct cm_array_of_nv_pair * nv_pair_arr_p,
      struct cm_nv_pair * key_nv_pair_p)
{
   uint32_t uiCnt = 0;
   struct cm_dm_data_element *pNode = NULL;


   if ((!dm_path_p) || (!nv_pair_arr_p) || (!key_nv_pair_p))
   {
      CM_JE_DEBUG_PRINT ("Invalild Input");
      return OF_FAILURE;
   }

   for (uiCnt = 0; uiCnt < nv_pair_arr_p->count_ui; uiCnt++)
   {
      pNode =
         (struct cm_dm_data_element *) cmi_dm_get_child_info_by_name (dm_path_p,
               nv_pair_arr_p->nv_pairs
               [uiCnt].name_p);

      if (pNode == NULL)
      {
         CM_JE_DEBUG_PRINT ("Get Child Info failed");
         return OF_FAILURE;
      }
      if (pNode->element_attrib.key_b == TRUE)
      {
         if( je_copy_nvpair_to_nvpair(&nv_pair_arr_p->nv_pairs[uiCnt], key_nv_pair_p) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT
               ("Copy Nv Pair to Nv Pair Failed");
            return OF_FAILURE;
         }

         return OF_SUCCESS;
      }
   }
   return OF_FAILURE;
}


int32_t je_get_key_nvpair_arr(char * dm_path_p,
      struct cm_array_of_nv_pair * nv_pair_arr_p,
      struct cm_array_of_nv_pair * key_child_nv_array_p)
{
   struct cm_dm_data_element *pDMParentNode=NULL, *pChildNode;
   uint32_t uiKey=0, uiKeyChildCnt=0, uiCnt;
   UCMDllQ_t *pDll=NULL;
   UCMDllQNode_t *pNode;

   if ((!dm_path_p) || (!nv_pair_arr_p) || (!key_child_nv_array_p))
   {
      CM_JE_DEBUG_PRINT ("Invalild Input");
      return OF_FAILURE;
   }

   pDMParentNode = (struct cm_dm_data_element *)
      cmi_dm_get_dm_node_from_instance_path(dm_path_p, of_strlen(dm_path_p));


   pDll = &(pDMParentNode->child_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         uiKeyChildCnt ++;
      }
   }

   key_child_nv_array_p->count_ui=uiKeyChildCnt;
   if(uiKeyChildCnt != 0 )
   {
      key_child_nv_array_p->nv_pairs=(struct cm_nv_pair *)of_calloc(key_child_nv_array_p->count_ui, sizeof(struct cm_nv_pair));

      CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
      {
         pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
         if (pChildNode->element_attrib.key_b == TRUE)
         {
            for(uiCnt=0; uiCnt < nv_pair_arr_p->count_ui; uiCnt++)
            {
               if(!of_strcmp(pChildNode->name_p, nv_pair_arr_p->nv_pairs[uiCnt].name_p))
               {
                  je_copy_nvpair_to_nvpair( &nv_pair_arr_p->nv_pairs[uiCnt],
                        &key_child_nv_array_p->nv_pairs[uiKey++]);
                  break;
               }
            }
         }
      }
   }
   else
   {
      CM_JE_DEBUG_PRINT("Key Childs Zer0");
      if (pDMParentNode->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR)
      {
         if ((nv_pair_arr_p->count_ui == 1))
         {
            key_child_nv_array_p->count_ui=nv_pair_arr_p->count_ui;
            key_child_nv_array_p->nv_pairs=(struct cm_nv_pair *)of_calloc(key_child_nv_array_p->count_ui, sizeof(struct cm_nv_pair));
            je_copy_nvpair_to_nvpair( &nv_pair_arr_p->nv_pairs[0],
                  &key_child_nv_array_p->nv_pairs[0]);
         }
         else
         {
            return OF_FAILURE;
         }
      }
      else if(pDMParentNode->element_attrib.element_type ==   CM_DMNODE_TYPE_ANCHOR)
      {
         key_child_nv_array_p->count_ui=nv_pair_arr_p->count_ui;
         key_child_nv_array_p->nv_pairs=(struct cm_nv_pair *)
            of_calloc(key_child_nv_array_p->count_ui, sizeof(struct cm_nv_pair));
         CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
         {
            pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
            if (pChildNode->element_attrib.key_b == TRUE)
            {
               for(uiCnt=0; uiCnt < nv_pair_arr_p->count_ui; uiCnt++)
               {
                  if(!of_strcmp(pChildNode->name_p, nv_pair_arr_p->nv_pairs[uiCnt].name_p))
                  {
                     je_copy_nvpair_to_nvpair( &nv_pair_arr_p->nv_pairs[uiCnt],
                           &key_child_nv_array_p->nv_pairs[uiKey++]);
                     break;
                  }
               }
            }
         }
      }
      else
      {
         return OF_FAILURE;
      }

   }    
   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API converts Array of ID Value Pair structure Array into  Array of Name
  Value Pair structure Array.

  <b>Input paramameters: </b> \n
  <b><i>dm_path_p</i></b> pointer to Data Model Path
  <b><i>pInArrIvPairArr</b></i> - Pointer to Array of ID Value Pair Array structure
  <b><i>uiArrCnt</i></b> - Number of Array of ID Value Pair structures
  <b>Output Parameters: </b>
  <b><i>pOutArrNvPairArr</b></i> - Pointer to Array of Name Value Pair Array structure
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_arr_ivpairarr_to_arr_nvpairarr 
 ** Description   : This API converts Array of ID Value Pair structure Array into
 **                  Array of Name Value Pair structure Array.
 ** Input params  : dm_path_p - Pointer to Data Model Path
 **               : pInArrIvPairArr - Pointer to Array of ID Value Pair Array structure
 **               : uiArrCnt - Number of Array of ID Value Pair structures
 **  Output Params :  pOutArrNvPairArr - Pointer to Array of Name Value Pair Array structure
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_arr_ivpairarr_to_arr_nvpairarr (char * dm_path_p,
      struct cm_array_of_iv_pairs *
      pInArrIvPairArr, uint32_t count_ui,
      struct cm_array_of_nv_pair *
      pOutArrNvPairArr)
{
   uint32_t uiCnt = 0;

   if ((!dm_path_p) || (!pInArrIvPairArr) || (!pOutArrNvPairArr))
   {
      CM_JE_DEBUG_PRINT ("Invalild Input");
      return OF_FAILURE;
   }

   for (uiCnt = 0; uiCnt < count_ui; uiCnt++)
   {
      if ((je_copy_ivpairarr_to_nvpairarr (dm_path_p,
                  &(pInArrIvPairArr[uiCnt]),
                  &(pOutArrNvPairArr[uiCnt]))) !=
            OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Copy IvPair To NvPair");
         CM_FREE_PTR_ARRAY_OF_NVPAIR_ARRAY (pOutArrNvPairArr, count_ui);
         return OF_FAILURE;
      }
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API converts ID Value Pair structure Array into Name Value Pair Array 
  structure.

  <b>Input paramameters: </b> \n
  <b><i>dm_path_p</i></b> pointer to Data Model Path
  <b><i>pInIvPairArr</b></i> - Pointer to ID Value Pair Array structure
  <b>Output Parameters: </b>
  <b><i>pOutNvPairArr</b></i> - Pointer to Name Value Pair Array structure
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_ivpairarr_to_nvpairarr 
 ** Description   : This API converts  ID Value Pair Array structure into
 **                   of Name Value Pair Array structure.
 ** Input params  : dm_path_p - Pointer to Data Model Path
 **               : pInIvPairArr - Pointer to  ID Value Pair Array structure
 **  Output Params :  pOutNvPairArr - Pointer to  Name Value Pair Array structure
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_ivpairarr_to_nvpairarr (char * dm_path_p,
      struct cm_array_of_iv_pairs * pInIvPairArr,
      struct cm_array_of_nv_pair * pOutNvPairArr)
{
   uint32_t uiCnt = 0;

   if ((!dm_path_p) || (!pInIvPairArr) || (!pOutNvPairArr))
   {
      CM_JE_DEBUG_PRINT ("Invalild Input");
      return OF_FAILURE;
   }

   pOutNvPairArr->count_ui = 0;
   for (uiCnt = 0; uiCnt < pInIvPairArr->count_ui; uiCnt++)
   {
      if(pInIvPairArr->iv_pairs[uiCnt].id_ui != CM_MGMT_ENGINE_ID)
      {
         pOutNvPairArr->count_ui += 1;
      }
   }

   //  pOutNvPairArr->count_ui = pInIvPairArr->count_ui;
   pOutNvPairArr->nv_pairs = (struct cm_nv_pair *) of_calloc (pOutNvPairArr->count_ui,
         sizeof (struct cm_nv_pair));
   if (!(pOutNvPairArr->nv_pairs))
   {
      CM_JE_DEBUG_PRINT ("Memory Allocatin Failed for Array Of IV Pairs");
      return OF_FAILURE;
   }

   for (uiCnt = 0; uiCnt < pOutNvPairArr->count_ui; uiCnt++)
   {
      if(pInIvPairArr->iv_pairs[uiCnt].id_ui != CM_MGMT_ENGINE_ID)
      {
         if ((je_copy_ivpair_to_nvpair (dm_path_p,
                     &(pInIvPairArr->iv_pairs[uiCnt]),
                     &(pOutNvPairArr->nv_pairs[uiCnt]))) !=
               OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Copy IvPair To NvPair");
            return OF_FAILURE;
         }
      }
   }
   return OF_SUCCESS;
}

/**
  \ingroup JEUTLS
  This API converts ID Value Pair structure  into Name Value Pair structure.

  <b>Input paramameters: </b> \n
  <b><i>dm_path_p</i></b> pointer to Data Model Path
  <b><i>pInIvPair</b></i> - Pointer to ID Value Pair structure
  <b>Output Parameters: </b>
  <b><i>pOutNvPair</b></i> - Pointer to Name Value Pair structure
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : je_copy_ivpair_to_nvpair 
 ** Description   : This API converts  ID Value Pair  structure into
 **                   of Name Value Pair structure.
 ** Input params  : dm_path_p - Pointer to Data Model Path
 **               : pInIvPair - Pointer to  ID Value Pair structure
 **  Output Params :  pOutNvPair - Pointer to  Name Value Pair structure
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t je_copy_ivpair_to_nvpair (char * dm_path_p,
      struct cm_iv_pair * pInIvPair,
      struct cm_nv_pair * pOutNvPair)
{
   char *name_p = NULL;
   struct cm_dm_data_element *pNode=NULL;
   char *pRelativity=CM_DM_ELEMENT_RELATIVITY_NAME;
   char *pRelativeKey=CM_DM_ELEMENT_RELATIVE_KEY_NAME;

   /* Input Name Value Pair Structure */
   if (pInIvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid NV Pair Output ");
      return OF_FAILURE;
   }

   /* Output Name Value Pair Structure */
   if (pOutNvPair == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Output Nv Pair");
      return OF_FAILURE;
   }

   /* Get DM Node Name by ID and Path */
   name_p = (char *) cmi_dm_get_child_name_by_id (dm_path_p, pInIvPair->id_ui);
   if (!name_p)
   {
#ifdef CM_JE_SEARCH_CHILD_IN_COMPLETE_DMPATH
      pNode=(struct cm_dm_data_element*)cmi_dm_get_child_node_by_id_from_dm_path(dm_path_p,pInIvPair->id_ui);
      if (pNode == NULL)
      {
         CM_JE_DEBUG_PRINT ("cmi_dm_get_child_node_by_id_from_dm_path cmi_dm_get_child_by_name failed for node %d",pInIvPair->id_ui);
         {
            if(pInIvPair->id_ui == CM_DM_ELEMENT_RELATIVITY_ID)
            {
               name_p=pRelativity;
            }
            else if(pInIvPair->id_ui == CM_DM_ELEMENT_RELATIVE_KEY_ID)
            {
               name_p=pRelativeKey;
            }
            else
            {
               CM_JE_DEBUG_PRINT ("cmi_dm_get_child_name_by_id Failed");
               return OF_FAILURE;
            }
         }
      }
      else
      {
         name_p=pNode->name_p;
      }
#else
      pDMNode = (struct cm_dm_data_element *)
         cmi_dm_get_dm_node_from_instance_path(dm_path_p, of_strlen(dm_path_p));
      if (pDMNode->element_attrib.element_type ==  CM_DMNODE_TYPE_SCALAR_VAR)
      {
         CM_JE_DEBUG_PRINT ("Its a Scalar Variable");
         pNode =(struct cm_dm_data_element *) cmi_dm_get_child_by_id (pDMNode->parent_p,pInIvPair->id_ui);
         if (pNode == NULL)
         {
            CM_JE_DEBUG_PRINT ("cmi_dm_get_child_by_name failed for node %d",pInIvPair->id_ui);
            return OF_FAILURE;
         }
         name_p=pNode->name_p;
      }
      else
      {
         CM_JE_DEBUG_PRINT ("cmi_dm_get_child_name_by_id Failed");
         if(pInIvPair->id_ui == CM_DM_ELEMENT_RELATIVITY_ID)
         {
            name_p=pRelativity;
         }
         else if(pInIvPair->id_ui == CM_DM_ELEMENT_RELATIVE_KEY_ID)
         {
            name_p=pRelativeKey;
         }
         else
         {
            return OF_FAILURE;
         }
      }
#endif
   }

   /* Copy Name Length and Name */
   if(name_p)
   {
      pOutNvPair->name_length = of_strlen (name_p);
      pOutNvPair->name_p = of_calloc (1, pOutNvPair->name_length + 1);
      if (!(pOutNvPair->name_p))
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for NvPair Name");
         return OF_FAILURE;
      }
      of_strncpy (pOutNvPair->name_p, name_p, pOutNvPair->name_length);
   }
   else
   {
      pOutNvPair->name_length = 0;
   }

   /* Copy Value type */
   pOutNvPair->value_type = pInIvPair->value_type;

   /* Copy Value Length */
   pOutNvPair->value_length = pInIvPair->value_length;

   /* Allocate Memory for Value */
   if(pOutNvPair->value_length > 0 )
   {
      pOutNvPair->value_p = of_calloc (1, pOutNvPair->value_length + 1);
      if (!(pOutNvPair->value_p))
      {
         of_free (pOutNvPair->name_p);
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed for Value Field");
         return OF_FAILURE;
      }

      /* Copy Value */
      of_strncpy (pOutNvPair->value_p, pInIvPair->value_p, pInIvPair->value_length);
   }
   CM_JE_PRINT_PTR_NVPAIR (pOutNvPair);
   return OF_SUCCESS;
}


int32_t je_make_ivpairs (char * dm_path_p,
      struct cm_nv_pair * pNvPair1,
      struct cm_array_of_nv_pair * pInArrNvPairArr1,
      struct cm_array_of_nv_pair * pInArrNvPairArr2,
      struct cm_array_of_iv_pairs * pMandIvPairArr,
      struct cm_array_of_iv_pairs * pOptIvPairArr,
      struct cm_array_of_iv_pairs * key_iv_pair_arr_p,
      struct cm_result * result_p,
      uint32_t mgmt_engine_id)
{
   int32_t return_value;
   struct cm_array_of_nv_pair *array_of_nv_pair_p=NULL;
   struct cm_array_of_iv_pairs *array_of_iv_pair_p=NULL;

   /* TODO: Fill Result structure in case of failure with proper error code */
   if ((return_value=je_combine_nvpairs (pNvPair1, pInArrNvPairArr1,pInArrNvPairArr2,
               &array_of_nv_pair_p)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("je_combine_nvpairs Failed");
      return OF_FAILURE;
   }

   array_of_iv_pair_p=(struct cm_array_of_iv_pairs *)of_calloc(1, sizeof(struct cm_array_of_iv_pairs));
   if (!array_of_iv_pair_p)
   {
      CM_JE_DEBUG_PRINT("Memory Allocation Failed");
      cm_je_free_ptr_nvpairArray(array_of_nv_pair_p);
      return OF_FAILURE;
   }

   if ((return_value=je_copy_nvpairarr_to_ivpairarr (dm_path_p,
               array_of_nv_pair_p, array_of_iv_pair_p, mgmt_engine_id)) != OF_SUCCESS)

   {
      CM_JE_DEBUG_PRINT("je_copy_nvpairarr_to_ivpairarr Failed");
      cm_je_free_ptr_ivpairArray(array_of_iv_pair_p);/*CID 352*/
      cm_je_free_ptr_nvpairArray(array_of_nv_pair_p);
      return OF_FAILURE;
   }

   if ((return_value=je_split_ivpair_arr(dm_path_p, array_of_iv_pair_p,
               pMandIvPairArr, pOptIvPairArr, key_iv_pair_arr_p, result_p, mgmt_engine_id)) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT("je_split_ivpair_arr Failed");
      cm_je_free_ptr_nvpairArray(array_of_nv_pair_p);
      cm_je_free_ptr_ivpairArray(array_of_iv_pair_p);
      return OF_FAILURE;
   }

   cm_je_free_ptr_nvpairArray(array_of_nv_pair_p);
   cm_je_free_ptr_ivpairArray(array_of_iv_pair_p);

   return OF_SUCCESS;
}

int32_t je_combine_nvpairs (struct cm_nv_pair * pNvPair1,
      struct cm_array_of_nv_pair * pInArrNvPairArr1,
      struct cm_array_of_nv_pair * pInArrNvPairArr2,
      struct cm_array_of_nv_pair ** ppOutArrNvPairArr)
{
   uint32_t uiNvPairCnt = 0;
   uint32_t uiCnt = 0, j, ij;
   struct cm_array_of_nv_pair *nv_pair_arr_p;
   unsigned char bElementFound = FALSE;

   if (pNvPair1)
   {
      uiNvPairCnt++;
   }
   if (pInArrNvPairArr1)
   {
      for (ij = 0; ij < pInArrNvPairArr1->count_ui; ij++)
      {
         if (pInArrNvPairArr2)
         {
            for (j = 0; j < pInArrNvPairArr2->count_ui; j++)
            {
               if (of_strcmp(pInArrNvPairArr1->nv_pairs[ij].name_p, pInArrNvPairArr2->nv_pairs[j].name_p) == 0)
               {
                  uiCnt++;
                  break;
               }
            }
         }
      }
   }
   if (pInArrNvPairArr1)
   {
      if (pInArrNvPairArr1->count_ui > 0)
      {
         uiNvPairCnt += pInArrNvPairArr1->count_ui;
      }
   }
   if (pInArrNvPairArr2)
   {
      if (pInArrNvPairArr2->count_ui > 0)
      {
         uiNvPairCnt += pInArrNvPairArr2->count_ui;
      }
   }

   //  CM_JE_DEBUG_PRINT ("NvPair Count = %d", uiNvPairCnt);

   if (uiNvPairCnt == 0)
   {
      CM_JE_DEBUG_PRINT ("Pair Count is zero .. returnig");
      return OF_SUCCESS;
   }
   uiNvPairCnt = uiNvPairCnt - uiCnt;
   nv_pair_arr_p =
      (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
   if (!nv_pair_arr_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
      return OF_FAILURE;
   }
   nv_pair_arr_p->count_ui = uiNvPairCnt;
   nv_pair_arr_p->nv_pairs =
      (struct cm_nv_pair *) of_calloc (uiNvPairCnt, sizeof (struct cm_nv_pair));
   uiCnt = 0; //reassigning back to zero
   if (pNvPair1)
   {
      UCMJE_COPY_PTR_NVPAIR_TO_NVPAIR (pNvPair1, nv_pair_arr_p->nv_pairs[uiCnt]);
      uiCnt++;
   }
   if (pInArrNvPairArr1)
   {
      for (ij = 0; ij < pInArrNvPairArr1->count_ui; ij++)
      {
         bElementFound = FALSE;
         for (j = 0; j < pInArrNvPairArr2->count_ui; j++)
         {
            if (of_strcmp(pInArrNvPairArr1->nv_pairs[ij].name_p, pInArrNvPairArr2->nv_pairs[j].name_p) == 0)
            {
               bElementFound = TRUE;
               break; // break from inner for loop
            }
         }
         if (bElementFound == FALSE)
         {
            UCMJE_COPY_NVPAIR_TO_NVPAIR (pInArrNvPairArr1->nv_pairs[ij],
                  nv_pair_arr_p->nv_pairs[uiCnt]);
            uiCnt++;
            bElementFound = FALSE;
         }
      }
   }
   if (pInArrNvPairArr2)
   {
      for (j = 0; j < pInArrNvPairArr2->count_ui; j++)
      {
         UCMJE_COPY_NVPAIR_TO_NVPAIR (pInArrNvPairArr2->nv_pairs[j],
               nv_pair_arr_p->nv_pairs[uiCnt]);
         uiCnt++;
      }
   }
#if 0
   for (j = 0; j < uiCnt; j++)
   {
      CM_JE_PRINT_NVPAIR (nv_pair_arr_p->nv_pairs[j]);
   }
#endif
   *ppOutArrNvPairArr = nv_pair_arr_p;
   return OF_SUCCESS;
}

int32_t ucmJECopyApresult_pToUCMResult (char * dm_path_p,
      struct cm_app_result * app_result_p,
      struct cm_result * result_p)
{
   uint32_t uiResultLen = 0;

   if (!app_result_p)
   {
      CM_JE_DEBUG_PRINT ("Application Result is NULL");
      return OF_FAILURE;
   }

   if (!result_p)
   {
      CM_JE_DEBUG_PRINT ("UCM Result is NULL");
      return OF_FAILURE;
   }

   if (app_result_p->result_string_p)
   {
      uiResultLen = of_strlen (app_result_p->result_string_p);
      result_p->result.error.result_string_p =
         (char *) of_calloc (1, uiResultLen + 1);
      if (!result_p->result.error.result_string_p)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation  Failed");
         return OF_FAILURE;
      }
      of_strncpy (result_p->result.error.result_string_p, app_result_p->result_string_p,
            uiResultLen);
      CM_JE_DEBUG_PRINT ("Error =%s",result_p->result.error.result_string_p);
   }

   if (app_result_p->result.error.iv_pair.value_length > 0)
   {
      if (je_copy_ivpair_to_nvpair (dm_path_p,
               &app_result_p->result.error.iv_pair,
               &result_p->result.error.nv_pair) !=
            OF_SUCCESS)
      {
         CM_JE_DEBUG_PRINT ("Convertion of Iv Pair to NV Pair");
         return OF_FAILURE;
      }
      result_p->result.error.nv_pair_b = TRUE;
   }

   return OF_SUCCESS;
}

void  je_print_statistics(void)
{
   uiStastsCnt++;
   if ((uiStastsCnt % CM_JE_NUMBER_OF_REQUESTS) == 0 )
   {
      CM_JE_PRINT_GLOBAL_STATS;
      CM_JE_PRINT_SESSON_STATS;
      CM_JE_PRINT_SECAPPL_STATS;
#ifdef CM_VERSION_SUPPORT
      //cm_vh_print_config_version_history_t();
#endif
   }
   return;
}

void je_free_command(struct cm_command *command_p)
{
   if(command_p)
   {
      je_free_nvpair_array(&command_p->nv_pair_array);
      if(command_p->dm_path_p)
      {
         of_free(command_p->dm_path_p);
         command_p->dm_path_p=NULL;
      }
      of_free(command_p);
      command_p=NULL;
   }
   return;
}

void je_free_command_list(struct je_command_list *pCmdList)
{
   if(!pCmdList)
   {
      CM_JE_DEBUG_PRINT("Command List is NULL");
      return;
   }

   je_free_command(pCmdList->command_p);
   of_free(pCmdList);
   pCmdList=NULL;
   return;
}

void je_free_array_commands(struct cm_array_of_commands *array_of_commands_p,
      uint32_t  count_ui)

{
   uint32_t uindex_i;

   if(!array_of_commands_p)
   {
      CM_JE_DEBUG_PRINT("array_of_commands_p is NULL");
      return;
   } 
   for (uindex_i=0; uindex_i < count_ui; uindex_i++)
   {
      of_free(array_of_commands_p->Commands[uindex_i].dm_path_p);
      je_free_nvpair_array(&array_of_commands_p->Commands[uindex_i].nv_pair_array);
   }
   of_free(array_of_commands_p->Commands);
   of_free(array_of_commands_p);
   return;
}

void je_free_nvpair(struct cm_nv_pair *pNvPair)
{
   /* Free Memory for Element Name*/
   if(pNvPair->name_p)
   {
      if(pNvPair->name_length > 0 )
      {
         of_free(pNvPair->name_p);
         pNvPair->name_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong with pNvPair->Name's memory");
         return;
      }
   }
   /*Free Memory for Element Value*/
   if(pNvPair->value_p)
   {
      if(pNvPair->value_length > 0 )
      {
         of_free(pNvPair->value_p);
         pNvPair->value_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong with pNvPair->Value memory");
         return;
      }
   }
   of_memset(pNvPair,0,sizeof(struct cm_nv_pair));
   return;
}

void cm_je_free_ptr_nvpair(struct cm_nv_pair *pNvPair)
{
   if(!pNvPair)
   {
      CM_DEBUG_PRINT("Input is null");
      return;
   }
   /* Free Memory for Element Name*/
   if(pNvPair->name_p)
   {
      if(pNvPair->name_length > 0 )
      {
         of_free(pNvPair->name_p);
         pNvPair->name_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong in NvPair Name");
         return;
      }
   }
   else
   {
      if(pNvPair->name_length > 0 )
      {
         CM_DEBUG_PRINT("Name is NULL and its len is greater than zero ????");
         return;
      }
   }
   /*Free Memory for Element Value*/
   if(pNvPair->value_p)
   {
      if(pNvPair->value_length > 0 )
      {
         of_free(pNvPair->value_p);
         pNvPair->value_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong in NvPair Value");
         return;
      }
   }
   else
   {
      if(pNvPair->value_length > 0 )
      {
         CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");
         return;
      }
   }
   of_free(pNvPair);
   pNvPair=NULL;
   return;
}

void cm_je_free_ptr_nvpair_member_of_array(struct cm_nv_pair *pNvPair)
{

   if(!pNvPair)
   {
      CM_DEBUG_PRINT("Input is null");
      return;
   }
   /* Free Memory for Element Name*/
   if(pNvPair->name_p)
   {
      if(pNvPair->name_length > 0 )
      {
         of_free(pNvPair->name_p);
         pNvPair->name_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong in NvPair Name");
         return;
      }
   }
   else
   {
      if(pNvPair->name_length > 0 )
      {
         CM_DEBUG_PRINT("Name is NULL and its len is greater than zero ????");
         return;
      }
   }
   /*Free Memory for Element Value*/
   if(pNvPair->value_p)
   {
      if(pNvPair->value_length > 0 )
      {
         of_free(pNvPair->value_p);
         pNvPair->value_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong in NvPair Value");
         return;
      }
   }
   else
   {
      if(pNvPair->value_length > 0 )
      {
         CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");
         return;
      }
   }
   return;
}

void je_free_nvpair_array(struct cm_array_of_nv_pair *pnv_pair_array)
{
   uint32_t uiNvCnt=0;
   for (uiNvCnt=0; uiNvCnt < pnv_pair_array->count_ui; uiNvCnt++)
   {
      je_free_nvpair(&pnv_pair_array->nv_pairs[uiNvCnt]);
   }
   if ( pnv_pair_array->count_ui > 0 )
   {
      of_free(pnv_pair_array->nv_pairs);
   }
   of_memset(pnv_pair_array, 0, sizeof(struct cm_array_of_nv_pair));
   return;
}

void cm_je_free_ptr_nvpairArray(struct cm_array_of_nv_pair *pnv_pair_array)
{
   uint32_t uiNvCnt=0;

   if(!pnv_pair_array)
   {
      CM_DEBUG_PRINT("Input is null");
      return;
   }
   for (uiNvCnt=0; uiNvCnt < pnv_pair_array->count_ui; uiNvCnt++)
   {
      je_free_nvpair(&pnv_pair_array->nv_pairs[uiNvCnt]);
   }
   if (pnv_pair_array->count_ui > 0 )
   {
      of_free(pnv_pair_array->nv_pairs);
   }
   of_free(pnv_pair_array);
   pnv_pair_array=NULL;
   return;
}

void cm_je_free_ivpair(struct cm_iv_pair *pIvPair)
{
   /*Free Memory for Element Value*/
   if(pIvPair->value_p)
   {
      if(pIvPair->value_length > 0 )
      {
         of_free(pIvPair->value_p);
         pIvPair->value_p=NULL;
         pIvPair->value_length = 0;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong with pIvPair->Value memory");
         return;
      }
   }
   else
   {
      if(pIvPair->value_length > 0 )
      {
         CM_DEBUG_PRINT("Something wrong with pIvPair->Value Length");
         return;
      }
   }
   of_memset(pIvPair, 0 , sizeof(struct cm_iv_pair));
   return;
}

void cm_je_free_ptr_ivpair(struct cm_iv_pair *pIvPair)
{
   if(pIvPair)
   {
      CM_DEBUG_PRINT("Input is null");
      return;
   }
   /*Free Memory for Element Value*/
   if(pIvPair->value_p)
   {
      if(pIvPair->value_length > 0 )
      {
         of_free(pIvPair->value_p);
         pIvPair->value_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong in IvPair Value");
      }
   }
   else
   {
      if(pIvPair->value_length > 0 )
      {
         CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");
      }
   }
   of_free(pIvPair);
   return;
}

void cm_je_free_ptr_ivpar_member_of_array(struct cm_iv_pair *pIvPair)
{
   if(pIvPair)
   {
      CM_DEBUG_PRINT("Input is null");
      return;
   }
   /*Free Memory for Element Value*/
   if(pIvPair->value_p)
   {
      if(pIvPair->value_length > 0 )
      {
         of_free(pIvPair->value_p);
         pIvPair->value_p=NULL;
      }
      else
      {
         CM_DEBUG_PRINT("Something wrong in IvPair Value");
         return;
      }
   }
   else
   {
      if(pIvPair->value_length > 0 )
      {
         CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");
         return;
      }
   }
   return;
}

void cm_je_free_ivpair_array(struct cm_array_of_iv_pairs *pIvPairArray)
{
   uint32_t uiIvCnt=0;
   for (uiIvCnt=0; uiIvCnt < pIvPairArray->count_ui; uiIvCnt++)
   {
      cm_je_free_ivpair(&pIvPairArray->iv_pairs[uiIvCnt]);
   }
   if ( pIvPairArray->count_ui > 0 )
   {
      of_free(pIvPairArray->iv_pairs);
   }
   of_memset(pIvPairArray, 0, sizeof(struct cm_array_of_iv_pairs));
   return;
}

void cm_je_free_ptr_ivpairArray(struct cm_array_of_iv_pairs *pIvPairArray)
{
   uint32_t uiIvCnt=0;

   if(!pIvPairArray)
   {
      CM_DEBUG_PRINT("Input is null");
      return;
   }

   for (uiIvCnt=0; uiIvCnt < pIvPairArray->count_ui; uiIvCnt++)
   {
      cm_je_free_ivpair(&pIvPairArray->iv_pairs[uiIvCnt]);
   }

   if (pIvPairArray->count_ui > 0 )
   {
      of_free(pIvPairArray->iv_pairs);
   }
   of_free(pIvPairArray);
   pIvPairArray=NULL;

   return;
}

int32_t je_get_last_token (char * path_p, char **pLastToken)
{
   char *sep = ".";
   char *word;

   for (word = strtok (path_p, sep); word; word = strtok (NULL, sep))
   {
      {
         CM_JE_DEBUG_PRINT ("word is %s", word);
         *pLastToken = word;
      }
   }
   CM_JE_DEBUG_PRINT ("Last word is %s", *pLastToken);
   return OF_SUCCESS;
}

unsigned char  ucmJEIsDigits(char *string_p,uint32_t uiLen)
{
   uint32_t uiCnt=0;
   for (uiCnt=0; uiCnt < uiLen; uiCnt++)
   {
      if (!CM_JE_IS_DIGIT(string_p[uiCnt]))
      {
         return FALSE;
      }
   }
   return TRUE;
}


int32_t je_create_and_push_stack_node(struct cm_dm_data_element *pDMNode,  
      void *opaque_info_p,  uint32_t    opaque_info_length,
      struct cm_array_of_iv_pairs *dm_path_keys_iv_pair_arr_p,
      struct cm_array_of_iv_pairs *pIvPairArr)
{
   struct je_stack_node *pStackNode = NULL;
   char *pInstDMPath=NULL;
#if 0 
   UCMDM_Inode_t *pDMInode=NULL;
   void *pInodeOpaqueInfo;
   int32_t return_value;
#endif
   struct cm_array_of_iv_pairs *keys_array_p;

   if((pDMNode ==  NULL) || (opaque_info_p == NULL) || (pIvPairArr == NULL))
   {
      CM_JE_DEBUG_PRINT("Invalid input");
      return OF_FAILURE;
   }

   pStackNode=(struct je_stack_node *) of_calloc (1,    sizeof  (struct je_stack_node));
   if (!pStackNode)
   {
      CM_JE_DEBUG_PRINT("Memory allocation failed");
      return OF_FAILURE;
   }
   pStackNode->pDMNode = pDMNode;
   pStackNode->dm_path_p= (char *)cmi_dm_create_name_path_from_node(pDMNode);
   CM_JE_DEBUG_PRINT("Stack Node DMPath = %s", pStackNode->dm_path_p);
   pStackNode->opaque_info_p =(uint32_t *) of_calloc(1,opaque_info_length*sizeof(uint32_t));
   of_memcpy(pStackNode->opaque_info_p, opaque_info_p,(opaque_info_length*sizeof(uint32_t)));
   pStackNode->opaque_info_length=opaque_info_length;

   keys_array_p=je_combine_nvpair_arr(dm_path_keys_iv_pair_arr_p, pIvPairArr);
   pInstDMPath=(char *)cmi_dm_prepare_dm_instance_path(pStackNode->dm_path_p, keys_array_p);
   if(pInstDMPath == NULL)
   {
      CM_JE_DEBUG_PRINT("Instance Path NULL");
      je_cleanup_stack_node(pStackNode);
      return OF_FAILURE;
   }
   CM_JE_DEBUG_PRINT("Instance Path = %s", pInstDMPath);
#if 0
   if (return_value = cmi_dm_get_first_inode_using_instance_path (pInstDMPath,
            UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM, &pDMInode, &pInodeOpaqueInfo) !=
         OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Get Inode FAILED");
   }
   else
   {
      /*Cleaning Opaque Info*/
      cmi_dm_free_instance_opaque_info (pInodeOpaqueInfo);
   }
   //   pStackNode->pDMInode= pDMInode;
#endif
   pStackNode->pDMInode=(UCMDM_Inode_t *)cmi_dm_get_inode_by_name_path(pInstDMPath);
#if 0
   if(pStackNode->pDMInode == NULL)
   {
      CM_JE_DEBUG_PRINT("DMInode NULL");
      of_free(pInstDMPath);
      je_cleanup_stack_node(pStackNode);
      return OF_FAILURE;
   }
#endif
   pStackNode->pIvPairArr=pIvPairArr;
   CM_DLLQ_APPEND_NODE (&je_del_stack_list, &(pStackNode->list_node));
   CM_JE_DEBUG_PRINT("Successfully added to stack for Node =%s", pDMNode->name_p);

   if(pInstDMPath)
      of_free(pInstDMPath);
   UCMJE_FREE_PTR_IVPAIR_ARRAY(keys_array_p, keys_array_p->count_ui);
   return OF_SUCCESS;
}

int32_t je_update_stack_node( struct je_stack_node *pStackNode,
      struct cm_array_of_iv_pairs *dm_path_keys_iv_pair_arr_p,
      struct cm_array_of_iv_pairs *pIvPairArr)
{
   char *pInstDMPath=NULL;
   struct cm_array_of_iv_pairs *keys_array_p;

   if((pStackNode == NULL ) || (pIvPairArr == NULL))
   {
      CM_JE_DEBUG_PRINT("Invalid input");
      return OF_FAILURE;
   }

   if(pStackNode->pIvPairArr)
   {
      UCMJE_FREE_PTR_IVPAIR_ARRAY(pStackNode->pIvPairArr, pStackNode->pIvPairArr->count_ui);
   }

   keys_array_p=je_combine_nvpair_arr(dm_path_keys_iv_pair_arr_p, pIvPairArr);
   pInstDMPath=(char *)cmi_dm_prepare_dm_instance_path(pStackNode->dm_path_p, keys_array_p);
   if(pInstDMPath == NULL)
   {
      CM_JE_DEBUG_PRINT("Instance Path NULL");
      je_cleanup_stack_node(pStackNode);
      return OF_FAILURE;
   }
   CM_JE_DEBUG_PRINT("Instance Path = %s", pInstDMPath);
   pStackNode->pDMInode=(UCMDM_Inode_t *)cmi_dm_get_inode_by_name_path(pInstDMPath);
   pStackNode->pIvPairArr=pIvPairArr;
   CM_JE_DEBUG_PRINT("Successfully updated at node at stack =%s", pStackNode->pDMNode->name_p);
   of_free(pInstDMPath);
   UCMJE_FREE_PTR_IVPAIR_ARRAY(keys_array_p, keys_array_p->count_ui);
   return OF_SUCCESS;
}

void je_delete_table_stack (UCMDllQ_t* pJEDllQ)
{
   struct je_stack_node *pJEStackNode;
   UCMDllQNode_t *pNode, *pTmp;

   if (pJEDllQ != NULL)
   {
      CM_DLLQ_DYN_SCAN (pJEDllQ, pNode, pTmp, UCMDllQNode_t *)
      {
         pJEStackNode =
            CM_DLLQ_LIST_MEMBER (pNode, struct je_stack_node *, list_node);
         if (pJEStackNode)
         {
            CM_DLLQ_DELETE_NODE (pJEDllQ, &(pJEStackNode->list_node));
            je_cleanup_stack_node (pJEStackNode);
         }
      }
   }
}

void je_cleanup_stack_node (struct je_stack_node *  pJEStackNode)
{
   if (pJEStackNode != NULL)
   {
      if (pJEStackNode->opaque_info_p)
      {
         of_free (pJEStackNode->opaque_info_p);
      }
      CM_JE_DEBUG_PRINT("Deleting Stack Node %s path %s",pJEStackNode->pDMNode->name_p, pJEStackNode->dm_path_p);
      if(pJEStackNode->dm_path_p)
         of_free (pJEStackNode->dm_path_p);

      UCMJE_FREE_PTR_IVPAIR_ARRAY(pJEStackNode->pIvPairArr, pJEStackNode->pIvPairArr->count_ui);
      of_free (pJEStackNode);
   }
}

struct cm_array_of_iv_pairs *je_collect_keys_from_stack(UCMDllQ_t *pJEDllQ,  struct cm_array_of_iv_pairs *dm_path_array_of_keys_p)
{
   uint32_t uiStackIvCnt=0, i=0, j=0 , uiDMPathCount=0;
   int32_t return_value;
   struct cm_array_of_iv_pairs *array_of_key_iv_pairs_p;
   UCMDllQNode_t *pNode=NULL;
   struct je_stack_node *pTableStackNode;
   uint32_t uiTotalIvCnt=0;

   CM_DLLQ_SCAN (pJEDllQ, pNode, UCMDllQNode_t *)
   {
      pTableStackNode =
         CM_DLLQ_LIST_MEMBER (pNode, struct je_stack_node *, list_node);
      uiStackIvCnt += pTableStackNode->pIvPairArr->count_ui;
   }
   if ((dm_path_array_of_keys_p != NULL) && ((dm_path_array_of_keys_p->count_ui) > 0))
   {
      uiDMPathCount = dm_path_array_of_keys_p->count_ui;
   }

   uiTotalIvCnt=uiStackIvCnt + uiDMPathCount;
   CM_JE_DEBUG_PRINT("Keys Iv Pair count is  %d", uiTotalIvCnt);
   if( uiTotalIvCnt == 0 )
   {
      return NULL;
   }

   /* Allocate memory */
   array_of_key_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (unlikely (!array_of_key_iv_pairs_p))
   {
      return NULL;
   }             
   array_of_key_iv_pairs_p->count_ui= uiTotalIvCnt;
   array_of_key_iv_pairs_p->iv_pairs =
      (struct cm_iv_pair *) of_calloc (array_of_key_iv_pairs_p->count_ui, sizeof (struct cm_iv_pair));
   if (array_of_key_iv_pairs_p->iv_pairs == NULL)
   {
      of_free (array_of_key_iv_pairs_p);
      return NULL;
   }

   for (i = 0; i < uiDMPathCount; i++)
   {
      return_value =
         je_copy_ivpair_to_ivpair (&dm_path_array_of_keys_p->iv_pairs[i], &array_of_key_iv_pairs_p->iv_pairs[i]);
   }

   uiStackIvCnt=0;
   CM_DLLQ_SCAN (pJEDllQ, pNode, UCMDllQNode_t *)
   {
      pTableStackNode =
         CM_DLLQ_LIST_MEMBER (pNode, struct je_stack_node *, list_node);

      for (j=0; j < pTableStackNode->pIvPairArr->count_ui; j++)
      {
         return_value =
            je_copy_ivpair_to_ivpair (&pTableStackNode->pIvPairArr->iv_pairs[j], 
                  &array_of_key_iv_pairs_p->iv_pairs[i+uiStackIvCnt+j]);
      }
      uiStackIvCnt += j;
   }

   CM_JE_PRINT_IVPAIR_ARRAY(array_of_key_iv_pairs_p);
   return array_of_key_iv_pairs_p;
}

struct cm_array_of_iv_pairs *je_find_key_ivpair_array(struct cm_dm_data_element *pElement, struct cm_array_of_iv_pairs *pIvArr)
{

   UCMDllQNode_t *pNode = NULL;
   UCMDllQ_t *pDll = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   struct cm_array_of_iv_pairs *key_child_iv_array_p=NULL;
   uint32_t uiKey=0, uiKeyChildCnt=0, uiCnt;

   if ((!pElement) || (!pIvArr))
   {
      return NULL;
   }

   pDll = &(pElement->child_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         uiKeyChildCnt ++;
      }
   }

   //   CM_JE_DEBUG_PRINT("Key Child count %d", uiKeyChildCnt);
   if ( uiKeyChildCnt == 0 )
   {
      return NULL;
   }

   key_child_iv_array_p = (struct cm_array_of_iv_pairs *)of_calloc (1, sizeof(struct cm_array_of_iv_pairs));
   key_child_iv_array_p->count_ui=uiKeyChildCnt;
   key_child_iv_array_p->iv_pairs=(struct cm_iv_pair *)of_calloc(key_child_iv_array_p->count_ui, sizeof(struct cm_iv_pair));

   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         for(uiCnt=0; uiCnt < pIvArr->count_ui; uiCnt++)
         {
            if(pChildNode->id_ui == pIvArr->iv_pairs[uiCnt].id_ui)
            {
               je_copy_ivpair_to_ivpair( &pIvArr->iv_pairs[uiCnt],
                     &key_child_iv_array_p->iv_pairs[uiKey++]);
               break;
            }
         }
      }
   }

   CM_JE_PRINT_IVPAIR_ARRAY(key_child_iv_array_p);
   return key_child_iv_array_p;
}

struct cm_array_of_nv_pair *ucmJEFindkey_nv_pairs(struct cm_dm_data_element *pElement, struct cm_array_of_nv_pair *pNvArr)
{

   UCMDllQNode_t *pNode = NULL;
   UCMDllQ_t *pDll = NULL;
   struct cm_dm_data_element *pChildNode = NULL;
   struct cm_array_of_nv_pair *key_child_nv_array_p=NULL;
   uint32_t uiKey=0, uiKeyChildCnt=0, uiCnt;

   if ((!pElement) || (!pNvArr))
   {
      return NULL;
   }

   pDll = &(pElement->child_list);
   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         uiKeyChildCnt ++;
      }
   }

   //   CM_JE_DEBUG_PRINT("Key Child count %d", uiKeyChildCnt);
   if ( uiKeyChildCnt == 0 )
   {
      return NULL;
   }

   key_child_nv_array_p = (struct cm_array_of_nv_pair *)of_calloc (1, sizeof(struct cm_array_of_nv_pair));
   key_child_nv_array_p->count_ui=uiKeyChildCnt;
   key_child_nv_array_p->nv_pairs=(struct cm_nv_pair *)of_calloc(key_child_nv_array_p->count_ui, sizeof(struct cm_nv_pair));

   CM_DLLQ_SCAN (pDll, pNode, UCMDllQNode_t *)
   {
      pChildNode = CM_DLLQ_LIST_MEMBER (pNode, struct cm_dm_data_element *, list_node);
      if (pChildNode->element_attrib.key_b == TRUE)
      {
         for(uiCnt=0; uiCnt < pNvArr->count_ui; uiCnt++)
         {
            if(!of_strcmp(pChildNode->name_p, pNvArr->nv_pairs[uiCnt].name_p))
            {
               je_copy_nvpair_to_nvpair( &pNvArr->nv_pairs[uiCnt],
                     &key_child_nv_array_p->nv_pairs[uiKey++]);
               break;
            }
         }
      }
   }

   CM_JE_PRINT_NVPAIR_ARRAY(key_child_nv_array_p);
   return key_child_nv_array_p;
}

struct cm_array_of_iv_pairs *je_combine_nvpair_arr(struct cm_array_of_iv_pairs *arr1_p,struct cm_array_of_iv_pairs *arr2_p)
{
   int32_t count_ui=0,i=0,j=0;
   struct cm_array_of_iv_pairs *result_arr_p;

   result_arr_p=(struct cm_array_of_iv_pairs *)of_calloc(1,sizeof(struct cm_array_of_iv_pairs));
   if(arr1_p)
   {
      count_ui = arr1_p->count_ui;
   }

   if(arr2_p)
   {
      count_ui += arr2_p->count_ui;
   }

   result_arr_p->count_ui=count_ui;
   if(result_arr_p->count_ui == 0)
      return result_arr_p;

   result_arr_p->iv_pairs=(struct cm_iv_pair *)of_calloc(1,result_arr_p->count_ui *sizeof(struct cm_iv_pair)); 

   if(arr1_p)
   {

      for(i=0; i < arr1_p->count_ui; i++)
      {
         je_copy_ivpair_to_ivpair(&arr1_p->iv_pairs[i], &result_arr_p->iv_pairs[i]);
      }
   }

   if(arr2_p)
   {

      for(j=0; j < arr2_p->count_ui; j++)
      {
         je_copy_ivpair_to_ivpair(&arr2_p->iv_pairs[j], &result_arr_p->iv_pairs[i+j]);
      }
   }
   return result_arr_p;
}

int32_t je_fill_command_info(int32_t iCmd, char * dm_path_p,
      int32_t count_i, struct cm_nv_pair * pNvPair,
      struct cm_command * pCmd)
{
   of_memset (pCmd, 0, sizeof (struct cm_command));
   pCmd->command_id = iCmd;
   pCmd->dm_path_p = dm_path_p;
   pCmd->nv_pair_array.count_ui = count_i;
   pCmd->nv_pair_array.nv_pairs = pNvPair;
}

#endif
#endif
