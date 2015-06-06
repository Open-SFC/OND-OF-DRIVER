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
 * File name: cmutil.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file includes the common data structures  
 *              definitions used across UCM modules.
 * 
 */

#ifndef _UCMUTIL_H_
#define _UCMUTIL_H_

/*****************************************************************************
 * *  * * * * * * * * * M A C R O   D E F I N I T I O N S * * * * * * * * * *
 ****************************************************************************/

#ifdef CM_COMMON_DEBUG
#define CM_DEBUG_PRINT printf("\n%35s(%4d): ",__FUNCTION__,__LINE__),\
                    printf
#else
#define CM_DEBUG_PRINT(format,args...)
//printf(format,##args);
#endif

/*****************************************************************************
 *  * * * * * * S T A T I S T I C S   M A C R O S 	* * * * * * * * * * * * * 
 ****************************************************************************/
#define CM_INC_STATS(stats_struct,field)\
{\
	 stats_struct[field]++;\
}
#define CM_DEC_STATS(stats_struct,field)\
{\
	 stats_struct[field]--;\
}

/*****************************************************************************
 * * *  * * * * * * * N V P A I R   M A C R O S *	* * * * * * * * * * * * * *
 ****************************************************************************/
#define CM_FREE_NVPAIR(nv_pair)\
{\
	 /* Free Memory for Element Name*/\
	 if(nv_pair.name_p)\
	 {\
			if(nv_pair.name_length > 0 )\
			{\
				 of_free(nv_pair.name_p);\
				 nv_pair.name_p=NULL;\
			}\
			else\
			{\
				 CM_DEBUG_PRINT("Something wrong with nv_pair Name's memory");\
			}\
	 }\
	 /*Free Memory for Element Value*/\
	 if(nv_pair.value_p)\
	 {\
			if(nv_pair.value_length > 0 )\
			{\
				 of_free(nv_pair.value_p);\
				 nv_pair.value_p=NULL;\
			}\
			else\
			{\
				 CM_DEBUG_PRINT("Something wrong with nv_pair Value memory");\
			}\
	 }\
}

#define CM_FREE_NVPAIR_ARRAY(nv_pair_array,count_ui)\
{\
	 uint32_t nv_cnt_ui=0;\
	 for (nv_cnt_ui=0; nv_cnt_ui < count_ui; nv_cnt_ui++)\
	 {\
			CM_FREE_NVPAIR(nv_pair_array.nv_pairs[nv_cnt_ui]);\
	 }\
	 if ( count_ui > 0 )\
	 {\
			of_free(nv_pair_array.nv_pairs);\
	 }\
	 of_memset(&nv_pair_array, 0, sizeof(struct cm_array_of_nv_pair));\
}

