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
 * File name: nwsideportscbk.c
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
#include "crm_port_api.h"

int32_t crm_vn_nw_sideports_appl_ucmcbk_init (void);
int32_t crm_vn_nw_sideports_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p);
int32_t crm_vn_nw_sideports_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p);


int32_t crm_vn_nw_sideports_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t command_id, struct cm_app_result ** result_p);


int32_t crm_vn_nw_sideports_setmandparams(struct cm_array_of_iv_pairs
    *pMandParams,
    struct crm_nwport_config_info* nwport_config_info,
    struct cm_app_result ** presult_p);

int32_t crm_vn_nw_sideports_setoptparams(struct cm_array_of_iv_pairs *pOptParams,
    struct crm_nwport_config_info* nwport_config_info,
    struct cm_app_result ** presult_p);


int32_t crm_vn_nw_sideports_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,uint8_t *nw_type_p,
    struct cm_app_result **  presult_p);

int32_t crm_vn_nw_sideports_validateoptparams(struct cm_array_of_iv_pairs *opt_iv_pairs_p,
    uint8_t *nw_type_p,
    struct cm_app_result **  presult_p);


int32_t crm_vn_nw_sideports_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t crm_vn_nw_sideports_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t crm_vn_nw_sideports_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t crm_nw_sideports_ucm_getparams(struct   crm_nwport_config_info *crm_port_info_p,
        struct cm_array_of_iv_pairs * result_iv_pairs_p);


struct cm_dm_app_callbacks crm_vn_nw_sideports_ucm_callbacks =
{
  NULL,
  crm_vn_nw_sideports_addrec,
  NULL,
  crm_vn_nw_sideports_delrec,
  NULL,
  crm_vn_nw_sideports_getfirstnrecs,
  crm_vn_nw_sideports_getnextnrecs,
  crm_vn_nw_sideports_getexactrec,
  crm_vn_nw_sideports_verifycfg,
  NULL
};


int32_t crm_vn_nw_sideports_appl_ucmcbk_init (void)
{

  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_NWSIDEPORTS_APPL_ID, &crm_vn_nw_sideports_ucm_callbacks);
  return OF_SUCCESS;
}


