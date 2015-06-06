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
 * File name: vncbk.c
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
#include "crm_vn_api.h"


int32_t crm_vn_appl_ucmcbk_init (void);

int32_t crm_vn_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);


int32_t crm_vn_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t crm_vn_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t crm_vn_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);

int32_t crm_vn_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t crm_vn_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t crm_vn_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t crm_vn_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t command_id, struct cm_app_result ** result_p);







/************************ *********************************************/
int32_t crm_vn_ucm_validatemandparams (struct cm_array_of_iv_pairs *
                        pMandParams, int32_t *nw_type_p,
                        struct cm_app_result **
                        presult_p);

int32_t crm_vn_ucm_validateoptparams (struct cm_array_of_iv_pairs *
                        pOptParams,int32_t *nw_type_p,
                        struct cm_app_result ** presult_p);


int32_t crm_vn_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        uint32_t *vn_seg_id, uint32_t *vn_service_port,
                        struct crm_vn_config_info* vn_info, 
	                struct cm_app_result ** presult_p);

int32_t crm_vn_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
			struct crm_vn_config_info* vn_info,
                        struct cm_app_result ** presult_p);


int32_t crm_vn_ucm_getparams (struct crm_vn_config_info *crm_vn_config_info_p,
                        struct cm_array_of_iv_pairs * result_iv_pairs_p);


struct cm_dm_app_callbacks crm_vn_ucm_callbacks = 
{
  NULL,
  crm_vn_addrec,
  crm_vn_modrec,
  crm_vn_delrec,
  NULL,
  crm_vn_getfirstnrecs,
  crm_vn_getnextnrecs,
  crm_vn_getexactrec,
  crm_vn_verifycfg,
  NULL
};


int32_t crm_vn_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_VIRTUALNETWORK_APPL_ID, &crm_vn_ucm_callbacks);  
  return OF_SUCCESS;
}



int32_t crm_vn_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *crm_vn_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct crm_vn_config_info vn_config_info={};
   uint64_t crm_vn_handle;
   uint32_t vn_seg_id, vn_service_port;

   CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_ucm_setmandparams (pMandParams, &vn_seg_id,&vn_service_port,
				&vn_config_info, 
				&crm_vn_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vn_result;
    return OF_FAILURE;
  }

  if ((crm_vn_ucm_setoptparams (pOptParams, &vn_config_info, &crm_vn_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=crm_vn_result;
    return OF_FAILURE;
  }

  if(vn_config_info.nw_type == VLAN_TYPE)
  {
    vn_config_info.nw_params.vlan_nw.vlan_id = vn_seg_id;
  }
  else if(vn_config_info.nw_type == VXLAN_TYPE)
  {
    vn_config_info.nw_params.vxlan_nw.nid = vn_seg_id;
    vn_config_info.nw_params.vxlan_nw.service_port= vn_service_port;
  }

 
  return_value =crm_add_virtual_network(&vn_config_info,  &crm_vn_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn add  Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_VN_ADD_FAILED);
    *result_p = crm_vn_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM VN added successfully");
  return OF_SUCCESS;  
}

int32_t crm_vn_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *crm_vn_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct crm_vn_config_info vn_config_info={};
   uint32_t field_flags = 0;
   uint32_t vn_seg_id, vn_service_port;

   CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_ucm_setmandparams (pMandParams, 
                                  &vn_seg_id, &vn_service_port,
				&vn_config_info, 
				&crm_vn_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vn_result;
    return OF_FAILURE;
  }

  if ((crm_vn_ucm_setoptparams (pOptParams, &vn_config_info, &crm_vn_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=crm_vn_result;
    return OF_FAILURE;
  }

  if(vn_config_info.nw_type == VLAN_TYPE)
  {
    vn_config_info.nw_params.vlan_nw.vlan_id = vn_seg_id;
  }
  else if(vn_config_info.nw_type == VXLAN_TYPE)
  {
    vn_config_info.nw_params.vxlan_nw.nid = vn_seg_id;
    vn_config_info.nw_params.vxlan_nw.service_port= vn_service_port;
  }

 
  return_value = crm_modify_virtual_network(&vn_config_info,field_flags);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn mod  Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_VN_MOD_FAILED);
    *result_p = crm_vn_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("Virtual Network  Modified successfully");
  return OF_SUCCESS;  
}



int32_t crm_vn_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{
  struct   cm_app_result *crm_vn_result = NULL;
  int32_t  return_value = OF_FAILURE;
  uint32_t vn_seg_id, vn_service_port;
  struct   crm_vn_config_info  vn_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vn_ucm_setmandparams (keys_arr_p, &vn_seg_id,&vn_service_port,
                                &vn_info, 
                                &crm_vn_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = crm_vn_result;
    return OF_FAILURE;
  }
  return_value = crm_del_virtual_network(vn_info.nw_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn add  Failed");
    fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_VN_DEL_FAILED);
    *result_p = crm_vn_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM vn delete successfully");
  return OF_SUCCESS;  

}
int32_t crm_vn_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  struct  crm_vn_config_info vn_config_info={};
  struct  vn_runtime_info  runtime_info={};
  uint64_t vn_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
 return_value = crm_get_first_virtual_network(&vn_config_info, &runtime_info, &vn_handle);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for CRM vn Table");
   return OF_FAILURE;
 }

 CM_CBK_DEBUG_PRINT ("name:%s",vn_config_info.nw_name);
 result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
 crm_vn_ucm_getparams (&vn_config_info, &result_iv_pairs_p[uiRecCount]);
 uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
 *count_p = uiRecCount;
 return OF_SUCCESS;
}


