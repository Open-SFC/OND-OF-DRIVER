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
 * File name: subnetcbk.c
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
#include "crm_subnet_api.h"

int32_t crm_subnet_appl_ucmcbk_init (void);
int32_t crm_subnet_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);
int32_t crm_subnet_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);



int32_t crm_subnet_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);
int32_t crm_subnet_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t crm_subnet_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t crm_subnet_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                        struct cm_array_of_iv_pairs ** pIvPairArr);


int32_t crm_subnet_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);






/************UTILTY FUNCTIONS ************/

int32_t crm_subnet_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                      struct crm_subnet_config_info *sub_config_info,
                                      struct cm_app_result ** presult_p);



int32_t crm_subnet_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct crm_subnet_config_info *sub_config_info,
                        struct cm_app_result ** presult_p);


uint32_t crm_subnet_ucm_validatemandparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                        struct cm_app_result **  presult_p);


uint32_t crm_subnet_ucm_validateoptsparams(struct cm_array_of_iv_pairs *mand_iv_pairs_p,
                        struct cm_app_result **  presult_p);



int32_t crm_subnet_ucm_getparams (struct crm_subnet_config_info *sub_config_info,
                                  struct cm_array_of_iv_pairs * result_iv_pairs_p);




struct cm_dm_app_callbacks crm_subnet_ucm_callbacks =
{
  NULL,
  crm_subnet_addrec,
  crm_subnet_setrec,
  crm_subnet_delrec,
  NULL,
  crm_subnet_getfirstnrecs,
  crm_subnet_getnextnrecs,
  crm_subnet_getexactrec,
  crm_subnet_verifycfg,
  NULL
};


int32_t crm_subnet_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_SUBNET_APPL_ID, &crm_subnet_ucm_callbacks);
  return OF_SUCCESS;
}

int32_t crm_subnet_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

  struct   cm_app_result *crm_subnet_result = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_subnet_config_info sub_config_info={};
  uint64_t output_subnet_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_subnet_ucm_setmandparams (pMandParams,&sub_config_info,&crm_subnet_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_subnet_result;
    return OF_FAILURE;
  }
  sub_config_info.ip_version=4;
   return_value = crm_subnet_ucm_setoptparams (pOptParams,&sub_config_info, &crm_subnet_result);
   if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
     fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
     *result_p = crm_subnet_result;
     return OF_FAILURE;
   }


  return_value =crm_add_subnet(&sub_config_info, &output_subnet_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm subnet add  Failed");
    fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SUBNET_ADD_FAILED);
    *result_p = crm_subnet_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM subnet added successfully");
  return OF_SUCCESS;
}
int32_t crm_subnet_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

  struct   cm_app_result *crm_subnet_result = NULL;
  int32_t  return_value = OF_FAILURE;
  struct   crm_subnet_config_info sub_config_info={};
  uint64_t output_subnet_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_subnet_ucm_setmandparams (pMandParams,&sub_config_info,&crm_subnet_result))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_subnet_result;
    return OF_FAILURE;
  }
  sub_config_info.ip_version=4;
   return_value = crm_subnet_ucm_setoptparams (pOptParams,&sub_config_info, &crm_subnet_result);
   if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
     fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
     *result_p = crm_subnet_result;
     return OF_FAILURE;
   }


  return_value =crm_modify_subnet(&sub_config_info, &output_subnet_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm subnet modification  Failed");
    fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SUBNET_ADD_FAILED);
    *result_p = crm_subnet_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM subnet Modified successfully");
  return OF_SUCCESS;
}
  

 