int32_t crm_vn_nw_sideports_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * pMandParams,
    struct cm_array_of_iv_pairs * pOptParams,
    struct cm_app_result ** result_p)
{
  struct   cm_app_result *crm_vn_nw_sideports_result = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_nwport_config_info nw_port_info={};
  uint64_t  output_crm_port_handle;


  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_nw_sideports_setmandparams (pMandParams,&nw_port_info,&crm_vn_nw_sideports_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vn_nw_sideports_result;
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT("switch_name : %s",nw_port_info.switch_name);
  if ((crm_vn_nw_sideports_setoptparams (pOptParams,&nw_port_info,&crm_vn_nw_sideports_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional  Parameters Failed");
    fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=crm_vn_nw_sideports_result;
    return OF_FAILURE;
  }
  return_value =crm_add_crm_nwport(&nw_port_info, &output_crm_port_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn nw sideports attach  Failed");
    fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_CRM_PORT_ADD_FAILED);
    *result_p = crm_vn_nw_sideports_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM VN NW sideport attached successfully");
  return OF_SUCCESS;
}

int32_t crm_vn_nw_sideports_delrec (void * config_transaction_p,
    struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_app_result ** result_p)

{
  struct   cm_app_result *crm_nw_sideports_result = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_nwport_config_info port_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vn_nw_sideports_setmandparams(keys_arr_p, &port_config_info, &crm_nw_sideports_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_nw_sideports_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_nw_sideports_result;
    return OF_FAILURE;
  }

  return_value = crm_del_crm_nwport(port_config_info.port_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("network  sideport delete  Failed");
    fill_app_result_struct (&crm_nw_sideports_result, NULL, CM_GLU_PORT_DELETE_FAILED);
    *result_p = crm_nw_sideports_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM network sideport deleted successfully");
  return OF_SUCCESS;
}
int32_t crm_vn_nw_sideports_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct   crm_nwport_config_info crm_port_info={};

  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char     port_name [CRM_MAX_PORT_NAME_LEN]={};

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset(port_name, 0, sizeof(port_name));
  return_value = crm_get_first_nw_crm_port(port_name, &crm_port_info);
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (return_value !=OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first crm port failed!");
    return OF_FAILURE;
  }
  crm_nw_sideports_ucm_getparams (&crm_port_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t crm_vn_nw_sideports_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct   cm_app_result *crm_vm_sideport_result = NULL;
  struct   crm_nwport_config_info crm_port_info={};

  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char     port_name [CRM_MAX_PORT_NAME_LEN]={};

  of_memset(port_name, 0, sizeof(port_name));

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_nw_sideports_setmandparams (prev_record_key_p, &crm_port_info, &crm_vm_sideport_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value = crm_get_next_nw_crm_port(crm_port_info.port_name,
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
  crm_nw_sideports_ucm_getparams (&crm_port_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t crm_vn_nw_sideports_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  ret_val = OF_FAILURE;
  struct   cm_app_result *crm_vm_sideport_result = NULL;
  struct   crm_nwport_config_info crm_port_info={};
  uint64_t port_handle;

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vn_nw_sideports_setmandparams (keys_arr_p, &crm_port_info, &crm_vm_sideport_result)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  ret_val = crm_get_port_handle(crm_port_info.port_name , &port_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "port name invalid!.");
    return CRM_FAILURE;
  }
  of_memset(&crm_port_info, 0 , sizeof(crm_port_info));
  ret_val = crm_get_exact_nwport(port_handle, &crm_port_info);
  if (ret_val != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Unable to find the matching record ");
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
  crm_nw_sideports_ucm_getparams(&crm_port_info, result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}

int32_t crm_vn_nw_sideports_verifycfg (struct cm_array_of_iv_pairs *keys_arr_p,
    uint32_t command_id, struct cm_app_result ** result_p)
{

  struct cm_app_result *crm_vn_nw_sideports_result = NULL;
  int32_t return_value = OF_FAILURE;
  uint8_t nw_type=-1;
  struct   crm_nwport_config_info nw_port_info={};
  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&nw_port_info, 0, sizeof (nw_port_info)); 

  return_value = crm_vn_nw_sideports_validatemandparams(keys_arr_p, &nw_type, &crm_vn_nw_sideports_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    *result_p =crm_vn_nw_sideports_result;
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("nw_type:%d",nw_type);
  return_value = crm_vn_nw_sideports_validateoptparams(keys_arr_p, &nw_type,&crm_vn_nw_sideports_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate optional Parameters Failed");
    *result_p =crm_vn_nw_sideports_result;
    return OF_FAILURE;
  }
  return OF_SUCCESS;

}

int32_t crm_vn_nw_sideports_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p, uint8_t *nw_type_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *crm_vn_nw_sideports_result = NULL;
  uint8_t vxlan_vni_id_set=FALSE,remote_ip_set=FALSE,vxlan_serviceport_set=FALSE;
  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_NWSIDEPORTS_PORTNAME_ID:

        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VN NW sideports  name is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_PORT_NAME_NULL);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_PORT_NAME_LEN )
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Network NW Sideport Name");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_PORT_NAME_INVALID);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_NWSIDEPORTS_PORTTYPE_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VN NW sideports  ID is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_PORT_ID_NULL);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        break;
      case CM_DM_NWSIDEPORTS_NWTYPE_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VN NW sideports nwtype ID is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_NWID_NULL);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        if(!strcmp((char *)mand_iv_pairs_p->iv_pairs[count].value_p,"VLAN_TYPE"))
        {
          *nw_type_p=VLAN_TYPE;
        }
        else if(!strcmp((char *)mand_iv_pairs_p->iv_pairs[count].value_p,"VXLAN_TYPE"))
        {
          *nw_type_p=VXLAN_TYPE;
        }
        else
        {
          CM_CBK_DEBUG_PRINT ("Invalid network type selected");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_NW_TYPE_INVALID);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;

        }
        break;

      case CM_DM_NWSIDEPORTS_SWITCHNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VN NW sideports switch name is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_SWITCH_NAME_NULL);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_LOGICAL_SWITCH_NAME_LEN )
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Network NW Sideport Name");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_SWITCH_NAME_INVALID);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_NWSIDEPORTS_BRIDGENAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VN NW sideports bridge name is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_BRIDGE_NAME_NULL);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_LOGICAL_SWITCH_NAME_LEN )
        {
          CM_CBK_DEBUG_PRINT ("Invalid Virtual Network NW Sideport Name");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_BRIDGENAME_INVALID);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_NWSIDEPORTS_VXLAN_VNI_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("VXLAN VNI is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_VXLAN_VNI_ID_NULL);
          *presult_p = crm_vn_nw_sideports_result;                                                                 return OF_FAILURE;
        }
        vxlan_vni_id_set=TRUE;
        break;


      case CM_DM_NWSIDEPORTS_VXLAN_SERVICEPORT_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {                                                                                                          CM_CBK_DEBUG_PRINT ("VXLAN Service port is NULL");                                                       fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_PORT_ID_NULL);
          *presult_p = crm_vn_nw_sideports_result;                                                                 return OF_FAILURE;
        }
        vxlan_serviceport_set=TRUE;
        break;

