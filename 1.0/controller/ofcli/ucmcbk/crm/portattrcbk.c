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
 * File name: portattributecbk.c
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "crmapi.h"
#include "crm_vm_api.h"
#include "crm_port_api.h"


int32_t crm_port_attribute_appl_ucmcbk_init (void);

int32_t crm_port_attribute_addrec (void * config_trans_p,
			struct cm_array_of_iv_pairs * pMandParams,
			struct cm_array_of_iv_pairs * pOptParams,
			struct cm_app_result ** result_p);
int32_t crm_port_attribute_setrec (void * config_trans_p,
		  		     struct cm_array_of_iv_pairs * pMandParams,
			             struct cm_array_of_iv_pairs * pOptParams,
			             struct cm_app_result ** result_p);


int32_t crm_port_attribute_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);

int32_t crm_port_attribute_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p);

int32_t crm_port_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t crm_port_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t crm_port_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * attribute  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t crm_port_attribute_ucm_validatemandparams (struct cm_array_of_iv_pairs *pMandParams,
			struct cm_app_result **presult_p);



int32_t crm_port_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
			char *port_name_p,
			struct crm_attribute_name_value_pair *attribute_info,
			struct cm_app_result                  **presult_p);


int32_t crm_attribute_port_getparams (struct crm_attribute_name_value_output_info *attribute_info,
                struct cm_array_of_iv_pairs * result_iv_pairs_p);




struct cm_dm_app_callbacks crm_port_attribute_ucm_callbacks = 
{
  NULL,
  crm_port_attribute_addrec,
  crm_port_attribute_setrec,
  crm_port_attribute_delrec,
  NULL,
  crm_port_attribute_getfirstnrecs,
  crm_port_attribute_getnextnrecs,
  crm_port_attribute_getexactrec,
  crm_port_attribute_verifycfg,
  NULL
};

int32_t crm_port_attribute_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_VIRTUALNETWORK_COMPUTENODES_VMSIDEPORTS_PORTATTR_APPL_ID, &crm_port_attribute_ucm_callbacks);
  return OF_SUCCESS;
}

int32_t crm_port_attribute_addrec (void * config_trans_p,
		  		     struct cm_array_of_iv_pairs * pMandParams,
			             struct cm_array_of_iv_pairs * pOptParams,
			             struct cm_app_result ** result_p)
{
  struct   cm_app_result *attribute_result = NULL;
  struct   crm_attribute_name_value_pair crm_attribute_name_value_info={};
  char port_name[CRM_MAX_VM_NAME_LEN+1];

  