int32_t crm_subnet_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{
  struct cm_app_result *crm_subnet_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct crm_subnet_config_info sub_config_info={};

   

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_subnet_ucm_setmandparams (keys_arr_p,&sub_config_info , &crm_subnet_result)) !=
           OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p = crm_subnet_result;
    return OF_FAILURE;
  }

  return_value = crm_subnet_ucm_setoptparams (keys_arr_p,&sub_config_info, &crm_subnet_result);
   if (return_value != OF_SUCCESS)
   {
     CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
     fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
     *result_p = crm_subnet_result;
     return OF_FAILURE;
   }
  

  return_value =crm_del_subnet(sub_config_info.subnet_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm subnet add  Failed");
    fill_app_result_struct (&crm_subnet_result, NULL, CM_GLU_SUBNET_DEL_FAILED);
    *result_p = crm_subnet_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM subnet delete successfully");
  return OF_SUCCESS;

}


 int32_t crm_subnet_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)

 {

   struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiRecCount = 0;
   struct  crm_subnet_config_info sub_config_info={};
   struct  subnet_runtime_info  runtime_info={};
   uint64_t subnet_handle;

   CM_CBK_DEBUG_PRINT ("Entered");
   return_value = crm_get_first_subnet(&sub_config_info, &runtime_info, &subnet_handle);
 if (return_value != OF_SUCCESS)
 {
   CM_CBK_DEBUG_PRINT ("Get first record failed for CRM subnet Table");
   return OF_FAILURE;
 }


 result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
 if (result_iv_pairs_p == NULL)
 {
   CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
 }
 crm_subnet_ucm_getparams (&sub_config_info, &result_iv_pairs_p[uiRecCount]);
 uiRecCount++;
 *array_of_iv_pair_arr_p = result_iv_pairs_p;
 *count_p = uiRecCount;
 return OF_SUCCESS;
}

 int32_t crm_subnet_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)

 {
   struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   struct cm_app_result *subnet_result = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiRecCount = 0;

   struct  crm_subnet_config_info sub_config_info={};
   struct  subnet_runtime_info  runtime_info={};
   uint64_t subnet_handle;


  CM_CBK_DEBUG_PRINT ("Entered");

 if ((crm_subnet_ucm_setmandparams (prev_record_key_p, &sub_config_info, &subnet_result)) !=
                                OF_SUCCESS)
  {
     CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
     return OF_FAILURE;
  }
  return_value = crm_get_subnet_handle(sub_config_info.subnet_name, &subnet_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: subnet doesn't exists with name %s",sub_config_info.subnet_name);
    return OF_FAILURE;
  }

  of_memset (&sub_config_info, 0, sizeof(sub_config_info));

 return_value = crm_get_next_subnet(&sub_config_info, &runtime_info, &subnet_handle);

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
  crm_subnet_ucm_getparams (&sub_config_info, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}





int32_t crm_subnet_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                                      struct crm_subnet_config_info *sub_config_info,   
                                      struct cm_app_result ** presult_p)

