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
 * File name: cmgutil.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/25/2013
 * Description: 
 * 
 */

#ifndef __CM_UTIL_H
#define __CM_UTIL_H

#define of_memcpy memcpy
#define of_memset memset
#define of_calloc calloc
#define of_free free
#define of_strncpy strncpy
#define of_strcpy strcpy
#define of_sprintf sprintf
#define of_strlen strlen
#define of_atoi atoi
#define of_atol atol
#define ucm_int64_to_str(value,str) of_ltoa
#define ucm_uint8_from_str_ptr(a)    (*(uint8_t *)a-'0')
#define CM_CBK_PRINT_IVPAIR_ARRAY(arr_iv_pair_p)\
	 if(arr_iv_pair_p)\
   {\
			uint32_t count_ui;\
			for(count_ui=0;count_ui < arr_iv_pair_p->count_ui; count_ui++)\
			{\
				 CM_PRINT_IVPAIR(arr_iv_pair_p->iv_pairs[count_ui]);\
			}\
	 }

#define CM_PRINT_IVPAIR(pIvPair) \
	 if(pIvPair.value_p)\
	 {\
			CM_CBK_DEBUG_PRINT ("id_ui=%d ValType=%d Len=%d Value=%s",\
						pIvPair.id_ui,\
						pIvPair.value_type,\
						pIvPair.value_length,(char *)pIvPair.value_p);\
	 }\
	 else\
	 {\
			CM_CBK_DEBUG_PRINT ("id_ui=%d ValType=%d Len=%d",\
						pIvPair.id_ui,\
						pIvPair.value_type,\
						pIvPair.value_length);\
	 }

#define	 OF_CM_MAX_ALLOWED_RECORD_COUNT  10
#define OF_CM_VALIDATE_RECORD_COUNT(RecCnt) \
	{\
		 CM_CBK_DEBUG_PRINT ("Record Count is %ld",RecCnt);\
		 if ( RecCnt <= 0 )\
		 {\
				CM_CBK_DEBUG_PRINT ("Record Count is less than zero");\
					 return OF_FAILURE;\
		 }\
		 if ( RecCnt > OF_CM_MAX_ALLOWED_RECORD_COUNT)\
		 {\
				CM_CBK_DEBUG_PRINT ("Record Count is greater than max value");\
					 return OF_FAILURE;\
		 }\
	}

//#define CM_CBK_DEBUG_PRINT printf("\n%35s(%4d): ",__FUNCTION__,__LINE__),\
//	                       printf    

#define CM_CBK_DEBUG_PRINT(format,args...)   

/******************************************************************************
 * * * * * * * * * C O M M O N  U T I L I T I E S * * * * * * * * * * * * * * *
 *****************************************************************************/

void igwUCMFreeIvPair (struct cm_iv_pair * pIvPair);
void igwUCMFreeIvPairArray (struct cm_array_of_iv_pairs * arr_iv_pair_p,
			uint32_t count_ui);
int32_t igwUCMFILLIVPAIR (struct cm_iv_pair * pIvPair, uint32_t id_ui,
			uint32_t uiValType, void * pData);
int32_t igwUCMCopyIvPairToIvPair (struct cm_iv_pair * pInIvPair,
			struct cm_iv_pair * pOutIvPair);
void fill_app_result_struct(struct cm_app_result** pOutput,struct cm_iv_pair* pIvPair,int32_t result_code);

uint32_t fill_err_msg_to_cm_app_struct(struct cm_app_result** result_p,char* errBuff);
void fill_app_result_struct(struct cm_app_result** pOutput,struct cm_iv_pair* pIvPair,int32_t result_code);


int32_t UCMDPJECopyIvPairArrayToBuffer(struct cm_array_of_iv_pairs *pIvPairAry,char *pUCMTempRespBuf, uint32_t *puiLen);
int32_t UCMDPJECopyIvPairToBuffer(struct cm_iv_pair *, char *s,uint32_t *uiLen);
int32_t UCMDPJECopyIvPairArryFromBuffer (char *pRequestData,struct cm_array_of_iv_pairs *nv_pair_array, uint32_t * puiLen);
int32_t UCMDPJECopyIvPairFromBuffer (char * pRequestData,
			struct cm_iv_pair * pIvPair, uint32_t * puiLen);

void UCMDPJEFreeIvPairArray(struct cm_array_of_iv_pairs *pIvPairArray);
void UCMDPJEFreeIvPair(struct cm_iv_pair *pIvPair);

uint64_t numValue(char ch);
uint64_t charTo64bitNum(char *p);

#endif