  uint64_t port_handle;
  int32_t  return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&crm_attribute_name_value_info, 0, sizeof (struct crm_attribute_name_value_pair));

  of_memset(port_name, 0, sizeof(port_name));
  if ((crm_port_attribute_ucm_setmandparams (pMandParams, 
	      port_name, &crm_attribute_name_value_info, 
	      &attribute_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = attribute_result;
    return OF_FAILURE;
  }
  return_value = crm_get_port_handle(port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("port does not exist with name %s",port_name);
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_VM_NAME_NULL);
    *result_p = attribute_result;
    return OF_FAILURE;
  }
  return_value = crm_add_attribute_to_port(port_name, &crm_attribute_name_value_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Attribute Addition Failed(ret:%d",return_value);
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_ATTRIBUTE_ADD_FAILED);
    *result_p = attribute_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Attribute added succesfully");
  return OF_SUCCESS;

}
int32_t crm_port_attribute_setrec (void * config_trans_p,
		  		     struct cm_array_of_iv_pairs * pMandParams,
			             struct cm_array_of_iv_pairs * pOptParams,
			             struct cm_app_result ** result_p)
{
  struct   cm_app_result *attribute_result = NULL;
  struct   crm_attribute_name_value_pair crm_attribute_name_value_info={};
  char port_name[CRM_MAX_VM_NAME_LEN+1];

  uint64_t port_handle;
  int32_t  return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&crm_attribute_name_value_info, 0, sizeof (struct crm_attribute_name_value_pair));

  of_memset(port_name, 0, sizeof(port_name));
  if ((crm_port_attribute_ucm_setmandparams (pMandParams, 
	      port_name, &crm_attribute_name_value_info, 
	      &attribute_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = attribute_result;
    return OF_FAILURE;
  }

  return_value = crm_get_port_handle(port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("port does not exist with name %s",port_name);
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_VM_NAME_NULL);
    *result_p = attribute_result;
    return OF_FAILURE;
  }
  return_value = crm_add_attribute_to_port(port_name, &crm_attribute_name_value_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Attribute modification failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_ATTRIBUTE_ADD_FAILED);
    *result_p = attribute_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Attribute modifed succesfully");
  return OF_SUCCESS;

}



int32_t crm_port_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct   crm_attribute_name_value_pair        crm_attribute_name_value_info={};
  struct   crm_attribute_name_value_output_info crm_attribute_output={};
  char     port_name[CRM_MAX_VM_NAME_LEN+1]={};
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  of_memset (&crm_attribute_name_value_info, 0, sizeof (struct crm_attribute_name_value_pair));
  of_memset(port_name, 0, sizeof(port_name));
  if ((crm_port_attribute_ucm_setmandparams (prev_record_key_p, 
		port_name, &crm_attribute_name_value_info, 
					NULL)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if ((crm_port_attribute_ucm_setmandparams (keys_arr_p, 
		port_name, &crm_attribute_name_value_info, 
					NULL)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }


  return_value = crm_get_port_handle(port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("port does not exist with name %s",port_name);
    return OF_FAILURE;
  }
#if 0
  strcpy(current_name_string,  prev_record_key_p->iv_pairs[uindex_i].value_p);
  strcpy(current_value_string, prev_record_key_p->iv_pairs[uindex_i+1].value_p);
  strcpy(crm_attribute_output.name_string, current_name_string);
#endif
  crm_attribute_output.name_length = CRM_MAX_ATTRIBUTE_NAME_LEN -1;
  crm_attribute_output.value_length = CRM_MAX_ATTRIBUTE_VALUE_LEN -1;
  return_value = crm_get_port_next_attribute(port_name, crm_attribute_name_value_info.name_string,&crm_attribute_output);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm_get_port_next_attribute_name failed!");
    return OF_FAILURE;
  }

 result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
   return OF_FAILURE;
 }
  crm_attribute_port_getparams(&crm_attribute_output, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;	
  return CRM_SUCCESS;  
}

int32_t crm_port_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
		uint32_t * count_p,
		struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct   crm_attribute_name_value_pair crm_attribute_name_value_info={};
  struct   crm_attribute_name_value_output_info crm_attribute_output={};
  char port_name[CRM_MAX_VM_NAME_LEN+1]={};

  uint64_t port_handle;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&crm_attribute_name_value_info, 0, sizeof (struct crm_attribute_name_value_pair));
  of_memset(port_name, 0, sizeof(port_name));
  if ((crm_port_attribute_ucm_setmandparams (keys_arr_p, port_name, &crm_attribute_name_value_info, 
					NULL)) != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     return OF_FAILURE;
  }
  return_value = crm_get_port_handle(port_name, &port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Subnet does not exist with id %s",port_name);
    return OF_FAILURE;
  }

  crm_attribute_output.name_length = CRM_MAX_ATTRIBUTE_NAME_LEN -1;
  crm_attribute_output.value_length = CRM_MAX_ATTRIBUTE_VALUE_LEN -1;

  return_value = crm_get_port_first_attribute(port_name, &crm_attribute_output);
  if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("crm_get_first_port_attribute_name failed!");
     return OF_FAILURE;
   }
   result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  crm_attribute_port_getparams(&crm_attribute_output, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;  
  return CRM_SUCCESS;  
}

int32_t crm_port_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_attribute_name_value_pair         crm_attribute_name_value_info={};
  struct   crm_attribute_name_value_output_info  crm_attribute_name_value_output={};
  char     port_name[CRM_MAX_VM_NAME_LEN+1]={};
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset(port_name, 0, sizeof(port_name));
  result_iv_pairs_p =
                     (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
   {
     CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
     return OF_FAILURE;
   }

  of_memset (&crm_attribute_name_value_info, 0, sizeof (struct crm_attribute_name_value_pair));

  if ((crm_port_attribute_ucm_setmandparams (keys_arr_p,port_name, &crm_attribute_name_value_info, 
	      NULL)) != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     return OF_FAILURE;
   }
   return_value = crm_get_port_handle(port_name, &port_handle);
   if (return_value != OF_SUCCESS)
    {
      CM_CBK_DEBUG_PRINT ("port does not exist with name %s",port_name);
      return OF_FAILURE;
    }

  return_value = crm_get_port_exact_attribute(port_name, crm_attribute_name_value_info.name_string,&crm_attribute_name_value_output);
    if (return_value != OF_SUCCESS)
     {
       CM_CBK_DEBUG_PRINT ("Unable to find  exact attribute record!");
       return OF_FAILURE;
     }

  crm_attribute_port_getparams (&crm_attribute_name_value_output, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return return_value;
}

int32_t crm_attribute_port_getparams (struct crm_attribute_name_value_output_info *attribute_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (2, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_PORTATTR_ATTRNAME_ID,
			CM_DATA_TYPE_STRING, attribute_info->name_string);
  uindex_i++;

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_PORTATTR_ATTRVAL_ID,
			CM_DATA_TYPE_STRING, attribute_info->value_string);
  uindex_i++;

  result_iv_pairs_p->count_ui = uindex_i;
  return OF_SUCCESS;   

}