{
  uint32_t uiMandParamCnt;
  
  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
  {
    switch(pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
      case CM_DM_SUBNET_NAME_ID:
           CM_CBK_DEBUG_PRINT ("subnet_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           of_strncpy (sub_config_info->subnet_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
           pMandParams->iv_pairs[uiMandParamCnt].value_length);
                                               
           break;
      
      case CM_DM_SUBNET_VNNAME_ID:
           CM_CBK_DEBUG_PRINT ("vn_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           of_strncpy (sub_config_info->vn_name, (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
           pMandParams->iv_pairs[uiMandParamCnt].value_length);

           break;

       case CM_DM_SUBNET_CIDRIP_ID:
            CM_CBK_DEBUG_PRINT ("subnet_ip:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
            if(cm_val_and_conv_aton((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,&(sub_config_info->cidr_ip))!=OF_SUCCESS)
            return OF_FAILURE;
            break;
              
                      
              
      case CM_DM_SUBNET_CIDRMASK_ID:
           CM_CBK_DEBUG_PRINT ("subnet_mask:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           if(cm_val_and_conv_aton((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p,&(sub_config_info->cidr_mask))!=OF_SUCCESS)
           return OF_FAILURE;
            break;

              
      case CM_DM_SUBNET_IP_VERSION_ID:
           CM_CBK_DEBUG_PRINT ("ip_version:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           if(of_atoi((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p)==4)
             sub_config_info->ip_version=of_atoi((char *)pMandParams->iv_pairs[uiMandParamCnt].value_p);
           else
             return CM_GLU_SUBNET_IP_VERSION_INVALID;
           break;



           
      }
    }
    CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
    return OF_SUCCESS;
}






int32_t crm_subnet_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** pIvPairArr)

{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *crm_subnet_result = NULL;
  struct crm_subnet_config_info   sub_config_info={};
  struct  subnet_runtime_info   runtime_info={};
  uint32_t uiRecCount = 0;
  int32_t iRes = OF_FAILURE;
  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset(&sub_config_info, 0, sizeof(sub_config_info));
  if((crm_subnet_ucm_setmandparams(key_iv_pairs_p, &sub_config_info, &crm_subnet_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }
 
 iRes = crm_get_subnet_by_name(sub_config_info.subnet_name, &sub_config_info,&runtime_info);
  if (iRes != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: Subnet doesn't exists with name %s",sub_config_info.subnet_name);
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
  crm_subnet_ucm_getparams(&sub_config_info, &result_iv_pairs_p[uiRecCount]);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}



int32_t crm_subnet_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_subnet_result = NULL;
  int32_t return_value = OF_FAILURE;
  struct crm_subnet_config_info   sub_config_info={};

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&sub_config_info, 0, sizeof (sub_config_info));

  return_value = crm_subnet_ucm_validatemandparams(key_iv_pairs_p, &of_subnet_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value = crm_subnet_ucm_validateoptsparams(key_iv_pairs_p, &of_subnet_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }
  *result_p = of_subnet_result;
  return OF_SUCCESS;

}



uint32_t crm_subnet_ucm_validatemandparams(struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **  presult_p)
{
  uint32_t count;
  struct cm_app_result *of_subnet_result = NULL;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (count = 0; count < mand_iv_pairs_p->count_ui;
      count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[count].id_ui)
    {
      case CM_DM_SUBNET_CIDRIP_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet IP is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_IP_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;
      

      case CM_DM_SUBNET_CIDRMASK_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet Mask is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_MASK_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;

  

     case CM_DM_SUBNET_IP_VERSION_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet IP version is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_IP_VERSION_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
               break;

   
   
      case CM_DM_SUBNET_NAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Subnet   name is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_NAME_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_SUBNET_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid subnet Name");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_NAME_ID_INVALID);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;

      case CM_DM_SUBNET_VNNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Subnet VN name  is NULL");
          fill_app_result_struct(&of_subnet_result, NULL, CM_GLU_VN_NAME_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        if (of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) < 0 ||
            of_strlen(mand_iv_pairs_p->iv_pairs[count].value_p) > CRM_MAX_VN_NAME_LEN)
        {
          CM_CBK_DEBUG_PRINT ("Invalid Subent VN name");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_VN_NAME_ID_INVALID);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;


           


   }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}




uint32_t crm_subnet_ucm_validateoptsparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct cm_app_result ** presult_p)
{
  uint32_t uiOptParamCnt;
  struct cm_app_result *of_subnet_result = NULL;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
        
  
      case CM_DM_SUBNET_GATEWAY_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet Gateway is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_GATEWAY_IP_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;
 

            
    case CM_DM_SUBNET_ENABLEDHCP_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet DHCP enable is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_ENABLE_DHCP_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;
       
     
    case CM_DM_SUBNET_DNS_SERVER1_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet Mask is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_DNS_SERVER_1_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;

     
    case CM_DM_SUBNET_DNS_SERVER2_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet Mask is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_DNS_SERVER_2_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;


    case CM_DM_SUBNET_DNS_SERVER3_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet Mask is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_DNS_SERVER_3_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;



     case CM_DM_SUBNET_DNS_SERVER4_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT (" Subnet Mask is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_DNS_SERVER_4_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }
        break;


     case CM_DM_SUBNET_POOL_START_ADDR_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Subnet Pool Start ID  is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_POOL_START_ID_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }

        break;


     case CM_DM_SUBNET_POOL_END_ADDR_ID:
        if (pOptParams->iv_pairs[uiOptParamCnt].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Subnet Pool End ID  is NULL");
          fill_app_result_struct (&of_subnet_result, NULL, CM_GLU_SUBNET_POOL_END_ID_NULL);
          *presult_p = of_subnet_result;
          return OF_FAILURE;
        }





  

    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
  return OF_SUCCESS;
}



















int32_t crm_subnet_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
                        struct crm_subnet_config_info *sub_config_info,
                        struct cm_app_result ** presult_p)

{

  uint32_t uiOptParamCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (uiOptParamCnt = 0; uiOptParamCnt < pOptParams->count_ui; uiOptParamCnt++)
  {
    switch (pOptParams->iv_pairs[uiOptParamCnt].id_ui)
    {
              
      case CM_DM_SUBNET_GATEWAY_ID:
     CM_CBK_DEBUG_PRINT ("subnet_gw_ip:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
      if(cm_val_and_conv_aton((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p,&(sub_config_info->gateway_ip))!=OF_SUCCESS)
      return OF_FAILURE;
      break;


      case CM_DM_SUBNET_ENABLEDHCP_ID:
      sub_config_info->enable_dhcp_b=of_atoi((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p);
      break;



     case CM_DM_SUBNET_DNS_SERVER1_ID:
     if(cm_val_and_conv_aton((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p,&(sub_config_info->dns_nameservers[0]))!=OF_SUCCESS)
     return OF_FAILURE;
     break;
           

     case CM_DM_SUBNET_DNS_SERVER2_ID:
     if(cm_val_and_conv_aton((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p,&(sub_config_info->dns_nameservers[1]))!=OF_SUCCESS)
     return OF_FAILURE;
     break;



     case CM_DM_SUBNET_DNS_SERVER3_ID:
     if(cm_val_and_conv_aton((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p,&(sub_config_info->dns_nameservers[2]))!=OF_SUCCESS)
     return OF_FAILURE;
     break;


              
     case CM_DM_SUBNET_DNS_SERVER4_ID:
     CM_CBK_DEBUG_PRINT ("dns4:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
     if(cm_val_and_conv_aton((char *)pOptParams->iv_pairs[uiOptParamCnt].value_p,&(sub_config_info->dns_nameservers[3]))!=OF_SUCCESS)
     return OF_FAILURE;
     break;
   
     case CM_DM_SUBNET_POOL_START_ADDR_ID:           
     CM_CBK_DEBUG_PRINT ("subnet_pool_start_ip:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
     if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &(sub_config_info->pool_start_address))!=OF_SUCCESS)
     {
        CM_CBK_DEBUG_PRINT("Invalid pool start address");
        return OF_FAILURE;
     }
     break;

     case CM_DM_SUBNET_POOL_END_ADDR_ID:
     CM_CBK_DEBUG_PRINT ("subnet_pool_end_ip:%s",(char *) pOptParams->iv_pairs[uiOptParamCnt].value_p);
     if(cm_val_and_conv_aton((char *) pOptParams->iv_pairs[uiOptParamCnt].value_p, &(sub_config_info->pool_end_address))!=OF_SUCCESS)
     {
        CM_CBK_DEBUG_PRINT("Invalid pool end address : %ld ",sub_config_info->pool_start_address);
        return OF_FAILURE;
     }


    }
         
   }
     
     CM_CBK_PRINT_IVPAIR_ARRAY (pOptParams);
     return OF_SUCCESS;
     
}



int32_t crm_subnet_ucm_getparams (struct crm_subnet_config_info *sub_config_info,
                                  struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;
  char buf[128] = "";
  CM_CBK_DEBUG_PRINT ("Entered");
         

  #define CM_SUBNET_CHILD_COUNT 13

  result_iv_pairs_p->iv_pairs =
  (struct cm_iv_pair *) of_calloc (CM_SUBNET_CHILD_COUNT, sizeof (struct cm_iv_pair));
         
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
  CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
  return OF_FAILURE;
  }

  sprintf(buf,"%s",sub_config_info->subnet_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_NAME_ID,
                   CM_DATA_TYPE_STRING, buf);
      
  uindex_i++;

         

  sprintf(buf,"%s",sub_config_info->vn_name);
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_VNNAME_ID,
                   CM_DATA_TYPE_STRING, buf);
  uindex_i++; 

         

   cm_inet_ntoal(sub_config_info->pool_start_address, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_POOL_START_ADDR_ID,
                    CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;
         
           

   cm_inet_ntoal(sub_config_info->pool_end_address, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_POOL_END_ADDR_ID,
                    CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


   cm_inet_ntoal(sub_config_info->cidr_ip, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_CIDRIP_ID,
                    CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


   cm_inet_ntoal(sub_config_info->cidr_mask, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_CIDRMASK_ID,
                              CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;



   sprintf(buf,"%d",sub_config_info->enable_dhcp_b);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_ENABLEDHCP_ID,
                    CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


   sprintf(buf,"%d",sub_config_info->ip_version);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_IP_VERSION_ID,
                    CM_DATA_TYPE_BOOLEAN, buf);
   uindex_i++;

         

   cm_inet_ntoal(sub_config_info->gateway_ip, buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_GATEWAY_ID,
                    CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


   of_memset(buf, 0 ,sizeof(buf));
   cm_inet_ntoal(sub_config_info->dns_nameservers[0], buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_DNS_SERVER1_ID,            CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


   of_memset(buf, 0 ,sizeof(buf));
   cm_inet_ntoal(sub_config_info->dns_nameservers[1], buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_DNS_SERVER2_ID,        CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;

 
         
   of_memset(buf, 0 ,sizeof(buf));
   cm_inet_ntoal(sub_config_info->dns_nameservers[2], buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_DNS_SERVER3_ID,
                         CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;

        
   of_memset(buf, 0 ,sizeof(buf));
   cm_inet_ntoal(sub_config_info->dns_nameservers[3], buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SUBNET_DNS_SERVER4_ID, CM_DATA_TYPE_IPADDR, buf);
   uindex_i++;


   result_iv_pairs_p->count_ui = uindex_i;
   return OF_SUCCESS;


}
#endif 