int32_t crm_vn_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *vn_result = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;

  struct  crm_vn_config_info vn_config_info={};
  struct  vn_runtime_info  runtime_info={};
  uint64_t vn_handle;
  uint32_t vn_seg_id, vn_service_port;

  CM_CBK_DEBUG_PRINT ("Entered");

 if ((crm_vn_ucm_setmandparams (prev_record_key_p, &vn_seg_id, &vn_service_port,&vn_config_info, &vn_result)) !=
                                OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
  }

  return_value = crm_get_vn_handle(vn_config_info.nw_name, &vn_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: vn doesn't exists with name %s",vn_config_info.nw_name);
    return OF_FAILURE;
  }

  of_memset (&vn_config_info, 0, sizeof(vn_config_info));


  return_value = crm_get_next_virtual_network(&vn_config_info, &runtime_info, &vn_handle);

  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("failed");
    *count_p = 0;
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     return OF_FAILURE;
  }
  crm_vn_ucm_getparams (&vn_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}
 



int32_t crm_vn_ucm_getparams (struct crm_vn_config_info *vn_info,
                        struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;
  char buf[128] = "";
  CM_CBK_DEBUG_PRINT ("Entered");

  #define CM_VN_CHILD_COUNT 11

  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (CM_VN_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }
  sprintf(buf,"%s",vn_info->nw_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALNETWORK_NAME_ID,
                               CM_DATA_TYPE_STRING, buf);
  uindex_i++;


  if(vn_info->nw_type == VLAN_TYPE)
  {
    sprintf(buf,"%s","VLAN_TYPE");
  }
  else if(vn_info->nw_type == VXLAN_TYPE)
  {  
    sprintf(buf,"%s","VXLAN_TYPE");
  }
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIRTUALNETWORK_TYPE_ID,
                                 CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  sprintf(buf,"%s",vn_info->tenant_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIRTUALNETWORK_TENANT_ID,
                                 CM_DATA_TYPE_STRING, buf);
  uindex_i++;



  if(vn_info->nw_type == VLAN_TYPE)
  {
    sprintf(buf , "%u", vn_info->nw_params.vlan_nw.vlan_id);
    FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIRTUALNETWORK_SEGMENTATION_ID_ID,
                                 CM_DATA_TYPE_STRING, buf);
    uindex_i++;
  }
  else if(vn_info->nw_type == VXLAN_TYPE)
  {
    sprintf(buf , "%u", vn_info->nw_params.vxlan_nw.nid);
    FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIRTUALNETWORK_SEGMENTATION_ID_ID,
                                 CM_DATA_TYPE_STRING, buf);
    uindex_i++;

    sprintf(buf , "%d", vn_info->nw_params.vxlan_nw.service_port);
    FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIRTUALNETWORK_VXLAN_SERVICE_PORT_ID,
                                 CM_DATA_TYPE_STRING, buf);
    uindex_i++;

  }


  sprintf(buf ,"%s" ,vn_info->nw_desc);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALNETWORK_DESCRIPTION_ID,
                   CM_DATA_TYPE_STRING, buf);
  uindex_i++;



  sprintf(buf, "%u" , vn_info->vn_operational_status_b);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VIRTUALNETWORK_STATUS_ID,
                   CM_DATA_TYPE_BOOLEAN, buf);

  uindex_i++;

  sprintf(buf ,"%s" ,vn_info->provider_physical_network);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALNETWORK_PROVIDER_PHYSICAL_NTWK_ID,
                         CM_DATA_TYPE_STRING, buf);
  uindex_i++;


  sprintf(buf , "%u", vn_info->vn_admin_state_up_b);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALNETWORK_STATE_ID,
                   CM_DATA_TYPE_BOOLEAN, buf);

   uindex_i++;
   

  sprintf(buf , "%u", vn_info->router_external_b);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VIRTUALNETWORK_EXTERNAL_ID,
                   CM_DATA_TYPE_BOOLEAN, buf);

  uindex_i++;
          


  result_iv_pairs_p->count_ui = uindex_i;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}




