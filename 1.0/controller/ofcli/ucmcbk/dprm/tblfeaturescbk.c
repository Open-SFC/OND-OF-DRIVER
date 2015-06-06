/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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
 * File name:  tblfeaturescbk.c
 * Author: Freescale Semiconductor    
 * Date:   05/20/2013
 * Description:  
*/

#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"

int32_t of_tblfeatures_appl_ucmcbk_init(void);
int32_t of_tblfeatures_addrec (void * pConfigTransaction,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** pResult);

int32_t of_tblfeatures_setrec (void * pConfigTransaction,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** pResult);

int32_t of_tblfeatures_delrec (void * pConfigTransction,
			struct cm_array_of_iv_pairs * pKeysArr,
			struct cm_app_result ** pResult);

int32_t of_tblfeatures_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
			uint32_t * pCount,
			struct cm_array_of_iv_pairs ** pArrayofIvPairArr);

int32_t of_tblfeatures_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
			struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * pCount,
			struct cm_array_of_iv_pairs ** pNextNRecordData_p);

int32_t of_tblfeatures_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
			struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t of_tblfeatures_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
			uint32_t uiCommand, struct cm_app_result ** pResult);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
 int32_t of_tblfeatures_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **ppResult);

 int32_t of_tblfeatures_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** ppResult);


 int32_t of_tblfeatures_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_datapath_general_info *datapath_info,
			struct ofi_table_features_info* table_info,
			struct cm_app_result ** ppResult);

 int32_t of_tblfeatures_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct ofi_table_features_info * table_info,
			struct cm_app_result ** ppResult);


 int32_t of_tblfeatures_ucm_getparams (struct ofi_table_features_info *table_info,
			struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_tablefeatures_ucm_callbacks = {
	 NULL,
         NULL,
	 NULL,
	 of_tblfeatures_delrec,
	 NULL,
	 of_tblfeatures_getfirstnrecs,
	 of_tblfeatures_getnextnrecs,
	 of_tblfeatures_getexactrec,
	 NULL,
	 NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_tblfeatures_appl_ucmcbk_init(void)
{
	 CM_CBK_DEBUG_PRINT ("Entered");
  cm_register_app_callbacks(CM_ON_DIRECTOR_DATAPATH_STATS_TABLEFEATURES_APPL_ID,&of_tablefeatures_ucm_callbacks);
	 return OF_SUCCESS;
}



int32_t of_tblfeatures_getfirstnrecs (struct cm_array_of_iv_pairs * pKeysArr,
			uint32_t * pCount,
			struct cm_array_of_iv_pairs ** pArrayofIvPairArr)
{
	 CM_CBK_DEBUG_PRINT ("Entered");
	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 int32_t iRetVal = OF_FAILURE;
	 uint32_t uiRecCount = 0;
	 struct cm_app_result *tbl_result = NULL;
	 struct dprm_datapath_general_info datapath_info={};
	 struct ofi_table_features_info table_info;
	 struct dprm_port_runtime_info runtime_info;
	 uint64_t datapath_handle;
	 uint64_t port_handle;

	 of_memset (&table_info, 0, sizeof (struct ofi_table_features_info));

	 if ((of_tblfeatures_ucm_setmandparams (pKeysArr,&datapath_info, &table_info, &tbl_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 iRetVal=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	 if (iRetVal != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get DPRM Handle failed");
			return OF_FAILURE;
	 }
						CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", datapath_info.dpid);
						CM_CBK_DEBUG_PRINT("dphandle is: %llx\r\n", datapath_handle);
#if 0
	 iRetVal=dprm_get_first_datapath_port(datapath_handle, &table_info, &runtime_info, &port_handle);
	 if (iRetVal != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get first record failed for DPRM Table");
			return OF_FAILURE;
	 }
#endif
	 CM_CBK_DEBUG_PRINT ("call to cntlr_send_tablefeature_request");
	 iRetVal=cntlr_send_tablefeature_request(datapath_handle); 
	 if ( iRetVal != OF_SUCCESS)
	 {
		 CM_CBK_DEBUG_PRINT ("table features failed");
		 return OF_FAILURE;
	 } 
	 CM_CBK_DEBUG_PRINT ("table features is sent as multipart response");
	 *pCount = uiRecCount;
	 return CM_CNTRL_MULTIPART_RESPONSE;
	 
}


int32_t of_tblfeatures_getnextnrecs (struct cm_array_of_iv_pairs * pKeysArr,
			struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * pCount,
			struct cm_array_of_iv_pairs ** pNextNRecordData_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *tbl_result = NULL;
	 int32_t iRetVal = OF_FAILURE;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 *pCount=0;
         return iRetVal;

}

int32_t of_tblfeatures_delrec (void * pConfigTransction,
			struct cm_array_of_iv_pairs * pKeysArr,
			struct cm_app_result ** pResult)
{
  CM_CBK_DEBUG_PRINT ("Entered");
  return OF_SUCCESS;
}

int32_t of_tblfeatures_getexactrec (struct cm_array_of_iv_pairs * pKeysArr,
			struct cm_array_of_iv_pairs ** pIvPairArr)
{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	struct cm_app_result *tbl_result = NULL;
	int32_t iRetVal = OF_FAILURE;

	CM_CBK_DEBUG_PRINT ("Entered");
	return iRetVal;
}

int32_t of_tblfeatures_verifycfg (struct cm_array_of_iv_pairs * pKeysArr,
			uint32_t uiCommand, struct cm_app_result ** pResult)
{
	 struct cm_app_result *tbl_result = NULL;
	 int32_t iRetVal = OF_FAILURE;
	 struct ofi_table_features_info table_info = { };

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&table_info, 0, sizeof (struct ofi_table_features_info));

	 iRetVal = of_tblfeatures_ucm_validatemandparams (pKeysArr, &tbl_result);
	 if (iRetVal != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if (datapath_ucm_validateoptparams (pKeysArr, &tbl_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 *pResult = tbl_result;
	 return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

 int32_t of_tblfeatures_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct cm_app_result **ppResult)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *tbl_result = NULL;
	 uint32_t uiPortId;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				default:
						break;

			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t of_tblfeatures_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** ppResult)
{
	 uint32_t uiOptParamCnt, uiTableCnt;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
	 {
			switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
			{
				 default:
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
	 return OF_SUCCESS;
}

 int32_t of_tblfeatures_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_datapath_general_info *datapath_info,
			struct ofi_table_features_info* table_info,		struct cm_app_result ** ppResult)
{
	 uint32_t uiMandParamCnt;
	 uint64_t idpId ;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DATAPATH_DATAPATHID_ID:
						idpId=charTo64bitNum((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
						datapath_info->dpid=idpId;
						CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
						break;

			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t of_tblfeatures_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct ofi_table_features_info * table_info,
			struct cm_app_result ** ppResult)
{
	 uint32_t uiOptParamCnt;

	 CM_CBK_DEBUG_PRINT ("Entered");

	 for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
	 {
			switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
			{
				 default:
						break;
			}
	 }

	 CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
	 return OF_SUCCESS;
}



 int32_t of_tblfeatures_ucm_getparams (struct ofi_table_features_info *table_info,
			struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	 char string[64], string2[4096];
	 int32_t index = 0, jj, count_ui, iIndex = 0;
#define CM_TABLEFEATURES_CHILD_COUNT 200
	 count_ui = CM_TABLEFEATURES_CHILD_COUNT;

	 result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
	 if(!result_iv_pairs_p->iv_pairs)
	 {
			CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
			return OF_FAILURE;
	 }

         of_memset(string, 0, sizeof(string)); 
         strcpy(string, table_info->name);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLEFEATURES_NAME_ID,CM_DATA_TYPE_STRING, string);
	 index++;
	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->table_id, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLEFEATURES_ID_ID,CM_DATA_TYPE_STRING, string);
	 index++;

	 of_memset(string, 0, sizeof(string));
         of_sprintf(string,"%llx", table_info->metadata_match);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLEFEATURES_METADATA_MATCH_ID,CM_DATA_TYPE_STRING, string);
	 index++;
	 
	 of_memset(string, 0, sizeof(string));
         of_sprintf(string,"%llx", table_info->metadata_write);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLEFEATURES_METADATA_WRITE_ID,CM_DATA_TYPE_STRING, string);
	 index++;
        
/*  //Deepthi - must uncomment this -  config should be added to ofi_table_features... structure. 
	 of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->config, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLEFEATURES_CONFIG_ID,CM_DATA_TYPE_STRING, string);
	 index++;
*/
	 of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->max_entries, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_TABLEFEATURES_MAX_ENTRIES_ID,CM_DATA_TYPE_STRING, string);
	 index++;

	 of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_flow_inst, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_FLOW_INST_ID, CM_DATA_TYPE_STRING, string);
	 index++;
         
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_flow_inst; iIndex++)
         { 
	   of_memset(string, 0, sizeof(string));
           switch(*((uint16_t*)table_info->flow_instructions+iIndex))
	   {
	     case 1:
		strcat(string2, "goto");
		break;
	     case 2:
		strcat(string2, "meta");
		break;
	     case 3:
		strcat(string2, "write");
		break;
	     case 4:
		strcat(string2, "apply");
		break;
	     case 5:
		strcat(string2, "clear");
		break;
	     case 6:
		strcat(string2, "meter");
		break;
	     case 7:
		strcat(string2, "experimenter");
		break;
	     default:
		break;
	   }
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_FLOW_INSTRUCTIONS_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_miss_inst, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_MISS_INST_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_miss_inst ; iIndex++)
         { 
           of_memset(string, 0, sizeof(string));
           switch(*((uint16_t*)table_info->miss_entry_instructions+iIndex))
	   {
	     case 1:
		strcat(string2, "goto");
		break;
	     case 2:
		strcat(string2, "meta");
		break;
	     case 3:
		strcat(string2, "write");
		break;
	     case 4:
		strcat(string2, "apply");
		break;
	     case 5:
		strcat(string2, "clear");
		break;
	     case 6:
		strcat(string2, "meter");
		break;
	     case 7:
		strcat(string2, "experimenter");
		break;
	     default:
		break;
	   }
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_MISS_ENTRY_INSTRUCTIONS_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
         
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_next_tbl, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_NEXT_TBL_ID, CM_DATA_TYPE_STRING, string);
	 index++;   //9
	
         //if(table_info->no_of_next_tbl > 0)
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_next_tbl ; iIndex++)
         { 
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint8_t*)table_info->next_tbl+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NEXT_TBL_ID, CM_DATA_TYPE_STRING, string2);
	 index++; //10
	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_next_tbls_4_miss, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_NEXT_TBLS_FOR_MISS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         //if(table_info->no_of_next_tbls_4_miss > 0)
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_next_tbls_4_miss ; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint8_t*)table_info->next_tbl_4_flow_miss+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NEXT_TBL_FOR_FLOW_MISS_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
         