      case CM_DM_NWSIDEPORTS_REMOTEIP_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)                                                    {
          CM_CBK_DEBUG_PRINT (" VXLAN Remote ip is NULL");
          fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_REMOTE_IP_NULL);
          *presult_p = crm_vn_nw_sideports_result;
          return OF_FAILURE;
        }                                                                                                       remote_ip_set=TRUE;                                                                                       break;
      default:
        break;                                                                                             }
  } 
  /***
    if(*nw_type_p == VLAN_TYPE)
    {
    if(vlan_id_set==FALSE)
    {
    CM_CBK_DEBUG_PRINT ("Network type selected VLAN TYPE .Enter vlan id.");
    fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_NW_PORT_VLAN_PARAMS_NOT_SET);
  *presult_p = crm_vn_nw_sideports_result;
  return OF_FAILURE;
  }
  }
  ***/
  if(*nw_type_p == VXLAN_TYPE)
  {
    if((vxlan_vni_id_set==FALSE) ||(vxlan_serviceport_set==FALSE)||(remote_ip_set==FALSE))
    {
      CM_CBK_DEBUG_PRINT ("Network type selected VXLAN TYPE .Please enter vxlan vni,serviceport and remoteip");
      fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_NW_PORT_VXLAN_PARAMS_NOT_SET);
      *presult_p = crm_vn_nw_sideports_result;
      return OF_FAILURE;
    }
  }





  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}
int32_t crm_vn_nw_sideports_validateoptparams(struct cm_array_of_iv_pairs *pOptParams,
    uint8_t *nw_type_p,
    struct cm_app_result **  presult_p)

{
  
//  uint32_t uiMandParamCnt;
//  struct cm_app_result *crm_vn_nw_sideports_result = NULL;
  CM_CBK_DEBUG_PRINT ("Entered");
//  uint8_t vlan_id_set=FALSE,vxlan_vni_id_set=FALSE,remote_ip_set=FALSE,vxlan_serviceport_set=FALSE;

