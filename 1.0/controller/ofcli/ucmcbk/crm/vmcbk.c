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
 * File name: vmcbk.c
 * Author: Varun Kumar Reddy <b36461@freescale.com>
 * Date:   15/10/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "crm_vm_api.h"

int32_t crm_vm_appl_ucmcbk_init (void);

int32_t crm_vm_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t crm_vm_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t crm_vm_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t crm_vm_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);

int32_t crm_vm_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t crm_vm_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t crm_vm_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t crm_vm_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t command_id, struct cm_app_result ** result_p);







/************************ *********************************************/
int32_t crm_vm_ucm_validatemandparams (struct cm_array_of_iv_pairs *
                        pMandParams,
                        struct cm_app_result **
                        presult_p);

int32_t crm_vm_ucm_validateoptparams (struct cm_array_of_iv_pairs *
                        pOptParams,
                        struct cm_app_result ** presult_p);


int32_t crm_vm_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct crm_vm_config_info *crm_vm_config_info_p,
                        struct cm_app_result ** presult_p);

int32_t crm_vm_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
			struct crm_vm_config_info *crm_vm_config_info_p,
                        struct cm_app_result ** presult_p);


int32_t crm_vm_ucm_getparams (struct crm_vm_config_info *crm_vm_config_info_p,
                        struct cm_array_of_iv_pairs * result_iv_pairs_p);


struct cm_dm_app_callbacks crm_vm_ucm_callbacks = 
{
  NULL,
  crm_vm_addrec,
  crm_vm_setrec,
  crm_vm_delrec,
  NULL,
  crm_vm_getfirstnrecs,
  crm_vm_getnextnrecs,
  crm_vm_getexactrec,
  crm_vm_verifycfg,
  NULL
};


int32_t crm_vm_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  int32_t ret_val;
  ret_val = cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_VIRTUALMACHINE_APPL_ID, &crm_vm_ucm_callbacks);
  if(ret_val != OF_SUCCESS)
     return OF_FAILURE;
  CM_CBK_DEBUG_PRINT ("ret:%d",ret_val);
  return OF_SUCCESS;
}



int32_t crm_vm_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *crm_vm_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct crm_vm_config_info vm_config_info={};
   uint64_t crm_vm_handle;

   CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vm_ucm_setmandparams (pMandParams, &vm_config_info, &crm_vm_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vm_result;
    return OF_FAILURE;
  }

  if ((crm_vm_ucm_setoptparams (pOptParams, &vm_config_info, &crm_vm_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=crm_vm_result;
    return OF_FAILURE;
  }

 
  return_value =crm_add_virtual_machine(&vm_config_info,  &crm_vm_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vm add  Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_VM_ADD_FAILED);
    *result_p = crm_vm_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM vm added successfully");
  return OF_SUCCESS;  
}
int32_t crm_vm_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *crm_vm_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct crm_vm_config_info vm_config_info={};
   uint64_t crm_vm_handle;

   CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vm_ucm_setmandparams (pMandParams, &vm_config_info, &crm_vm_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vm_result;
    return OF_FAILURE;
  }

  if ((crm_vm_ucm_setoptparams (pOptParams, &vm_config_info, &crm_vm_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=crm_vm_result;
    return OF_FAILURE;
  }

 
  return_value =crm_modify_virtual_machine(&vm_config_info,  &crm_vm_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vm add  Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_VM_ADD_FAILED);
    *result_p = crm_vm_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM vm added successfully");
  return OF_SUCCESS;  
}



int32_t crm_vm_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{
  struct cm_app_result *crm_vm_result = NULL;
  int32_t return_value = OF_FAILURE;

  struct crm_vm_config_info  vm_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vm_ucm_setmandparams (keys_arr_p, &vm_info, &crm_vm_result)) !=
           OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = crm_vm_result;
    return OF_FAILURE;
  }
  return_value = crm_del_virtual_machine(vm_info.vm_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vm add  Failed");
    fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_VM_DEL_FAILED);
    *result_p = crm_vm_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM vm delete successfully");
  return OF_SUCCESS;  

}
int32_t crm_vm_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  struct  crm_vm_config_info crm_vm_config_info={};
  struct  vm_runtime_info  runtime_info={};
  uint64_t vm_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
 return_value = crm_get_first_vm(&crm_vm_config_info, &runtime_info, &vm_handle);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for CRM vm Table");
   return OF_FAILURE;
 }

 result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
 crm_vm_ucm_getparams (&crm_vm_config_info, &result_iv_pairs_p[uiRecCount]);
 uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
 *count_p = uiRecCount;
 return OF_SUCCESS;
}