#define CM_FREE_PTR_NVPAIR(nv_pair_p)\
{\
	 if(nv_pair_p)\
	 {\
			/* Free Memory for Element Name*/\
			if(nv_pair_p->name_p)\
			{\
				 if(nv_pair_p->name_length > 0 )\
				 {\
						of_free(nv_pair_p->name_p);\
						nv_pair_p->name_p=NULL;\
				 }\
				 else\
				 {\
						CM_DEBUG_PRINT("Something wrong in NvPair Name");\
				 }\
			}\
			else\
			{\
				 if(nv_pair_p->name_length > 0 )\
				 {\
						CM_DEBUG_PRINT("Name is NULL and its len is greater than zero ????");\
				 }\
			}\
			\
			/*Free Memory for Element Value*/\
			if(nv_pair_p->value_p)\
			{\
				 if(nv_pair_p->value_length > 0 )\
				 {\
						of_free(nv_pair_p->value_p);\
						nv_pair_p->value_p=NULL;\
				 }\
				 else\
				 {\
						CM_DEBUG_PRINT("Something wrong in NvPair Value");\
				 }\
			}\
			else\
			{\
				 if(nv_pair_p->value_length > 0 )\
				 {\
						CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");\
				 }\
			}\
			of_free(nv_pair_p);\
			nv_pair_p=NULL;\
	 }\
}

#define CM_FREE_PTR_NVPAIR_ARRAY(nv_pair_array_p,count_ui)\
{\
	 uint32_t nv_cnt_ui=0;\
	 if(nv_pair_array_p)\
	 {\
			for (nv_cnt_ui=0; nv_cnt_ui < count_ui; nv_cnt_ui++)\
			{\
				 CM_FREE_NVPAIR(nv_pair_array_p->nv_pairs[nv_cnt_ui]);\
			}\
			if (count_ui > 0 )\
			{\
				 of_free(nv_pair_array_p->nv_pairs);\
			}\
			of_free(nv_pair_array_p);\
			nv_pair_array_p=NULL;\
	 }\
}

#define CM_FREE_PTR_ARRAY_OF_NVPAIR_ARRAY(nv_pair_array_p,count_ui)\
{\
	 uint32_t rec_cnt_ui, nv_cnt_ui=0;\
	 \
	 for(rec_cnt_ui = 0; rec_cnt_ui < count_ui; rec_cnt_ui++)\
	 {\
			for (nv_cnt_ui=0; nv_cnt_ui < nv_pair_array_p[rec_cnt_ui].count_ui; nv_cnt_ui++)\
			{\
				 CM_FREE_NVPAIR(nv_pair_array_p[rec_cnt_ui].nv_pairs[nv_cnt_ui]);\
			}\
			if ( nv_pair_array_p[rec_cnt_ui].count_ui > 0 )\
			{\
				 of_free(nv_pair_array_p[rec_cnt_ui].nv_pairs);\
			}\
	 }\
	 of_free(nv_pair_array_p);\
}

#define CM_PRINT_ARRAY_OF_COMMANDS(array_of_commands_p) \
{\
	 uint32_t index_ui; \
	 struct cm_array_of_nv_pair *arr_nv_pair_p;\
	 uint32_t count_ui; \
	 if(array_of_commands_p)\
	 {\
			for (index_ui=0; index_ui < array_of_commands_p->count_ui; index_ui++)\
			{\
				 CM_DEBUG_PRINT("DMPath=%s",array_of_commands_p->Commands[index_ui].dm_path_p);\
				 CM_DEBUG_PRINT("Command Id=%d",array_of_commands_p->Commands[index_ui].command_id);\
				 arr_nv_pair_p=&array_of_commands_p->Commands[index_ui].nv_pair_array;\
				 for(count_ui=0;count_ui < arr_nv_pair_p->count_ui; count_ui++)\
				 {\
						CM_PRINT_NVPAIR(arr_nv_pair_p->nv_pairs[count_ui]);\
				 }\
			}\
	 }\
}

#define CM_FREE_PTR_NVPAIR_MEMBEROF_ARRAY(nv_pair_p)\
{\
	 /* Free Memory for Element Name*/\
	 if(nv_pair_p->name_p)\
	 {\
			if(nv_pair_p->name_length > 0 )\
			{\
				 of_free(nv_pair_p->name_p);\
				 nv_pair_p->name_p=NULL;\
			}\
			else\
			{\
				 CM_DEBUG_PRINT("Something wrong in nv_pair Name");\
			}\
	 }\
	 else\
	 {\
			if(nv_pair_p->name_length > 0 )\
			{\
				 CM_DEBUG_PRINT("Name is NULL and its len is greater than zero ????");\
			}\
	 }\
	 /*Free Memory for Element Value*/\
	 if(nv_pair_p->value_p)\
	 {\
			if(nv_pair_p->value_length > 0 )\
			{\
				 of_free(nv_pair_p->value_p);\
				 nv_pair_p->value_p=NULL;\
			}\
			else\
			{\
				 CM_DEBUG_PRINT("Something wrong in NvPair Value");\
			}\
	 }\
	 else\
	 {\
			if(nv_pair_p->value_length > 0 )\
			{\
				 CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");\
			}\
	 }\
}

#define CM_PRINT_NVPAIR(nv_pair_p) \
	 if(nv_pair_p.value_p)\
	 {\
			CM_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d Value=%s",\
						nv_pair_p.name_length,\
						nv_pair_p.name_p,\
						nv_pair_p.value_type,\
						nv_pair_p.value_length,(char *)nv_pair_p.value_p);\
	 }\
	 else\
	 {\
			CM_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d ",\
						nv_pair_p.name_length,\
						nv_pair_p.name_p,\
						nv_pair_p.value_type,\
						nv_pair_p.value_length);\
	 }

#define CM_PRINT_PTR_NVPAIR(nv_pair_p) \
if (nv_pair_p)\
{\
	 if(nv_pair_p->value_p)\
	 {\
			CM_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d Value=%s",\
						nv_pair_p->name_length,\
						nv_pair_p->name_p,\
						nv_pair_p->value_type,\
						nv_pair_p->value_length,(char *)nv_pair_p->value_p);\
	 }\
	 else\
	 {\
			CM_DEBUG_PRINT ("Len=%d Name=%s ValType=%d Len=%d ",\
						nv_pair_p->name_length,\
						nv_pair_p->name_p,\
						nv_pair_p->value_type,\
						nv_pair_p->value_length);\
	 }\
}\
else\
{\
	 CM_DEBUG_PRINT("Nv Pair is NULL, unable to print");\
}


/*****************************************************************************
 * * *  * * * * * * * I V P A I R   M A C R O S *	* * * * * * * * * * * * * *
 ****************************************************************************/

#define CM_FREE_IVPAIR(iv_pair)\
{\
	 /*Free Memory for Element Value*/\
	 if(iv_pair.value_p)\
	 {\
			if(iv_pair.value_length > 0 )\
			{\
				 of_free(iv_pair.value_p);\
				 iv_pair.value_p=NULL;\
			}\
			else\
			{\
				 CM_DEBUG_PRINT("Something wrong with iv_pair Value memory");\
			}\
	 }\
	 else\
	 {\
			if(iv_pair.value_length > 0 )\
			{\
				 CM_DEBUG_PRINT("Something wrong with iv_pair Value Length");\
			}\
	 }\
}


#define CM_FREE_IVPAIR_ARRAY(iv_pair_array,count_ui)\
{\
	 uint32_t iv_cnt_ui=0;\
	 for (iv_cnt_ui=0; iv_cnt_ui < count_ui; iv_cnt_ui++)\
	 {\
			CM_FREE_IVPAIR(iv_pair_array.iv_pairs[iv_cnt_ui]);\
	 }\
	 if ( count_ui > 0 )\
	 {\
			of_free(iv_pair_array.iv_pairs);\
	 }\
	 of_memset(&iv_pair_array, 0, sizeof(struct cm_array_of_iv_pairs));\
}

#define CM_FREE_IVPAIR_PTR(iv_pair_array_p,count_ui)\
{\
	 uint32_t iv_cnt_ui=0;\
	 for (iv_cnt_ui=0; iv_cnt_ui < count_ui; iv_cnt_ui++)\
	 {\
			CM_FREE_IVPAIR(iv_pair_array_p->iv_pairs[iv_cnt_ui]);\
	 }\
	 if ( count_ui > 0 )\
	 {\
			of_free(iv_pair_array_p->iv_pairs);\
	 }\
}

#define CM_FREE_PTR_IVPAIR(iv_pair_p)\
{\
	 if(iv_pair_p)\
	 {\
			/*Free Memory for Element Value*/\
			if(iv_pair_p->value_p)\
			{\
				 if(iv_pair_p->value_length > 0 )\
				 {\
						of_free(iv_pair_p->value_p);\
						iv_pair_p->value_p=NULL;\
				 }\
				 else\
				 {\
						CM_DEBUG_PRINT("Something wrong in iv_pair Value");\
				 }\
			}\
			else\
			{\
				 if(iv_pair_p->value_length > 0 )\
				 {\
						CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");\
				 }\
			}\
			of_free(iv_pair_p);\
	 }\
}


#define CM_FREE_PTR_IVPAIR_ARRAY(iv_pair_array_p,count_ui)\
{\
	 uint32_t iv_cnt_ui=0;\
	 if(iv_pair_array_p)\
	 {\
			for (iv_cnt_ui=0; iv_cnt_ui < count_ui; iv_cnt_ui++)\
			{\
				 CM_FREE_IVPAIR(iv_pair_array_p->iv_pairs[iv_cnt_ui]);\
			}\
			if (count_ui > 0 )\
			{\
				 of_free(iv_pair_array_p->iv_pairs);\
			}\
			of_free(iv_pair_array_p);\
			iv_pair_array_p=NULL;\
	 }\
}


#define CM_FREE_PTR_ARRAY_OF_IVPAIR_ARRAY(iv_pair_array_p,count_ui)\
{\
	 uint32_t rec_cnt_ui, iv_cnt_ui=0;\
	 \
	 for(rec_cnt_ui = 0; rec_cnt_ui < count_ui; rec_cnt_ui++)\
	 {\
			for (iv_cnt_ui=0; iv_cnt_ui < iv_pair_array_p[rec_cnt_ui].count_ui; iv_cnt_ui++)\
			{\
				 CM_FREE_IVPAIR(iv_pair_array_p[rec_cnt_ui].iv_pairs[iv_cnt_ui]);\
			}\
			if ( iv_pair_array_p[rec_cnt_ui].count_ui > 0 )\
			{\
				 of_free(iv_pair_array_p[rec_cnt_ui].iv_pairs);\
			}\
	 }\
	 of_free(iv_pair_array_p);\
}



#define CM_FREE_PTR_IVPAIR_MEMBEROF_ARRAY(iv_pair_p)\
{\
	 /*Free Memory for Element Value*/\
	 if(iv_pair_p->value_p)\
	 {\
			if(iv_pair_p->value_length > 0 )\
			{\
				 of_free(iv_pair_p->value_p);\
				 iv_pair_p->value_p=NULL;\
			}\
			else\
			{\
				 CM_DEBUG_PRINT("Something wrong in iv_pair Value");\
			}\
	 }\
	 else\
	 {\
			if(iv_pair_p->value_length > 0 )\
			{\
				 CM_DEBUG_PRINT("Value is NULL and its len is greater than zero ????");\
			}\
	 }\
}



#define CM_PRINT_IVPAIR(iv_pair_p)\
   CM_DEBUG_PRINT ("id_ui=%2d ValType=%2d Len=%3d Value=%s",\
         iv_pair_p.id_ui,   iv_pair_p.value_type,\
         iv_pair_p.value_length, (char *) iv_pair_p.value_p)

#define CM_PRINT_PTR_IVPAIR(iv_pair_p) \
if (iv_pair_p)\
{\
	 CM_DEBUG_PRINT ("id_ui=%2d ValType=%2d Len=%3d Value=%s",\
				 iv_pair_p->id_ui,   iv_pair_p->value_type,\
				 iv_pair_p->value_length, (char *) iv_pair_p->value_p);\
}\
else\
{\
	 CM_DEBUG_PRINT("Iv Pair is null, unable to print");\
}

#define CM_IPV6_ADDR_LEN 16

#define CM_GET_NVPAIR_LEN(nv_pair, uiLen) \
{\
	   uiLen += CM_UINT32_SIZE; \
	   uiLen += nv_pair.name_length; \
	   uiLen += CM_UINT32_SIZE;\
	   uiLen += CM_UINT32_SIZE;\
	   uiLen += nv_pair.value_length;}

#endif
