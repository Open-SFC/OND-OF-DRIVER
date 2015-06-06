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
 * File name: vmsideportscbk.c
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

int32_t crm_vm_sideports_appl_ucmcbk_init (void);
int32_t crm_vm_sideports_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p);
int32_t crm_vm_sideports_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p);
int32_t crm_vm_sideports_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);
int32_t crm_vm_sideports_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p);
int32_t crm_vm_sideports_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);
int32_t crm_vm_sideports_setmandparams (struct cm_array_of_iv_pairs 
    *pMandParams,
    struct crm_port_config_info *port_config_info,
    struct cm_app_result ** presult_p);
int32_t crm_vm_sideports_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **  presult_p);
int32_t crm_vm_sideports_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p);

int32_t crm_vm_sideports_ucm_getparams(char *port_name_p, struct   crm_port_config_info *crm_port_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);

struct cm_dm_app_callbacks crm_vm_sideports_ucm_callbacks =
{
  NULL,
  crm_vm_sideports_addrec,
  NULL,
  crm_vm_sideports_delrec,
  NULL,
  crm_vm_sideports_getfirstnrecs,
  crm_vm_sideports_getnextnrecs,
  crm_vm_sideports_getexactrec,
  crm_vm_sideports_verifycfg,
  NULL
};



int32_t crm_vm_sideports_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_VIRTUALNETWORK_COMPUTENODES_VMSIDEPORTS_APPL_ID, &crm_vm_sideports_ucm_callbacks);
  return OF_SUCCESS;
}