int32_t crm_vm_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *vm_result = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  struct  crm_vm_config_info vm_config_info={};
  struct  vm_runtime_info  runtime_info={};
  uint64_t vm_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

 if ((crm_vm_ucm_setmandparams (prev_record_key_p, &vm_config_info, &vm_result)) !=
                                OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
  }

  return_value = crm_get_vm_handle(vm_config_info.vm_name, &vm_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: vm doesn't exists with name %s",vm_config_info.vm_name);
    return OF_FAILURE;
  }

  of_memset (&vm_config_info, 0, sizeof(vm_config_info));


  return_value = crm_get_next_vm(&vm_config_info, &runtime_info, &vm_handle);

  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("failed");
    *count_p = 0;
    return OF_FAILURE;
  }
CM_CBK_DEBUG_PRINT ("name:%s",vm_config_info.vm_name);
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     return OF_FAILURE;
  }
  crm_vm_ucm_getparams (&vm_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}
 



int32_t crm_vm_ucm_getparams (struct crm_vm_config_info *vm_info,
                        struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
         uint32_t uindex_i = 0;
         char buf[128] = "";
         CM_CBK_DEBUG_PRINT ("Entered");
#define CM_VM_CHILD_COUNT 5

         result_iv_pairs_p->iv_pairs =
                        (struct cm_iv_pair *) of_calloc (CM_VM_CHILD_COUNT, sizeof (struct cm_iv_pair));
         if (result_iv_pairs_p->iv_pairs == NULL)
         {
                        CM_CBK_DEBUG_PRINT
                                 ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
                        return OF_FAILURE;
         }

         sprintf(buf,"%s",vm_info->vm_name);
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALMACHINE_NAME_ID,
                                 CM_DATA_TYPE_STRING, buf);
         uindex_i++;


	 if(vm_info->vm_type == VM_TYPE_NORMAL_APPLICATION)
	 {
           sprintf(buf,"%s","VM_TYPE_NORMAL_APPLICATION");
         }
	 else if(vm_info->vm_type == VM_TYPE_NETWORK_SERVICE)
	 {  
           sprintf(buf,"%s","VM_TYPE_NETWORK_SERVICE");
         }
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALMACHINE_TYPE_ID,
                                 CM_DATA_TYPE_STRING, buf);
         uindex_i++;

         sprintf(buf,"%s",vm_info->tenant_name);
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALMACHINE_TENANT_ID,
                                 CM_DATA_TYPE_STRING, buf);
         uindex_i++;

         sprintf(buf,"%s",vm_info->switch_name);
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALMACHINE_SWITCH_ID,
                                 CM_DATA_TYPE_STRING, buf);
         uindex_i++;

         sprintf(buf,"%s",vm_info->vm_desc);
         FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALMACHINE_DESCRIPTION_ID,
                                 CM_DATA_TYPE_STRING, buf);
         uindex_i++;

        result_iv_pairs_p->count_ui = uindex_i;
        CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
        return OF_SUCCESS;
}