int32_t crm_port_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
			char *port_name_p,
			struct crm_attribute_name_value_pair *attribute_info,
			struct cm_app_result              **presult_p)
{
   uint32_t uiMandParamCnt;

   CM_CBK_DEBUG_PRINT ("Entered");
   for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
   {
     switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
     {
       case CM_DM_VMSIDEPORTS_PORTNAME_ID:
             of_strncpy (port_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,pMandParams->iv_pairs[uiMandParamCnt].value_length);
	     CM_CBK_DEBUG_PRINT ("port_name %s",port_name_p);
             break;


      case CM_DM_PORTATTR_ATTRNAME_ID:
	    of_strncpy (attribute_info->name_string,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,pMandParams->iv_pairs[uiMandParamCnt].value_length);
            CM_CBK_DEBUG_PRINT ("Attribute name %s",attribute_info->name_string);
	    break;

     case CM_DM_PORTATTR_ATTRVAL_ID:
	   of_strncpy (attribute_info->value_string,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,pMandParams->iv_pairs[uiMandParamCnt].value_length);
	   CM_CBK_DEBUG_PRINT ("Attribute value %s",attribute_info->value_string);
	   break;
     }
   }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}


int32_t crm_port_attribute_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *attribute_result = NULL;
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");

  return_value = crm_port_attribute_ucm_validatemandparams (keys_arr_p, &attribute_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = attribute_result;
  return OF_SUCCESS;
}

int32_t crm_port_attribute_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct cm_app_result **presult_p)
{
  uint32_t uiMandParamCnt;
  struct cm_app_result *attribute_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui; uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_PORTATTR_ATTRNAME_ID:
        if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
	{
	  CM_CBK_DEBUG_PRINT ("Attribute Name is NULL");
	  fill_app_result_struct (&attribute_result, NULL, CM_ATTRIBUTE_NAME_NULL);
	  *presult_p = attribute_result;
	  return OF_FAILURE;
	}
	break;
      case CM_DM_PORTATTR_ATTRVAL_ID: 
	if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
        {
	  CM_CBK_DEBUG_PRINT ("Attribute Value is NULL");
	  fill_app_result_struct (&attribute_result, NULL, CM_ATTRIBUTE_VAL_NULL);
	  *presult_p = attribute_result;
	  return OF_FAILURE;
	}
	break;
      }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}
int32_t crm_port_attribute_delrec (void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
  struct cm_app_result *attribute_result = NULL;
  int32_t return_value = OF_FAILURE; 
  struct crm_attribute_name_value_pair attribute_info={};
  uint64_t port_handle;
  char port_name[CRM_MAX_VM_NAME_LEN+1]={};

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&attribute_info, 0, sizeof (struct crm_attribute_name_value_pair));

  if ((crm_port_attribute_ucm_setmandparams (keys_arr_p, port_name, &attribute_info, &attribute_result)) != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     fill_app_result_struct (&attribute_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
     *result_p = attribute_result;
     return OF_FAILURE;
     
   }

  return_value=crm_get_port_handle(port_name, &port_handle);
  if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Get port handle failed");
     fill_app_result_struct (&attribute_result, NULL, CM_GLU_VM_NAME_NULL);
     *result_p = attribute_result;
     return OF_FAILURE;
   }
  return_value=crm_delete_attribute_from_port(port_name, &attribute_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Delete attribute record failed");
    fill_app_result_struct (&attribute_result, NULL, CM_GLU_ATTRIBUTE_DEL_FAILED);
    *result_p = attribute_result;
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("port attribute deleted succesfully");
  return OF_SUCCESS;
}
#endif
