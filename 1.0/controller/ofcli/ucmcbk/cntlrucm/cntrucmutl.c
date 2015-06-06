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
 * File name: cntrucmutl.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT
 

#include"cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
//#define cntrlrucm_debugmsg printf("\r\n%30s(%d)::",__FUNCTION__,__LINE__);printf
//#define cntrlrucm_debugmsg 
      
/***************** FUNCTION DEFINATIONS ****************************/

int32_t UCMDPJECopyIvPairArrayToBuffer(struct cm_array_of_iv_pairs *pIvPairAry,char *pUCMTempRespBuf, uint32_t *puiLen)
{
  int32_t uiNvCnt = 0;
  uint32_t  uiLen = 0;

  of_mbuf_put_32(pUCMTempRespBuf, pIvPairAry->count_ui);
  pUCMTempRespBuf +=4;
  cntrlrucm_debugmsg("count_ui=%d",pIvPairAry->count_ui);

  for (uiNvCnt = 0, uiLen = 0;uiNvCnt < pIvPairAry->count_ui;uiNvCnt++,pUCMTempRespBuf += uiLen,*puiLen+=uiLen,uiLen=0)
  {
      if (UCMDPJECopyIvPairToBuffer(&pIvPairAry->iv_pairs[uiNvCnt], pUCMTempRespBuf,&uiLen) != OF_SUCCESS)
      {
         cntrlrucm_debugmsg("Copy Name Value Pairs into Buffer Failed");
         return OF_FAILURE;
      }
      cntrlrucm_debugmsg("uiLen=%d",uiLen);
   }
  *puiLen+=CM_UINT32_SIZE;
  return OF_SUCCESS;

}

int32_t UCMDPJECopyIvPairToBuffer (struct cm_iv_pair * pInNvPair,
                                 char * pResponseBuf, uint32_t * puiLen)
{

  /* Input Name Value Pair Structure */
  if (pInNvPair == NULL)
  {
    cntrlrucm_debugmsg("Invalid NV Pair Output ");
    return OF_FAILURE;
  }

  /* Output Response Buffer */
  if (pResponseBuf == NULL)
  {
    cntrlrucm_debugmsg("Invalid Output Buff");
    return OF_FAILURE;
  }

  /* Copy Name Length */
   of_mbuf_put_32 (pResponseBuf, pInNvPair->id_ui);
   pResponseBuf += CM_UINT32_SIZE;
   *puiLen += CM_UINT32_SIZE;

  /* Copy Value type */
   of_mbuf_put_32 (pResponseBuf, pInNvPair->value_type);
   pResponseBuf += CM_UINT32_SIZE;
  *puiLen += CM_UINT32_SIZE;

  /* Copy Value Length */
   of_mbuf_put_32 (pResponseBuf, pInNvPair->value_length);
   pResponseBuf += CM_UINT32_SIZE;
  *puiLen += CM_UINT32_SIZE;

  /* Copy Value */
	if(pInNvPair->value_length)
	{
		 of_memcpy (pResponseBuf, pInNvPair->value_p, pInNvPair->value_length);
		 pResponseBuf += pInNvPair->value_length;
		 *puiLen += pInNvPair->value_length;
	}

  return OF_SUCCESS;
}

int32_t UCMDPJECopyIvPairArryFromBuffer (char *pRequestData,struct cm_array_of_iv_pairs *nv_pair_array, uint32_t * puiLen)
{
    int32_t uiNvCnt = 0;
    uint32_t  uiLen = 0;

    if(nv_pair_array==NULL)
    return OF_FAILURE;

    nv_pair_array->count_ui = of_mbuf_get_32 (pRequestData);
    cntrlrucm_debugmsg("nv_pair_array->count_ui=%d",nv_pair_array->count_ui);
    pRequestData += CM_UINT32_SIZE;

    if(nv_pair_array->count_ui ==0 )
    {
         *puiLen=CM_UINT32_SIZE;
         return OF_SUCCESS;
    }

    if(nv_pair_array->count_ui > CM_MAX_NVPAIR_COUNT)
    {
        cntrlrucm_debugmsg("Exceeding Allowed Nv Pair Count");
        return OF_FAILURE;
    }

    /* Allocate Memory for NV Pairs */
    nv_pair_array->iv_pairs = (struct cm_iv_pair *)of_calloc (nv_pair_array->count_ui,sizeof (struct cm_iv_pair));
    if (!(nv_pair_array->iv_pairs))
    {
        cntrlrucm_debugmsg("Memory Allocation Failed for Nv Pairs");
        return OF_FAILURE;
    }

      /* Copy Each Nv Pair */
    for (uiNvCnt = 0, uiLen = 0;uiNvCnt < nv_pair_array->count_ui;uiNvCnt++, pRequestData += uiLen, *puiLen+=uiLen,uiLen = 0)
    {
        if ((UCMDPJECopyIvPairFromBuffer (pRequestData,&(nv_pair_array->iv_pairs[uiNvCnt]),&uiLen)) != OF_SUCCESS)
        {
          cntrlrucm_debugmsg("Copy Name Value Pairs failed ");
          return OF_FAILURE;
        }
    }
    *puiLen+=CM_UINT32_SIZE;
    return OF_SUCCESS;

}
int32_t UCMDPJECopyIvPairFromBuffer (char * pRequestData,
                                   struct cm_iv_pair * pIvPair, uint32_t * puiLen)
{