  // for (uiMandParamCnt = 0; uiOptParamCnt < pMandParams->count_ui; uiOptParamCnt++)
  // {
  // switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
  {/**************
     case CM_DM_NWSIDEPORTS_VLANID_ID:
     if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
     {
     CM_CBK_DEBUG_PRINT ("VLAN ID is NULL");
     fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_VLAN_ID_NULL);
   *presult_p = crm_vn_nw_sideports_result;
   return OF_FAILURE;
   }
   vlan_id_set=TRUE;
   break;

   case CM_DM_NWSIDEPORTS_VXLAN_VNI_ID:
   if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
   {
   CM_CBK_DEBUG_PRINT ("VXLAN VNI is NULL");
   fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_VXLAN_VNI_ID_NULL);
   *presult_p = crm_vn_nw_sideports_result;
   return OF_FAILURE;
   }
   vxlan_vni_id_set=TRUE;
   break;

   case CM_DM_NWSIDEPORTS_VXLAN_SERVICEPORT_ID:
   if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
   {
   CM_CBK_DEBUG_PRINT ("VXLAN Service port is NULL");
   fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_PORT_ID_NULL);
   *presult_p = crm_vn_nw_sideports_result;
   return OF_FAILURE;
   }
   vxlan_serviceport_set=TRUE;
   break;

   case CM_DM_NWSIDEPORTS_REMOTEIP_ID:
   if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
   {
   CM_CBK_DEBUG_PRINT (" VXLAN Remote ip is NULL");
   fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_REMOTE_IP_NULL);
   *presult_p = crm_vn_nw_sideports_result;
   return OF_FAILURE;
   }
   remote_ip_set=TRUE;
   break;
   default:
   break;
   }
   }

   if(*nw_type_p == VLAN_TYPE)
   {
   if(vlan_id_set==FALSE)
   {
   CM_CBK_DEBUG_PRINT ("Network type selected VLAN TYPE .Enter vlan id.");
   fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_NW_PORT_VLAN_PARAMS_NOT_SET);
   *presult_p = crm_vn_nw_sideports_result;
   return OF_FAILURE;
   }
   }
   else if(*nw_type_p == VXLAN_TYPE)
   {
   if((vxlan_vni_id_set==FALSE) ||(vxlan_serviceport_set==FALSE)||(remote_ip_set==FALSE))
   {
   CM_CBK_DEBUG_PRINT ("Network type selected VXLAN TYPE .Please enter vxlan vni,serviceport and remoteip");
   fill_app_result_struct (&crm_vn_nw_sideports_result, NULL, CM_GLU_NW_PORT_VXLAN_PARAMS_NOT_SET);
   *presult_p = crm_vn_nw_sideports_result;
   return OF_FAILURE;
   }************/
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}

int32_t crm_vn_nw_sideports_setmandparams(struct cm_array_of_iv_pairs *pMandParams,
                                          struct crm_nwport_config_info* nw_port_config_info,
                                          struct cm_app_result ** presult_p)
{
  uint32_t uiMandParamCnt;
  CM_CBK_DEBUG_PRINT ("Entered");

  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui; uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_NWSIDEPORTS_PORTNAME_ID:
        CM_CBK_DEBUG_PRINT ("portname:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (nw_port_config_info->port_name,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;

      case CM_DM_NWSIDEPORTS_PORTTYPE_ID:
        CM_CBK_DEBUG_PRINT ("porttype:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        if(!strcmp((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,"NWSIDE_UNICAST_PORT"))
        {
          nw_port_config_info->port_type=NW_UNICAST_PORT;
        }
        else if(!strcmp((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,"NWSIDE_BROADCAST_PORT"))
        {
          nw_port_config_info->port_type=NW_BROADCAST_PORT;
        }
        break;

      case CM_DM_NWSIDEPORTS_NWTYPE_ID:
        CM_CBK_DEBUG_PRINT ("Network Type:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        if(!strcmp((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,"VLAN_TYPE"))
        {
          nw_port_config_info->nw_type=VLAN_TYPE;
        }
        else if(!strcmp((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,"VXLAN_TYPE"))
        {
          nw_port_config_info->nw_type=VXLAN_TYPE;
        }
        break;


      case CM_DM_NWSIDEPORTS_SWITCHNAME_ID:
        CM_CBK_DEBUG_PRINT ("switch_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (nw_port_config_info->switch_name,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
       CM_CBK_DEBUG_PRINT("switch_name:%s",nw_port_config_info->switch_name);
        break;

      case CM_DM_NWSIDEPORTS_VXLAN_VNI_ID:
            CM_CBK_DEBUG_PRINT ("vxlan_vni:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
            nw_port_config_info->nw_port_config_info.vxlan_info.nid=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           // *presult_p = crm_vn_nw_sideports_result;
                                
            break;
      case CM_DM_NWSIDEPORTS_BRIDGENAME_ID:
        CM_CBK_DEBUG_PRINT ("bridge_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        of_strncpy (nw_port_config_info->br_name,
            (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
      

       case CM_DM_NWSIDEPORTS_VXLAN_SERVICEPORT_ID:
            CM_CBK_DEBUG_PRINT ("vxlan_serviceport:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
            nw_port_config_info->nw_port_config_info.vxlan_info.service_port=of_atoi((char *)            pMandParams->iv_pairs[uiMandParamCnt].value_p);
            break;

       case CM_DM_NWSIDEPORTS_REMOTEIP_ID:
            CM_CBK_DEBUG_PRINT ("remoteip:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
            if(cm_val_and_conv_aton((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
            &(nw_port_config_info->nw_port_config_info.vxlan_info.remote_ip))!=OF_SUCCESS)
            {
              CM_CBK_DEBUG_PRINT("Invalid pool start address");
              return OF_FAILURE;                                                                                     }
            break;

    }
 }
  CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
  return OF_SUCCESS;
}


int32_t crm_vn_nw_sideports_setoptparams(struct cm_array_of_iv_pairs *pOptParams,
    struct crm_nwport_config_info* crm_nw_port_config_info_p,
    struct cm_app_result ** presult_p)



{

  CM_CBK_DEBUG_PRINT ("Entered");
 // for (uiMandParamCnt = 0; uiOptParamCnt < pMandParams->count_ui; uiOptParamCnt++)
 // {
 //   switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
   // {  
      /************************************
      case CM_DM_NWSIDEPORTS_VLANID_ID:
        CM_CBK_DEBUG_PRINT ("vlan id:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        crm_nw_port_config_info_p->nw_port_config_info.vlan_info.vlan_id=of_atoi((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;

      case CM_DM_NWSIDEPORTS_VXLAN_VNI_ID:
        CM_CBK_DEBUG_PRINT ("vxlan_vni:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        crm_nw_port_config_info_p->nw_port_config_info.vxlan_info.vxlan_vni=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;

      case CM_DM_NWSIDEPORTS_VXLAN_SERVICEPORT_ID:
        CM_CBK_DEBUG_PRINT ("vxlan_serviceport:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        crm_nw_port_config_info_p->nw_port_config_info.vxlan_info.vxlan_service_port=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        break;

      case CM_DM_NWSIDEPORTS_REMOTEIP_ID:
        CM_CBK_DEBUG_PRINT ("remoteip:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
        if(cm_val_and_conv_aton((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
              &(crm_nw_port_config_info_p->nw_port_config_info.vxlan_info.remote_ip))!=OF_SUCCESS)
        {
          CM_CBK_DEBUG_PRINT("Invalid pool start address");
          return OF_FAILURE;
        }
        break;
      *******/
   // }
 // }
CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
return OF_SUCCESS;
}
int32_t crm_nw_sideports_ucm_getparams(struct   crm_nwport_config_info *crm_port_info_p,
        struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0,port_chld_cnt=0;
  char buf[128] = "";
  CM_CBK_DEBUG_PRINT ("Entered");
  

  if(crm_port_info_p->nw_type == VXLAN_TYPE)
     port_chld_cnt=9;
  else if(crm_port_info_p->nw_type == VLAN_TYPE)
     port_chld_cnt=7;
  else
    return OF_FAILURE;


  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (port_chld_cnt, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }
  sprintf(buf,"%s",crm_port_info_p->port_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_PORTNAME_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  if(crm_port_info_p->port_type==NW_UNICAST_PORT)
  {
    sprintf(buf,"%s","NWSIDE_UNICAST_PORT");
  }
  else if(crm_port_info_p->port_type==NW_BROADCAST_PORT)
  {
    sprintf(buf,"%s","NWSIDE_BROADCAST_PORT");
  }
  else
    sprintf(buf,"%s","none");

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_PORTTYPE_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  if(crm_port_info_p->nw_type==VXLAN_TYPE)
  {
    sprintf(buf,"%s","VXLAN_TYPE");
  }
  else if(crm_port_info_p->port_type==NW_BROADCAST_PORT)
  {
    sprintf(buf,"%s","VLAN_TYPE");
  }
  else
    sprintf(buf,"%s","none");

  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_NWTYPE_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  memset(buf,0,sizeof(buf));

   sprintf(buf,"%s",crm_port_info_p->br_name);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_BRIDGENAME_ID,
      CM_DATA_TYPE_STRING, buf);
   uindex_i++;
  
  of_itoa (crm_port_info_p->portId, buf);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_PORTID_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;

  memset(buf,0,sizeof(buf));
  sprintf(buf,"%s",crm_port_info_p->switch_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_SWITCHNAME_ID,
      CM_DATA_TYPE_STRING, buf);
  uindex_i++;


  if(crm_port_info_p->nw_type == VXLAN_TYPE)
  { 
    of_itoa (crm_port_info_p->nw_port_config_info.vxlan_info.nid, buf);
    FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_VXLAN_VNI_ID,
        CM_DATA_TYPE_STRING, buf);
    uindex_i++;

    of_itoa (crm_port_info_p->nw_port_config_info.vxlan_info.service_port, buf);
    FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_VXLAN_SERVICEPORT_ID,
        CM_DATA_TYPE_STRING, buf);
    uindex_i++;

    cm_inet_ntoal(crm_port_info_p->nw_port_config_info.vxlan_info.remote_ip, buf);
    FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_REMOTEIP_ID,
        CM_DATA_TYPE_IPADDR, buf);
    uindex_i++;

   

  }
  /***
  else if(crm_port_info_p->nw_type == VLAN_TYPE)
  {
    of_itoa (crm_port_info_p->nw_port_config_info.vlan_info.vlan_id, buf);
    FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_NWSIDEPORTS_VLANID_ID,
        CM_DATA_TYPE_STRING, buf);
    uindex_i++;


  }
**/ 
  result_iv_pairs_p->count_ui = uindex_i;
  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;


}


#endif 