#if 0	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_write_actns, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_WRITE_ACTNS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	
         //if(table_info->no_of_write_actns > 0)
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_write_actns ; iIndex++)
         { 
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint16_t*)table_info->flow_entry_write_actns+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_FLOW_ENTRY_WRITE_ACTNS_ID, CM_DATA_TYPE_STRING, string2);
         index++; 
         
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_write_actns_4_miss, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_WRITE_ACTNS_4_MISS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         //if(table_info->no_of_write_actns_4_miss > 0)
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_write_actns_4_miss ; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint16_t*)table_info->miss_entry_write_actns+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_MISS_ENTRY_WRITE_ACTNS_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
         
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_apply_actns, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_APPLY_ACTNS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
         
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_apply_actns; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint16_t*)table_info->flow_entry_apply_actns+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_FLOW_ENTRY_APPLY_ACTNS_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_apply_actns_4_miss, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_APPLY_ACTNS_4_MISS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_apply_actns_4_miss; iIndex++)
         { 
           of_memset(string, 0, sizeof(string));
//	   of_itoa (((uint16_t*)table_info->miss_entry_apply_actns)[iIndex], string);
	   of_itoa (*((uint16_t*)table_info->miss_entry_apply_actns+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_MISS_ENTRY_APPLY_ACTNS_ID, CM_DATA_TYPE_STRING, string2);
	 index++; //0-20

         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_match_fields, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_MATCH_FIELDS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	
        //TO-DO in of_cntlr_msg.c file 
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_match_fields; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint32_t*)table_info->match_field_info+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_MATCH_FIELD_INFO_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_wildcard_fields , string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_WILDCARD_FIELDS_ID, CM_DATA_TYPE_STRING, string);
	 index++;

         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_wildcard_fields; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint32_t*)table_info->wildcard_field_info +iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_WILDCARD_FIELD_INFO_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_wr_set_fields, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_WR_SET_FIELDS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_wr_set_fields; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint32_t*)table_info->flow_wr_set_field_info+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
	 }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_FLOW_WR_SET_FIELD_INFO_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_wr_set_fields_miss, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_WR_SET_FIELDS_MISS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_wr_set_fields_miss; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint32_t*)table_info->miss_wr_entry_set_field_info+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_MISS_WR_ENTRY_SET_FIELD_INFO_ID, CM_DATA_TYPE_STRING, string2);
	 index++; //28
	 
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_ap_set_fields, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_AP_SET_FIELDS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_ap_set_fields; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
  	   of_itoa (*((uint32_t*)table_info->flow_ap_set_field_info+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
	 }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_FLOW_AP_SET_FIELD_INFO_ID, CM_DATA_TYPE_STRING, string2);
	 index++; //30
         
         of_memset(string, 0, sizeof(string));
	 of_itoa (table_info->no_of_ap_set_fields_miss, string);
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_NO_OF_AP_SET_FIELDS_MISS_ID, CM_DATA_TYPE_STRING, string);
	 index++;
	 
         of_memset(string2, 0, sizeof(string2));
         for(iIndex = 0; iIndex < table_info->no_of_ap_set_fields_miss; iIndex++)
         {
           of_memset(string, 0, sizeof(string));
	   of_itoa (*((uint32_t*)table_info->miss_ap_entry_set_field_info+iIndex), string);
           strcat(string2, string);
           strcat(string2, " ");
         }
	 FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index], CM_DM_TABLEFEATURES_MISS_AP_ENTRY_SET_FIELD_INFO_ID, CM_DATA_TYPE_STRING, string2);
	 index++;
     
#endif	
         result_iv_pairs_p->count_ui = index;
	 return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