  /* Input Requet Buffer */
  if (pRequestData == NULL)
  {
    cntrlrucm_debugmsg ("Invalid Input Buff");
    return OF_FAILURE;
  }

  /* Output Name Value Pair Structure */
  if (pIvPair == NULL)
  {
    cntrlrucm_debugmsg ("Invalid NV Pair Output ");
    return OF_FAILURE;
  }

  /* Copy Length of Element Name */
  pIvPair->id_ui = of_mbuf_get_32 (pRequestData);
  pRequestData += CM_UINT32_SIZE;
  *puiLen += CM_UINT32_SIZE;

/* Copy Value Type */
  pIvPair->value_type = of_mbuf_get_32 (pRequestData);
  pRequestData += CM_UINT32_SIZE;
  *puiLen += CM_UINT32_SIZE;

  /* Copy Length of Element Value */
  pIvPair->value_length = of_mbuf_get_32 (pRequestData);
  pRequestData += CM_UINT32_SIZE;
  *puiLen += CM_UINT32_SIZE;

  /* Allocate Memory for Element Value */
     cntrlrucm_debugmsg("\r\n Key Len is %d\r\n",pIvPair->value_length);
        if (pIvPair->value_length > 0 )
        {
                 pIvPair->value_p = of_calloc (1, pIvPair->value_length + 1);
                 if (!(pIvPair->value_p))
                 {
                                cntrlrucm_debugmsg ("Memory Allocation Failed for Value Field");
                                return OF_FAILURE;
                 }

                 /* Copy Element Value */
                 of_strncpy (pIvPair->value_p, pRequestData, pIvPair->value_length);
                 cntrlrucm_debugmsg("\r\n KeyValue =%s\r\n",pIvPair->value_p);
                 pRequestData += pIvPair->value_length;
                 *puiLen += pIvPair->value_length;

                 //CM_JE_PRINT_PTR_NVPAIR (pIvPair);
        }
  return OF_SUCCESS;
}


void UCMDPJEFreeIvPair(struct cm_iv_pair *pIvPair)
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
				 cntrlrucm_debugmsg("Something wrong with pIvPair->Value memory");
				 return;
			}
	 }
	 else
	 {
			if(pIvPair->value_length > 0 )
			{
				 cntrlrucm_debugmsg("Something wrong with pIvPair->Value Length");
				 return;
			}
	 }
	 of_memset(pIvPair, 0 , sizeof(struct cm_iv_pair));
	 return;
}

void UCMDPJEFreeIvPairArray(struct cm_array_of_iv_pairs *pIvPairArray)
{     
	 uint32_t uiIvCnt=0; 
	 for (uiIvCnt=0; uiIvCnt < pIvPairArray->count_ui; uiIvCnt++)
	 {
			UCMDPJEFreeIvPair(&pIvPairArray->iv_pairs[uiIvCnt]);
	 }
	 if ( pIvPairArray->count_ui > 0 )
	 {
			of_free(pIvPairArray->iv_pairs);
	 }  
	 of_memset(pIvPairArray, 0, sizeof(struct cm_array_of_iv_pairs));
	 return;
}     

uint64_t numValue(char ch)
{
  if (ch >= '0' && ch <= '9') return ch-'0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a'+10;
  if (ch >= 'A' && ch <= 'F') return ch - 'A'+10;
  CM_CBK_DEBUG_PRINT ("Invalid data given");
  return OF_FAILURE;
}
uint64_t charTo64bitNum(char *p)
{
  uint64_t res=0;
  while(*p) 
  {
    res<<=4;
    res|= numValue(*p);
    p++;
	  }
  return res;
}

#endif
