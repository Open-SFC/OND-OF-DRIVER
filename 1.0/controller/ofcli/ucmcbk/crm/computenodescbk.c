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
 * File name: computenodescbk.c
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


int32_t crm_vn_computenode_appl_ucmcbk_init (void);
int32_t crm_vn_computenode_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);
int32_t crm_vn_computenode_setrec (void * config_trans_p,
                          struct cm_array_of_iv_pairs * pMandParams,
                          struct cm_array_of_iv_pairs * pOptParams,
                         struct cm_app_result ** result_p);
int32_t crm_vn_computenode_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);
int32_t crm_vn_computenode_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                            uint32_t * count_p,
			    struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);
int32_t crm_vn_computenode_getnextnrecs (struct cm_array_of_iv_pairs *keys_arr_p,
                             struct cm_array_of_iv_pairs *prev_record_key_p, 
 		             uint32_t * count_p,
			     struct cm_array_of_iv_pairs ** next_n_record_data_p);
int32_t crm_vn_computenode_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
        struct cm_array_of_iv_pairs ** pIvPairArr);
int32_t crm_vn_computenode_verifycfg(struct cm_array_of_iv_pairs *key_iv_pairs_p,
                            uint32_t command_id, 
			    struct cm_app_result ** result_p);
/***  Virtual network computenodes utility functions ***/
int32_t crm_vn_computenode_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
				        char *vn_name,
					char *switch_name,
					char *bridge_name);
int32_t crm_vn_computenode_ucm_validatemandparams(struct cm_array_of_iv_pairs *pMandParams,
			                      struct cm_app_result **presult_p);

int32_t crm_vn_computenode_ucm_getparams( char *switch_name_p, 
					 char *bridge_name_p,
					 struct cm_array_of_iv_pairs * result_iv_pairs_p);





struct cm_dm_app_callbacks crm_vn_comp_node_ucm_callbacks =
{
  NULL,
  crm_vn_computenode_addrec,
  crm_vn_computenode_setrec,
  crm_vn_computenode_delrec,
  NULL,
  crm_vn_computenode_getfirstnrecs,
  crm_vn_computenode_getnextnrecs,
  crm_vn_computenode_getexactrec,
  crm_vn_computenode_verifycfg,
  NULL
};


int32_t crm_vn_compuetnode_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_CRM_VIRTUALNETWORK_COMPUTENODES_APPL_ID, &crm_vn_comp_node_ucm_callbacks);
  return OF_SUCCESS;
}

/****************************************************************/
int32_t crm_vn_computenode_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
 
{
  struct   cm_app_result *crm_vn_comp_result = NULL;
  int32_t  return_value = OF_FAILURE; 
  char     vn_name[CRM_MAX_VN_NAME_LEN] ={};
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN+1]={};
  char     bridge_name[CRM_LOGICAL_SWITCH_NAME_LEN]={};
  
  CM_CBK_DEBUG_PRINT ("Entered");
 
  if ((crm_vn_computenode_setmandparams(pMandParams, vn_name, switch_name, bridge_name))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_comp_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vn_comp_result;
    return OF_FAILURE;

  } 

  return_value = crm_add_crm_compute_node_to_vn(vn_name, switch_name, bridge_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn compute node  Failed");
    fill_app_result_struct (&crm_vn_comp_result, NULL, CM_GLU_VN_COMP_NODE_ADD_FAILED);
    *result_p = crm_vn_comp_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM VN compute node added successfully");
  return OF_SUCCESS;
}
int32_t crm_vn_computenode_setrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
 
{
  struct   cm_app_result *crm_vn_comp_result = NULL;
  int32_t  return_value = OF_FAILURE; 
  char     vn_name[CRM_MAX_VN_NAME_LEN] ={};
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN+1]={};
  char     bridge_name[CRM_LOGICAL_SWITCH_NAME_LEN]={};
  
  CM_CBK_DEBUG_PRINT ("Entered");
 
  if ((crm_vn_computenode_setmandparams(pMandParams, vn_name, switch_name, bridge_name))!= OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_comp_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vn_comp_result;
    return OF_FAILURE;

  } 

  return_value = crm_add_crm_compute_node_to_vn(vn_name, switch_name, bridge_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn compute node  Failed");
    fill_app_result_struct (&crm_vn_comp_result, NULL, CM_GLU_VN_COMP_NODE_ADD_FAILED);
    *result_p = crm_vn_comp_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM VN compute node added successfully");
  return OF_SUCCESS;
}



