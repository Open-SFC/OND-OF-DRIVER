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
 * File name: dprmtblcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"



int32_t dprm_table_appl_ucmcbk_init (void);

int32_t dprm_table_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t dprm_table_setrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);

int32_t dprm_table_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t dprm_table_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t dprm_table_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t dprm_table_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t dprm_table_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
 int32_t dprm_table_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct cm_app_result **
			presult_p);

 int32_t dprm_table_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p);


 int32_t dprm_table_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,
			struct dprm_distributed_forwarding_domain_info *of_domain,
			struct dprm_table_info* dprm_table,
			struct cm_app_result ** presult_p);

 int32_t dprm_table_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_table_info * dprm_table,
			struct cm_app_result ** presult_p);


 int32_t dprm_table_ucm_getparams (struct dprm_table_info *dprm_table,
			struct cm_array_of_iv_pairs * result_iv_pairs_p);

 int32_t dprm_table_ucm_showtmandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct dprm_distributed_forwarding_domain_info *of_domain,
			struct dprm_table_info* dprm_table);


struct cm_dm_app_callbacks dprm_table_ucm_callbacks = {
	 NULL,
	 dprm_table_addrec,
	 NULL,
	 dprm_table_delrec,
	 NULL,
	 dprm_table_getfirstnrecs,
	 dprm_table_getnextnrecs,
	 dprm_table_getexactrec,
	 dprm_table_verifycfg,
	 NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t dprm_table_appl_ucmcbk_init (void)
{
	 CM_CBK_DEBUG_PRINT ("Entry");
	 cm_register_app_callbacks (CM_ON_DIRECTOR_DOMAIN_DOMAINTABLE_APPL_ID,&dprm_table_ucm_callbacks);
	 return OF_SUCCESS;
}

int32_t dprm_table_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *dprm_table_result = NULL;
	 int32_t return_value = OF_FAILURE,ii;
	 struct dprm_table_info dprm_table = { };
	 struct dprm_oftable_entry *oftable_entry=NULL;
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 struct    dprm_match_field* match_field_p = NULL;
	 struct dprm_table_info *dprm_table_p=NULL;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info ));

	 if ((dprm_table_ucm_setmandparams (pMandParams, &of_domain, &dprm_table, &dprm_table_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p=dprm_table_result;
			return OF_FAILURE;
	 }

	 return_value = dprm_table_ucm_setoptparams (pOptParams, &dprm_table, &dprm_table_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("dprm_get_forwarding_domain_handle  failed");
			return OF_FAILURE;
	 }

	 if (dprm_table.no_of_match_fields > 0)
	 {
			dprm_table_p=(struct   dprm_table_info *)of_calloc(1,sizeof(struct   dprm_table_info) + (dprm_table.no_of_match_fields *sizeof(struct dprm_match_field)));

			match_field_p = dprm_table_p->match_field_p;
			for(ii = 0;(ii < dprm_table.no_of_match_fields);ii++)
			{
				 match_field_p[ii].type = ii;
				 if (ii % 2 == 0 )
						match_field_p[ii].mask = 0;
				 else
						match_field_p[ii].mask = 1;
			}
			of_strncpy(dprm_table_p->table_name,dprm_table.table_name,DPRM_MAX_LEN_OF_TABLE_NAME);
			dprm_table_p->table_id =dprm_table.table_id;
			dprm_table_p->no_of_match_fields=dprm_table.no_of_match_fields;
	 }
	 else
	 {
			dprm_table_p=&dprm_table;
	 }
	 return_value = dprm_add_oftable_to_domain(domain_handle, dprm_table_p,&dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Domain Table Addition Failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }

	 return_value = get_domain_table_byname(domain_handle,dprm_table_p->table_name,&oftable_entry);
	 if(return_value != DPRM_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("get_domain_table_byname failed %s",dprm_table.table_name);
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }

	 return_value = add_domain_oftable_to_idbased_list(domain_handle,
				 dprm_table_p->table_id,oftable_entry);
	 if(return_value != DPRM_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT(" add_domain_oftable_to_idbased_list  failed %d", dprm_table.table_id);
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }

	 if ((dprm_table_p) && (dprm_table_p->no_of_match_fields > 0 ))
	 {
     of_free(dprm_table_p);
	 }
	 return OF_SUCCESS;
}

int32_t dprm_table_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
	 struct cm_app_result *dprm_table_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_table_info dprm_table = { };
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 struct dprm_distributed_forwarding_domain_info of_domain={};

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 if ((dprm_table_ucm_setmandparams (keys_arr_p,&of_domain, &dprm_table, &dprm_table_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }

	 return_value=dprm_get_oftable_handle(domain_handle, dprm_table.table_name,&dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get DPRM Handle failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }
	 
	 return_value = dprm_delete_oftable_from_domain(domain_handle, dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Delete Table record failed");
			fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
			*result_p = dprm_table_result;
			return OF_FAILURE;
	 }
	 return OF_SUCCESS;
}
#if 1


int32_t dprm_table_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 int32_t return_value = OF_FAILURE;
	 uint32_t uiRecCount = 0;
	 struct cm_app_result *dprm_table_result = NULL;
	 struct dprm_table_info dprm_table = { };
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 struct dprm_distributed_forwarding_domain_info of_domain={};


	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 if (( dprm_table_ucm_showmandparams (keys_arr_p,&of_domain, &dprm_table)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_first_domain_oftable(domain_handle, &dprm_table, &dprm_handle);
	 if (!( return_value == DPRM_ERROR_NO_OF_MATCH_FIELDS) || (return_value == DPRM_SUCCESS))
	 {
			CM_CBK_DEBUG_PRINT ("Get first record failed for DPRM Table");
			return OF_FAILURE;
	 }

	 result_iv_pairs_p =
			(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	 if (result_iv_pairs_p == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
	 }
	 dprm_table_ucm_getparams (&dprm_table, &result_iv_pairs_p[uiRecCount]);
	 uiRecCount++;
	 *array_of_iv_pair_arr_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}


int32_t dprm_table_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *dprm_table_result = NULL;
	 struct dprm_table_info dprm_table;
	 struct    dprm_distributed_forwarding_domain_runtime_info runtime_info;
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 int32_t return_value = OF_FAILURE;
	 uint32_t uiRecCount = 0;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 if ((dprm_table_ucm_showmandparams (keys_arr_p,&of_domain, &dprm_table)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
	 if ((dprm_table_ucm_showmandparams (prev_record_key_p,&of_domain, &dprm_table)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_oftable_handle(domain_handle, dprm_table.table_name,&dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_next_domain_oftable(domain_handle, &dprm_table, &dprm_handle);
	 if (( return_value == DPRM_ERROR_NO_OF_MATCH_FIELDS) || (return_value == DPRM_SUCCESS))
	 {
			CM_CBK_DEBUG_PRINT ("Next domain  record is : %s ",
						dprm_table.table_name);
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
    if (result_iv_pairs_p == NULL)
    {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
    }
			dprm_table_ucm_getparams (&dprm_table, &result_iv_pairs_p[uiRecCount]);
			uiRecCount++;
	 }

	 CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
	 CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
	 *next_n_record_data_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 return OF_SUCCESS;
}

int32_t dprm_table_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct dprm_table_info dprm_table;
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 uint32_t ii = 0;
	 int32_t return_value = OF_FAILURE;
	 struct cm_app_result *dprm_table_result = NULL;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 if ((dprm_table_ucm_showmandparams (keys_arr_p, &of_domain, &dprm_table)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("dprm_get_forwarding_domain_handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_oftable_handle(domain_handle, dprm_table.table_name,&dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));
	 
	 return_value=dprm_get_exact_domain_oftable(domain_handle, &dprm_table, dprm_handle );
	 if (!( return_value == DPRM_ERROR_NO_OF_MATCH_FIELDS) || (return_value == DPRM_SUCCESS))
	 {
			CM_CBK_DEBUG_PRINT ("Unable to find the matching record ");
    return OF_FAILURE;
	 }
	 else
	 {
			CM_CBK_DEBUG_PRINT ("Exact matching record found");
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
    if (result_iv_pairs_p == NULL)
    {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
    }
			dprm_table_ucm_getparams (&dprm_table, result_iv_pairs_p);
			*pIvPairArr = result_iv_pairs_p;
	 }
  return OF_SUCCESS;
}
#else
int32_t dprm_table_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 int32_t return_value = OF_FAILURE;
	 uint32_t uiRecCount = 0;
	 struct cm_app_result *dprm_table_result = NULL;
	 struct dprm_table_info dprm_table = { };
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 struct dprm_table_info *dprm_table_p=NULL;


	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 if ((dprm_table_ucm_setmandparams (keys_arr_p,&of_domain, &dprm_table, &dprm_table_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_first_domain_oftable(domain_handle, &dprm_table, &dprm_handle);
	 if ( return_value == DPRM_ERROR_NO_OF_MATCH_FIELDS)
	 {
			dprm_table_p=(struct   dprm_table_info *)of_calloc(1,sizeof(struct   dprm_table_info) + (dprm_table.no_of_match_fields *sizeof(struct dprm_match_field)));
			dprm_table_p->no_of_match_fields=dprm_table.no_of_match_fields;
			return_value=dprm_get_first_domain_oftable(domain_handle, dprm_table_p, &dprm_handle);
	 }
	 else
			dprm_table_p=&dprm_table;
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Get first record failed for DPRM Table");
			return OF_FAILURE;
	 }

	 result_iv_pairs_p =
			(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	 if (result_iv_pairs_p == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
	 }
	 dprm_table_ucm_getparams (dprm_table_p, &result_iv_pairs_p[uiRecCount]);
	 uiRecCount++;
	 *array_of_iv_pair_arr_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 if ((dprm_table_p) && (dprm_table_p->no_of_match_fields > 0 ))
	 {
     of_free(dprm_table_p);
	 }
	 return OF_SUCCESS;
}


int32_t dprm_table_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
			struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
	 struct dprm_table_info *dprm_table_p=NULL;
	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	 struct cm_app_result *dprm_table_result = NULL;
	 struct dprm_table_info dprm_table={};
	 struct    dprm_distributed_forwarding_domain_runtime_info runtime_info;
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 int32_t return_value = OF_FAILURE;
	 uint32_t uiRecCount = 0;
   
	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));
	 result_iv_pairs_p =
			(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	 if (result_iv_pairs_p == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
	 }

	 if ((dprm_table_ucm_setmandparams (keys_arr_p,&of_domain, &dprm_table, &dprm_table_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
	 if ((dprm_table_ucm_setmandparams (prev_record_key_p,&of_domain, &dprm_table, &dprm_table_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get forwarding handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_oftable_handle(domain_handle, dprm_table.table_name,&dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_next_domain_oftable(domain_handle, &dprm_table, &dprm_handle);
	 if ( return_value == DPRM_ERROR_NO_OF_MATCH_FIELDS)
	 {
			dprm_table_p=(struct  dprm_table_info *)of_calloc(1,sizeof(struct   dprm_table_info) + (dprm_table.no_of_match_fields *sizeof(struct dprm_match_field)));

			dprm_table_p->no_of_match_fields=dprm_table.no_of_match_fields;
			return_value=dprm_get_next_domain_oftable(domain_handle, dprm_table_p, &dprm_handle);
	 }
	 else
			dprm_table_p=&dprm_table;
	 if (return_value == OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Next domain  record is : %s",
						dprm_table_p->table_name);
			dprm_table_ucm_getparams (dprm_table_p, &result_iv_pairs_p[uiRecCount]);
			uiRecCount++;
	 }

	 CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
	 CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount);
	 *next_n_record_data_p = result_iv_pairs_p;
	 *count_p = uiRecCount;
	 if ((dprm_table_p) && (dprm_table_p->no_of_match_fields > 0 ))
	 {
			of_free(dprm_table_p);
	 }
	 return OF_SUCCESS;
}

int32_t dprm_table_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs ** pIvPairArr)
{

	 struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   struct    dprm_match_field* match_field_p = NULL;
	 struct dprm_table_info *dprm_table_p=NULL;
	 struct dprm_table_info dprm_table;
	 struct dprm_distributed_forwarding_domain_info of_domain={};
	 uint64_t domain_handle;
	 uint64_t dprm_handle;
	 uint32_t ii = 0;
	 int32_t return_value = OF_FAILURE;
	 struct cm_app_result *dprm_table_result = NULL;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 if ((dprm_table_ucm_setmandparams (keys_arr_p, &of_domain, &dprm_table, &dprm_table_result)) !=
				 OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
			return OF_FAILURE;
	 }
	 return_value=dprm_get_forwarding_domain_handle(of_domain.name, &domain_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("dprm_get_forwarding_domain_handle failed");
			return OF_FAILURE;
	 }

	 return_value=dprm_get_oftable_handle(domain_handle, dprm_table.table_name,&dprm_handle);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
			return OF_FAILURE;
	 }
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));
	 
	 return_value=dprm_get_exact_domain_oftable(domain_handle, &dprm_table, dprm_handle );
	 if ( return_value == DPRM_ERROR_NO_OF_MATCH_FIELDS)
	 {
			dprm_table_p->no_of_match_fields=dprm_table.no_of_match_fields;
			return_value=dprm_get_next_domain_oftable(domain_handle, &dprm_table, &dprm_handle);
	 }
	 else
			dprm_table_p= &dprm_table;

	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Unable to find the matching record ");
	 }
	 else
	 {
			CM_CBK_DEBUG_PRINT ("Exact matching record found");
    result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
    if (result_iv_pairs_p == NULL)
    {
      CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
      return OF_FAILURE;
    }
			dprm_table_ucm_getparams (dprm_table_p, result_iv_pairs_p);
			*pIvPairArr = result_iv_pairs_p;
			if ((dprm_table_p) && (dprm_table_p->no_of_match_fields > 0 ))
			{
				 of_free(dprm_table_p);
			}
	 }
	 return return_value;
}
#endif
int32_t dprm_table_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
	 struct cm_app_result *dprm_table_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_table_info dprm_table = { };

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&dprm_table, 0, sizeof (struct dprm_table_info));

	 return_value = dprm_table_ucm_validatemandparams (keys_arr_p, &dprm_table_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if (dprm_table_ucm_validateoptparams (keys_arr_p, &dprm_table_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 *result_p = dprm_table_result;
	 return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

#define CM_DOMAINTABLE_MAX_TABLEID 255

 int32_t dprm_table_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct cm_app_result **presult_p)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *dprm_table_result = NULL;
  uint8_t switch_type, uiMatchFldCnt, uiTableId;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DOMAIN_NAME_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Domain Name is NULL");
							 fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = dprm_table_result;
							 return OF_FAILURE;
						}
						break;

				 case CM_DM_DOMAINTABLE_TABLEID_ID: 
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Subject Name is NULL");
							 fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = dprm_table_result;
							 return OF_FAILURE;
						}
        uiTableId=(uint8_t)of_atoi(pMandParams->iv_pairs[uiMandParamCnt].value_p);
						if (uiTableId < 0 ||  uiTableId > CM_DOMAINTABLE_MAX_TABLEID)
						{
							 CM_CBK_DEBUG_PRINT ("Number of tables Exceeding its maximum Number");
							 fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = dprm_table_result;
							 return OF_FAILURE;
						}
						break;

				 case CM_DM_DOMAINTABLE_NUMOFMATCHFIELDS_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Number of tables is NULL");
							 fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = dprm_table_result;
							 return OF_FAILURE;
						}
						uiMatchFldCnt =
           ucm_uint8_from_str_ptr (pMandParams->iv_pairs[uiMandParamCnt].value_p);
						if (uiMatchFldCnt < 0 ||  uiMatchFldCnt > 24)
						{
							 CM_CBK_DEBUG_PRINT ("Number of Matching fields");
							 fill_app_result_struct (&dprm_table_result, NULL, CM_GLU_VLAN_FAILED);
							 *presult_p = dprm_table_result;
							 return OF_FAILURE;
						}
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t dprm_table_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt, uiMatchFldCnt;

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

 int32_t dprm_table_ucm_setmandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct dprm_distributed_forwarding_domain_info *of_domain,
			struct dprm_table_info* dprm_table,		struct cm_app_result ** presult_p)
{
	 uint32_t uiMandParamCnt;
	 uint32_t switch_type;
  uint8_t iMatchFldCnt,iTableId;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DOMAIN_NAME_ID:
						of_strncpy (of_domain->name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;

				 case CM_DM_DOMAINTABLE_TABLENAME_ID:
						of_strncpy (dprm_table->table_name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;

				 case CM_DM_DOMAINTABLE_TABLEID_ID:
        iTableId=of_atoi(pMandParams->iv_pairs[uiMandParamCnt].value_p);
						dprm_table->table_id=(uint8_t)iTableId;
						break;

				 case CM_DM_DOMAINTABLE_NUMOFMATCHFIELDS_ID:
        iMatchFldCnt = of_atoi(pMandParams->iv_pairs[uiMandParamCnt].value_p);
						dprm_table->no_of_match_fields=(uint8_t)iMatchFldCnt;
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t dprm_table_ucm_showmandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct dprm_distributed_forwarding_domain_info *of_domain,
			struct dprm_table_info* dprm_table)
{
	 uint32_t uiMandParamCnt;
	 uint32_t switch_type;
	 int32_t iMatchFldCnt,iTableId;

	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_DOMAIN_NAME_ID:
						of_strncpy (of_domain->name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;

				 case CM_DM_DOMAINTABLE_TABLENAME_ID:
						of_strncpy (dprm_table->table_name,
									(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
									pMandParams->iv_pairs[uiMandParamCnt].value_length);
						break;
#if 0
				 case CM_DM_DOMAINTABLE_TABLEID_ID:
						iTableId=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
						dprm_table->table_id=(uint8_t)iTableId;
						break;
#endif
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}
 int32_t dprm_table_ucm_setoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct dprm_table_info * dprm_table,
			struct cm_app_result ** presult_p)
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



 int32_t dprm_table_ucm_getparams (struct dprm_table_info *dprm_table,
			struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	 uint32_t uindex_i = 0;
	 char buf[64] = "";
#define CM_DOMAINTABLE_CHILD_COUNT 3

	 result_iv_pairs_p->iv_pairs =
			(struct cm_iv_pair *) of_calloc (CM_DOMAINTABLE_CHILD_COUNT, sizeof (struct cm_iv_pair));
	 if (result_iv_pairs_p->iv_pairs == NULL)
	 {
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
			return OF_FAILURE;
	 }


	 CM_CBK_DEBUG_PRINT("id %d match %d name %s", dprm_table->table_id,dprm_table->no_of_match_fields,
				 dprm_table->table_name);

	 /* Domain Name - key param*/
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DOMAINTABLE_TABLENAME_ID,
				 CM_DATA_TYPE_STRING, dprm_table->table_name);
	 uindex_i++;

	 /*Domains Subject name for authentication*/
	 of_memset(buf,0,64);
	 //buf[0]=dprm_table->table_id;
	 //buf[1]='\0';
   sprintf(buf,"%d",dprm_table->table_id);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_DOMAINTABLE_TABLEID_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;

	 /*Number of Domain tables*/
	 of_memset(buf,0,64);
//	 buf[0]=dprm_table->no_of_match_fields;
	// buf[1]='\0';
   sprintf(buf,"%d",dprm_table->no_of_match_fields);
	 FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_DOMAINTABLE_NUMOFMATCHFIELDS_ID,
				 CM_DATA_TYPE_STRING, buf);
	 uindex_i++;

	 result_iv_pairs_p->count_ui = uindex_i;
	 
	 CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
	 return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