int32_t crm_vm_sideports_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p)
{
  struct cm_app_result *port_result = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_port_config_info port_config_info={};
  uint64_t output_vm_sideports_handle;


  CM_CBK_DEBUG_PRINT ("Entered");

  if((crm_vm_sideports_setmandparams(pMandParams,&port_config_info,&port_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&port_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=port_result;
    return OF_FAILURE;
  }
  return_value =crm_add_crm_vmport(port_config_info.port_name,
      port_config_info.switch_name,
      port_config_info.br_name,
      port_config_info.port_type,
      port_config_info.vn_name,
      port_config_info.vm_name,
      port_config_info.vm_port_mac_p, 
      &output_vm_sideports_handle
      );

  if(return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("failed to add vm ports");
    fill_app_result_struct (&port_result, NULL, CM_GLU_CRM_PORT_ADD_FAILED);
    *result_p=port_result;
    return OF_FAILURE;

  }
   CM_CBK_DEBUG_PRINT ("vm ports added successfully to vn");
  return OF_SUCCESS;
}
int32_t crm_vm_sideports_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct   cm_app_result *crm_vm_sideport_result = NULL;
  struct   crm_port_config_info crm_port_info={};

  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char     port_name [CRM_MAX_PORT_NAME_LEN];

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset(port_name, 0, sizeof(port_name));

  if ((crm_vm_sideports_setmandparams (keys_arr_p, &crm_port_info, &crm_vm_sideport_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  return_value = crm_get_first_crm_vmport(crm_port_info.vn_name, 
        crm_port_info.switch_name,
        crm_port_info.br_name,
        port_name,
        &crm_port_info);
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (return_value !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first crm port failed!");
    return OF_FAILURE;
  }
  crm_vm_sideports_ucm_getparams (port_name, &crm_port_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t crm_vm_sideports_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct   cm_app_result *crm_vm_sideport_result = NULL;
  struct   crm_port_config_info crm_port_info={};

  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char     port_name [CRM_MAX_PORT_NAME_LEN]={};
  char     logical_switch_name[CRM_LOGICAL_SWITCH_NAME_LEN+1]={};

  of_memset(port_name, 0, sizeof(port_name));
  of_memset(logical_switch_name, 0,sizeof(logical_switch_name));

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vm_sideports_setmandparams (keys_arr_p, &crm_port_info, &crm_vm_sideport_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  if ((crm_vm_sideports_setmandparams (prev_record_key_p, &crm_port_info, &crm_vm_sideport_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  return_value = crm_get_next_crm_vmport(crm_port_info.vn_name, 
      crm_port_info.switch_name, 
      crm_port_info.br_name, 
      crm_port_info.port_name, 
      &crm_port_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get next record failed for crm port");
    return OF_FAILURE;
  }
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  crm_vm_sideports_ucm_getparams (port_name, &crm_port_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;

}
int32_t crm_vm_sideports_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct   cm_app_result *crm_vm_sideport_result = NULL;
  struct   crm_port_config_info crm_port_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vm_sideports_setmandparams (keys_arr_p, &crm_port_info, &crm_vm_sideport_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  return_value = crm_get_exact_crm_vmport(crm_port_info.vn_name, 
      crm_port_info.switch_name, 
      crm_port_info.br_name, 
      crm_port_info.port_name, 
      &crm_port_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get exact record failed for crm_port.");
    return OF_FAILURE;
  }
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  crm_vm_sideports_ucm_getparams (crm_port_info.port_name, &crm_port_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;

}
int32_t crm_vm_sideports_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p)

{
  struct   cm_app_result *crm_vm_sideports_result = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_port_config_info port_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vm_sideports_setmandparams (keys_arr_p, &port_config_info, &crm_vm_sideports_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vm_sideports_result;
    return OF_FAILURE;
  }

  return_value =crm_del_crm_vmport(port_config_info.port_name,
      port_config_info.port_type,
      port_config_info.vn_name,
      port_config_info.switch_name,
      port_config_info.vm_name,
      port_config_info.br_name);

  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("vm sideport delete  Failed");
    fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_PORT_DELETE_FAILED);
    *result_p = crm_vm_sideports_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM vm sideport deleted successfully");
  return OF_SUCCESS;
}
int32_t crm_vm_sideports_setmandparams(struct cm_array_of_iv_pairs *pMandParams,
    struct crm_port_config_info           *port_config_info,
    struct cm_app_result                   ** presult_p)
{
  uint32_t uiMandParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");
  char *data;

  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch(pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_VMSIDEPORTS_PORTNAME_ID:
        CM_CBK_DEBUG_PRINT ("vm_sideport_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (port_config_info->port_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);

        break;

      case CM_DM_VMSIDEPORTS_PORTTYPE_ID:
        if(!strcmp((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,"VMSIDE_PORT"))
        port_config_info->port_type=0;
        else if(!strcmp((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,"VMNS_PORT"))
        port_config_info->port_type=1;
        CM_CBK_DEBUG_PRINT ("port_type:%d",port_config_info->port_type);
        break;


      case CM_DM_COMPUTENODES_COMPUTENODENAME_ID:
        CM_CBK_DEBUG_PRINT ("vm_switch_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (port_config_info->switch_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);

        break;

      case CM_DM_VMSIDEPORTS_VMNAME_ID:
        CM_CBK_DEBUG_PRINT ("vm_sideports_vmname_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (port_config_info->vm_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);

        break;

      case CM_DM_VMSIDEPORTS_BRIDGENAME_ID:
        CM_CBK_DEBUG_PRINT ("vm_sideports_bridge_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (port_config_info->br_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);

        break;

      case CM_DM_VIRTUALNETWORK_NAME_ID:
        CM_CBK_DEBUG_PRINT ("vm_sideports_vnname_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (port_config_info->vn_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);

        break;


      case CM_DM_VMSIDEPORTS_VMPORTMACADDR_ID:
        CM_CBK_DEBUG_PRINT ("vm_sideports_mac_address:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        data = (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p;
        if ((*data == '0') &&
            ((*(data+1) == 'x') || (*(data+1) == 'X')))
        {
          of_flow_match_atox_mac_addr(data+2, port_config_info->vm_port_mac_p);
        }
        else
        {
          of_flow_match_atox_mac_addr(data, port_config_info->vm_port_mac_p);
        }

        CM_CBK_DEBUG_PRINT ("vm_sideports_mac_address:%s",port_config_info->vm_port_mac_p);
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;

}

int32_t crm_vm_sideports_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p)


{

  struct cm_app_result *crm_vm_sideports_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct crm_port_config_info port_config_info={};
  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&port_config_info, 0, sizeof (port_config_info));
  return_value = crm_vm_sideports_validatemandparams(key_iv_pairs_p, &crm_vm_sideports_result);
  if (return_value != OF_SUCCESS)
  {
    *result_p =crm_vm_sideports_result;
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  *result_p =crm_vm_sideports_result;
  return OF_SUCCESS;
}

int32_t crm_vm_sideports_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{

  uint32_t count;
  struct cm_app_result *crm_vm_sideports_result = NULL;
  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_VMSIDEPORTS_PORTNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VM sideports  name is NULL");
          fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_PORT_NAME_NULL);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }


        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_PORT_NAME_LEN )
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Network NW Sideport Name");
          fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_PORTNAME_INVALID);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }
        break;


      case CM_DM_VMSIDEPORTS_PORTTYPE_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VM sideports  ID is NULL");
          fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_PORTTYPE_NULL);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }
        CM_CBK_DEBUG_PRINT ("mand_iv_pairs_p->iv_pairs[count].value_p:%s",mand_iv_pairs_p->iv_pairs[count].value_p);
        if((strcmp(mand_iv_pairs_p->iv_pairs[count].value_p,"VMSIDE_PORT")!=0)&&
            (strcmp(mand_iv_pairs_p->iv_pairs[count].value_p,"VMNS_PORT")!=0)) 
        {
          CM_CBK_DEBUG_PRINT("Invalid  port type");
          fill_app_result_struct(&crm_vm_sideports_result, NULL, CM_GLU_PORTTYPE_INVALID);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }


      case CM_DM_VMSIDEPORTS_VMNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VM sideports vm name is NULL");
          fill_app_result_struct (&crm_vm_sideports_result, NULL,CM_GLU_VM_NAME_NULL);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }


        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_VM_NAME_LEN )
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Network NW Sideport Name");
          fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_VM_NAME_INVALID);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }
        break;


      case CM_DM_VMSIDEPORTS_BRIDGENAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VM sideports bridge name is NULL");
          fill_app_result_struct (&crm_vm_sideports_result, NULL,CM_GLU_BRIDGE_NAME_NULL);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }


        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_LOGICAL_SWITCH_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Machine Sideport Name");
          fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_BRIDGENAME_INVALID);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_VIRTUALNETWORK_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VM sideports vn name is NULL");
          fill_app_result_struct (&crm_vm_sideports_result, NULL,CM_GLU_VN_NAME_NULL);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }


        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_VN_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Machine Sideport VN Name");
          fill_app_result_struct (&crm_vm_sideports_result, NULL, CM_GLU_VN_NAME_ID_INVALID);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_VMSIDEPORTS_VMPORTMACADDR_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VM sideports vm mac is NULL");
          fill_app_result_struct (&crm_vm_sideports_result, NULL,CM_GLU_VM_SIDEPORT_MAC_NULL);
          *presult_p = crm_vm_sideports_result;
          return OF_FAILURE;
        }
        break;

    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t crm_vm_sideports_ucm_getparams(char *port_name_p, struct   crm_port_config_info *crm_port_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;
  char buf[128] = "";
  CM_CBK_DEBUG_PRINT ("Entered");

#define CM_VM_PORT_CHILD_COUNT 6

  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (CM_VM_PORT_CHILD_COUNT, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }

  sprintf(buf,"%s",crm_port_info_p->port_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VMSIDEPORTS_PORTNAME_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;


  if(crm_port_info_p->port_type==VM_PORT)
  {
    sprintf(buf,"%s","VMSIDE_PORT");
  }
  else if(crm_port_info_p->port_type==VMNS_PORT)
  {
    sprintf(buf,"%s","VMNS_PORT");
  }
  else
    sprintf(buf,"%s","none");

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VMSIDEPORTS_PORTTYPE_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  of_itoa (crm_port_info_p->portId, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VMSIDEPORTS_PORTID_ID,
  CM_DATA_TYPE_STRING, buf);
  uindex_i++;
          

  sprintf(buf,"%s",crm_port_info_p->vm_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VMSIDEPORTS_VMNAME_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  sprintf(buf,"%s",crm_port_info_p->br_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_VMSIDEPORTS_BRIDGENAME_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",crm_port_info_p->vm_port_mac_p[0],crm_port_info_p->vm_port_mac_p[1],crm_port_info_p->vm_port_mac_p[2],crm_port_info_p->vm_port_mac_p[3],crm_port_info_p->vm_port_mac_p[4],crm_port_info_p->vm_port_mac_p[5]);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_VMSIDEPORTS_VMPORTMACADDR_ID, CM_DATA_TYPE_STRING, buf);
  uindex_i++;


  result_iv_pairs_p->count_ui = uindex_i;
  return OF_SUCCESS;
}
#endif 