/**********************************************************************************************/
int32_t crm_vn_computenode_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
		                          uint32_t * count_p,
    	                                  struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  struct   cm_app_result *crm_vn_result = NULL;
  char     vn_name[CRM_MAX_VN_NAME_LEN]={};
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN]={};
  char     bridge_name[CRM_LOGICAL_SWITCH_NAME_LEN]={};

  CM_CBK_DEBUG_PRINT ("Entered");
  if ((crm_vn_computenode_setmandparams (keys_arr_p, vn_name, switch_name, bridge_name)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  } 
  return_value = crm_get_first_crm_compute_node(vn_name, switch_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first record failed for CRM virtual network computenode table");
    return OF_FAILURE;
  }
  return_value = crm_get_first_crm_logical_switch(vn_name, switch_name,bridge_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get first record failed for CRM logical switch");
    return OF_FAILURE;
  }

  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  crm_vn_computenode_ucm_getparams (switch_name, bridge_name, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *array_of_iv_pair_arr_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS;
}

int32_t crm_vn_computenode_getnextnrecs (struct cm_array_of_iv_pairs *keys_arr_p,
                                         struct cm_array_of_iv_pairs *prev_record_key_p, 
 				         uint32_t * count_p,
			                 struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  uint32_t uiRecCount = 0;
  char     vn_name[CRM_MAX_VN_NAME_LEN]={};
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN]={};
  char     bridge_name[CRM_LOGICAL_SWITCH_NAME_LEN]={};

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_computenode_setmandparams (keys_arr_p,
                                vn_name,
                                switch_name,
                                bridge_name
				)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  } 


  if ((crm_vn_computenode_setmandparams (prev_record_key_p,
                                vn_name,
                                switch_name,
                                bridge_name
				)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  } 

  return_value = crm_get_next_crm_logical_switch(vn_name, switch_name,bridge_name);
  if(return_value!=CRM_SUCCESS)
  {
    do
    {
      return_value = crm_get_next_crm_compute_node(vn_name, switch_name);
      if (return_value != CRM_SUCCESS)
      {
	CM_CBK_DEBUG_PRINT ("crm_get_next_crm_compute_node failed");
	return OF_FAILURE;
       }
       return_value = crm_get_first_crm_logical_switch(vn_name, switch_name,bridge_name);
    }while(return_value!=OF_SUCCESS);
  }

  if(return_value!=CRM_SUCCESS)
  {
    return CRM_FAILURE;
  }
  result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
    return OF_FAILURE;
  }
  crm_vn_computenode_ucm_getparams (switch_name, bridge_name, &result_iv_pairs_p[uiRecCount]);
  uiRecCount++;
  *next_n_record_data_p = result_iv_pairs_p;
  *count_p = uiRecCount;
  return OF_SUCCESS; 
}

int32_t crm_vn_computenode_ucm_getparams(char* switch_name_p, char *bridge_name_p,struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  uint32_t uindex_i = 0;

  CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (2, sizeof (struct cm_iv_pair));
  if (result_iv_pairs_p->iv_pairs == NULL)
  {
    CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
    return OF_FAILURE;
  }
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_COMPUTENODES_COMPUTENODENAME_ID,
                   CM_DATA_TYPE_STRING, switch_name_p);
  uindex_i++;
  FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_COMPUTENODES_BRIDGENAME_ID,
                   CM_DATA_TYPE_STRING, bridge_name_p);
  uindex_i++;
  result_iv_pairs_p->count_ui = uindex_i;
  return OF_SUCCESS;

}

int32_t crm_vn_computenode_delrec(void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p) 

{
  
  struct   cm_app_result *crm_vn_comp_result = NULL;
  int32_t  return_value = OF_FAILURE;
  char     vn_name[CRM_MAX_VN_NAME_LEN] ={};
  char     switch_name[CRM_LOGICAL_SWITCH_NAME_LEN]={};
  char     bridge_name[DPRM_MAX_SWITCH_NAME_LEN+1]={};

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_computenode_setmandparams (keys_arr_p,
                                vn_name,
                                switch_name,
                                bridge_name
				)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&crm_vn_comp_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=crm_vn_comp_result;
    return OF_FAILURE;

  }

 
  return_value = crm_del_crm_compute_node_from_vn(vn_name, switch_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("crm vn compute node  Failed");
    fill_app_result_struct (&crm_vn_comp_result, NULL, CM_GLU_VN_COMP_NODE_DEL_FAILED);
    *result_p = crm_vn_comp_result;
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("CRM VN compute node deleted successfully");
  return OF_SUCCESS;
}


 