int32_t crm_vm_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct crm_vm_config_info* vm_info,               struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui; uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_VIRTUALMACHINE_NAME_ID:
        CM_CBK_DEBUG_PRINT ("vm_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (vm_info->vm_name, 
		   (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	           pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;

      case CM_DM_VIRTUALMACHINE_TYPE_ID:
        CM_CBK_DEBUG_PRINT ("vm_type:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
	if(!strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,"VM_TYPE_NORMAL_APPLICATION"))
        {
	  vm_info->vm_type = VM_TYPE_NORMAL_APPLICATION;
        }
        else if(!strcmp((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,"VM_TYPE_NETWORK_SERVICE"))
	{
	  vm_info->vm_type = VM_TYPE_NETWORK_SERVICE;
        }
	break;


      case CM_DM_VIRTUALMACHINE_TENANT_ID:
        CM_CBK_DEBUG_PRINT ("vm_tenant_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (vm_info->tenant_name, 
		   (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	           pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;

      case CM_DM_VIRTUALMACHINE_SWITCH_ID:
        CM_CBK_DEBUG_PRINT ("vm_switch_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (vm_info->switch_name, 
		   (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	           pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;

      default:
	break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t crm_vm_ucm_setoptparams(struct cm_array_of_iv_pairs *opt_iv_pairs_p,
    struct crm_vm_config_info *crm_vm_config_info_p, struct cm_app_result ** result_pp)
{
  uint32_t param_count;
  CM_CBK_DEBUG_PRINT ("Entered");

  CM_CBK_DEBUG_PRINT("opt_iv_pairs_p->count_ui:%d\n",opt_iv_pairs_p->count_ui);

  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_VIRTUALMACHINE_DESCRIPTION_ID:
        of_strncpy(crm_vm_config_info_p->vm_desc, (char *) opt_iv_pairs_p->iv_pairs[param_count].value_p,
            opt_iv_pairs_p->iv_pairs[param_count].value_length);
        CM_CBK_DEBUG_PRINT("vm_desc:%s\n",crm_vm_config_info_p->vm_desc);
        break;

      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;

}

int32_t crm_vm_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_vm_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct crm_vm_config_info   vm_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&vm_config_info, 0, sizeof (vm_config_info));

  return_value = crm_vm_ucm_validatemandparams(key_iv_pairs_p, &of_vm_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_vm_result;
  return OF_SUCCESS;

}


int32_t crm_vm_ucm_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_vm_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_VIRTUALMACHINE_NAME_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("vm name is NULL");
          fill_app_result_struct (&of_vm_result, NULL, CM_GLU_VM_NAME_NULL);
          *presult_p = of_vm_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_VIRTUALMACHINE_TYPE_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("vm type is NULL");
          fill_app_result_struct (&of_vm_result, NULL, CM_GLU_VM_TYPE_NULL);
          *presult_p = of_vm_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_VIRTUALMACHINE_TENANT_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("vm tenant is NULL");
          fill_app_result_struct (&of_vm_result, NULL, CM_GLU_TENANT_NAME_NULL);
          *presult_p = of_vm_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_VIRTUALMACHINE_SWITCH_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("vm switch is NULL");
          fill_app_result_struct (&of_vm_result, NULL, CM_GLU_SWITCH_NULL);
          *presult_p = of_vm_result;
          return OF_FAILURE;
        }
        break; 
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}



int32_t crm_vm_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *crm_vm_result = NULL;
  struct crm_vm_config_info   vm_config_info={};
  struct  vm_runtime_info   runtime_info={};
  int32_t iRes = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&vm_config_info, 0, sizeof(vm_config_info));

  if ((crm_vm_ucm_setmandparams(key_iv_pairs_p, &vm_config_info, &crm_vm_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    //fill_app_result_struct (&crm_vm_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    //*result_p = crm_vm_result;
    return OF_FAILURE;
  }

  iRes = crm_get_vm_by_name(vm_config_info.vm_name, &vm_config_info,&runtime_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: vm doesn't exists with name %s",vm_config_info.vm_name);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("Exact matching record found");
  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));

  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
    return OF_FAILURE;
  }
  crm_vm_ucm_getparams(&vm_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}
#endif