int32_t crm_vn_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,uint32_t *vn_seg_id, uint32_t *vn_service_port,
                        struct crm_vn_config_info* vn_info, 
	                struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui; uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_VIRTUALNETWORK_NAME_ID:
        CM_CBK_DEBUG_PRINT ("vn_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (vn_info->nw_name, 
		   (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	           pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;

      case CM_DM_VIRTUALNETWORK_TENANT_ID:
        CM_CBK_DEBUG_PRINT ("vn_tenant:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (vn_info->tenant_name, 
		   (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
	           pMandParams->iv_pairs[uiMandParamCnt].value_length);        
        break;


      case CM_DM_VIRTUALNETWORK_TYPE_ID:
        CM_CBK_DEBUG_PRINT ("vn_nw_type:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
	if(!strcmp(((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p), "VLAN_TYPE"))
	{
	  vn_info->nw_type = VLAN_TYPE;	
	}
	else if(!strcmp(((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p), "VXLAN_TYPE"))
        {
	  vn_info->nw_type = VXLAN_TYPE;  
        }
        break;

    case CM_DM_VIRTUALNETWORK_SEGMENTATION_ID_ID:
        CM_CBK_DEBUG_PRINT ("vn_seg_id:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        *vn_seg_id = of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;

     case CM_DM_VIRTUALNETWORK_VXLAN_SERVICE_PORT_ID:
        CM_CBK_DEBUG_PRINT ("vn_service_port:%s",(char *)pMandParams->iv_pairs[uiMandParamCnt].value_p);
        *vn_service_port = of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;     


        default:
	break;

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}

int32_t crm_vn_ucm_setoptparams(struct cm_array_of_iv_pairs *opt_iv_pairs_p, 
    struct crm_vn_config_info *crm_vn_config_info_p, struct cm_app_result ** result_pp)
{
  uint32_t param_count;
  CM_CBK_DEBUG_PRINT ("Entered");


  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {


      case CM_DM_VIRTUALNETWORK_DESCRIPTION_ID:
        of_strncpy(crm_vn_config_info_p->nw_desc, (char *) opt_iv_pairs_p->iv_pairs[param_count].value_p,
            opt_iv_pairs_p->iv_pairs[param_count].value_length);
        CM_CBK_DEBUG_PRINT("nw_desc:%s\n",crm_vn_config_info_p->nw_desc);
        break;

      case CM_DM_VIRTUALNETWORK_STATUS_ID:
        crm_vn_config_info_p->vn_operational_status_b = of_atoi((char *) opt_iv_pairs_p->iv_pairs[param_count].value_p);
        CM_CBK_DEBUG_PRINT("vn_operational_status_b:%d\n",crm_vn_config_info_p->vn_operational_status_b);
        break;

      case CM_DM_VIRTUALNETWORK_STATE_ID:
        crm_vn_config_info_p->vn_admin_state_up_b = of_atoi((char *) opt_iv_pairs_p->iv_pairs[param_count].value_p);
        CM_CBK_DEBUG_PRINT("vn_admin_state_up_b:%d\n",crm_vn_config_info_p->vn_admin_state_up_b);
        break;

      case CM_DM_VIRTUALNETWORK_EXTERNAL_ID:
        crm_vn_config_info_p->router_external_b = of_atoi((char *) opt_iv_pairs_p->iv_pairs[param_count].value_p);
        CM_CBK_DEBUG_PRINT("router_external_b:%d\n",crm_vn_config_info_p->router_external_b);
        break;	

      case CM_DM_VIRTUALNETWORK_PROVIDER_PHYSICAL_NTWK_ID:
        of_strncpy(crm_vn_config_info_p->provider_physical_network, 
                   (char *) opt_iv_pairs_p->iv_pairs[param_count].value_p,
                   opt_iv_pairs_p->iv_pairs[param_count].value_length);
        CM_CBK_DEBUG_PRINT("provider_physical_network:%s\n",
                           crm_vn_config_info_p->provider_physical_network);
        break;

      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;

}

int32_t crm_vn_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_vn_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct crm_vn_config_info   vn_config_info={};
  int32_t nw_type;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&vn_config_info, 0, sizeof (vn_config_info));

  return_value = crm_vn_ucm_validatemandparams(key_iv_pairs_p, &nw_type, &of_vn_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    *result_p = of_vn_result;
    return OF_FAILURE;
  }
#if 0
  return_value = crm_vn_ucm_validateoptparams(key_iv_pairs_p,&nw_type, &of_vn_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    *result_p = of_vn_result;
    return OF_FAILURE;
  }
#endif
  return OF_SUCCESS;

}


int32_t crm_vn_ucm_validatemandparams(struct cm_array_of_iv_pairs * mand_iv_pairs_p, int32_t *nw_type_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_vn_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_VIRTUALNETWORK_NAME_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Virtual Network  name is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_NAME_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_VN_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Network Name");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_NAME_ID_INVALID);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_VIRTUALNETWORK_TYPE_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Virtual Network  Type is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_TYPE_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
        
	if(!(strcmp(mand_iv_pairs_p->iv_pairs[count].value_p, "VLAN_TYPE")))
	{
	  *nw_type_p = VLAN_TYPE;
	}
	else if(!(strcmp(mand_iv_pairs_p->iv_pairs[count].value_p, "VXLAN_TYPE")))
	{
	  *nw_type_p = VXLAN_TYPE;
	}
	else
	{ 
	  CM_CBK_DEBUG_PRINT ("Invalid Virtual Network Type");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_NAME_ID_INVALID);
          *presult_p = of_vn_result;
          return OF_FAILURE;
	}
        break;


      case CM_DM_VIRTUALNETWORK_TENANT_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Tenant Name is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_TENANT_NAME_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_TENANT_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid Tenant Name");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_TENANT_ID_INVALID);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        } 
        break;

      case CM_DM_VIRTUALNETWORK_SEGMENTATION_ID_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Virtual Network segmentation id is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_SEG_ID_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
	break;

      case CM_DM_VIRTUALNETWORK_VXLAN_SERVICE_PORT_ID:
	if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Virtual Network segmentation id is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_VXLAN_SERV_PORT_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
       break;
 
   }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}


int32_t crm_vn_ucm_validateoptparams (struct cm_array_of_iv_pairs *pOptParams,
			int32_t *nw_type_p,
                        struct cm_app_result ** presult_p)
{
  uint32_t uiOptParamCnt;
  struct cm_app_result *of_vn_result = NULL;
  int32_t seg_id_set=FALSE,service_port_set=FALSE;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
      case CM_DM_VIRTUALNETWORK_SEGMENTATION_ID_ID:
	if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Virtual Network segmentation id is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_SEG_ID_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
	seg_id_set=TRUE;
       break;

      case CM_DM_VIRTUALNETWORK_VXLAN_SERVICE_PORT_ID:
	if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Virtual Network segmentation id is NULL");
          fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_VXLAN_SERV_PORT_NULL);
          *presult_p = of_vn_result;
          return OF_FAILURE;
        }
	service_port_set=TRUE;
       break;
 
      default:
        break;
    }
  }
  if(*nw_type_p == VLAN_TYPE)
  {
    if(seg_id_set==FALSE)
    {
      CM_CBK_DEBUG_PRINT ("Network type selected VLAN TYPE .Enter segmentation id.");
      fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_VLAN_PARAMS_NOT_SET);
      *presult_p = of_vn_result;
      return OF_FAILURE; 
    }
  }
  else if(*nw_type_p == VXLAN_TYPE)
  {
    if((seg_id_set==FALSE) ||(service_port_set==FALSE))
    {
      CM_CBK_DEBUG_PRINT ("Network type selected VXLAN TYPE .Enter segmentation id and vxlan service port fields.");
      fill_app_result_struct (&of_vn_result, NULL, CM_GLU_VN_VXLAN_PARAMS_NOT_SET);
      *presult_p = of_vn_result;
      return OF_FAILURE; 
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}



int32_t crm_vn_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{

  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *crm_vn_result = NULL;
  struct crm_vn_config_info   vn_config_info={};
  struct  vn_runtime_info   runtime_info={};
  int32_t iRes = OF_FAILURE;
  uint32_t vn_seg_id, vn_service_port;

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&vn_config_info, 0, sizeof(vn_config_info));

  if ((crm_vn_ucm_setmandparams(key_iv_pairs_p,&vn_seg_id,&vn_service_port, &vn_config_info, &crm_vn_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    //fill_app_result_struct (&crm_vn_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    //*result_p = crm_vn_result;
    return OF_FAILURE;
  }

  iRes = crm_get_vn_by_name(vn_config_info.nw_name, &vn_config_info,&runtime_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: vn doesn't exists with name %s",vn_config_info.nw_name);
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
  crm_vn_ucm_getparams(&vn_config_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}
#endif