int32_t crm_vn_computenode_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
					char *vn_name,
					char *switch_name,
					char *bridge_name_p)
{
  uint32_t uiMandParamCnt;
  
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui; uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
    
#if 0 
      case CM_DM_COMPUTENODES_VNNAME_ID:
           CM_CBK_DEBUG_PRINT ("comp_node_vn_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           of_strncpy (vn_name,
                      (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                      pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

#endif
     case CM_DM_VIRTUALNETWORK_NAME_ID:
           CM_CBK_DEBUG_PRINT ("comp_node_vn_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
           of_strncpy (vn_name,
                      (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                      pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;
 

     case CM_DM_COMPUTENODES_COMPUTENODENAME_ID:
          CM_CBK_DEBUG_PRINT ("comp_node_switch_name:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy (switch_name,
                     (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                     pMandParams->iv_pairs[uiMandParamCnt].value_length);
      break;

     case CM_DM_COMPUTENODES_BRIDGENAME_ID:
          CM_CBK_DEBUG_PRINT ("comp_node_bridge:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
          of_strncpy ((char*)bridge_name_p,
                     (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                     pMandParams->iv_pairs[uiMandParamCnt].value_length);

      break;

    }
  }


 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
 return OF_SUCCESS;

}

int32_t crm_vn_computenode_verifycfg(struct cm_array_of_iv_pairs *key_iv_pairs_p,
                            uint32_t command_id, struct cm_app_result ** result_p)
{
  struct cm_app_result *of_result = NULL;
  int32_t return_value = OF_FAILURE;
  CM_CBK_DEBUG_PRINT ("Entered");
  return_value = crm_vn_computenode_ucm_validatemandparams (key_iv_pairs_p, &of_result);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
       return OF_FAILURE;
  }
  *result_p = of_result;
  return OF_SUCCESS;

}

int32_t crm_vn_computenode_ucm_validatemandparams(struct cm_array_of_iv_pairs *
                            pMandParams,    struct cm_app_result **presult_p)
{
  uint32_t uiMandParamCnt;
  struct cm_app_result *of_result = NULL;
  
  for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui; uiMandParamCnt++)
  {
    switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
    {
     case CM_DM_COMPUTENODES_COMPUTENODENAME_ID:
          if((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
	  {
	     CM_CBK_DEBUG_PRINT ("Compute node name null!");
	     fill_app_result_struct (&of_result, NULL, CM_GLU_VN_COMP_NODE_NAME_NULL);
	     *presult_p = of_result;
	     return OF_FAILURE;
	  }
          break;

     case CM_DM_COMPUTENODES_BRIDGENAME_ID:
	  if((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
	  {
	     CM_CBK_DEBUG_PRINT ("Bridge name null!");
	     fill_app_result_struct (&of_result, NULL, CM_GLU_BRIDGE_NAME_NULL);
	     *presult_p = of_result;
	     return OF_FAILURE;
	  }

          break;

    }
  }


 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
 return OF_SUCCESS;


}

int32_t crm_vn_computenode_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
        struct cm_array_of_iv_pairs ** pIvPairArr)
{
  struct   cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t  return_value = OF_FAILURE;
  char     vn_name[CRM_MAX_VN_NAME_LEN]={};
  char     switch_name[DPRM_MAX_SWITCH_NAME_LEN]={};
  char     bridge_name[CRM_LOGICAL_SWITCH_NAME_LEN]={};

  CM_CBK_DEBUG_PRINT ("Entered");

  if ((crm_vn_computenode_setmandparams (key_iv_pairs_p,
                                vn_name,
                                switch_name,
                                bridge_name
				)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  } 

  return_value = crm_get_exact_crm_compute_node_by_name(vn_name, switch_name);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: compute node doesn't exists with name %s",switch_name);
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
  crm_vn_computenode_ucm_getparams(switch_name, bridge_name,result_iv_pairs_p);
  *pIvPairArr = result_iv_pairs_p;
  return OF_SUCCESS;
}
#endif
