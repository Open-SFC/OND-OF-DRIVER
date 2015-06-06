#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "nsrm.h"

int32_t nsrm_nwservice_instance_init (void);

int32_t nsrm_nwservice_instance_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_nwservice_instance_modrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p);

int32_t nsrm_nwservice_instance_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p);

int32_t nsrm_nwservice_instance_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t nsrm_nwservice_instance_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t nsrm_nwservice_instance_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t nsrm_nwservice_instance_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p);

int32_t nsrm_nwservice_instance_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nwservice_instance_key*  nsrm_service_instance_key_info,
                        struct nsrm_nwservice_attach_key*     nschain_nwservice_object_p);

int32_t nsrm_nwservice_instance_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams);

int32_t nsrm_nwservice_instance_ucm_getparams (struct nsrm_nwservice_instance_record *instance_info,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p);

struct cm_dm_app_callbacks nsrm_nwservice_instance_callbacks =
{
  NULL,
  nsrm_nwservice_instance_addrec,
  NULL,
  nsrm_nwservice_instance_delrec,
  NULL,
  nsrm_nwservice_instance_getfirstnrecs,
  nsrm_nwservice_instance_getnextnrecs,
  nsrm_nwservice_instance_getexactrec,
  nsrm_nwservice_instance_verifycfg,
  NULL
};



int32_t nsrm_nwservice_instance_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  if (cm_register_app_callbacks (CM_ON_DIRECTOR_NSRM_CHAIN_SERVICES_SERVICE_INSTANCE_APPL_ID, &nsrm_nwservice_instance_callbacks) != OF_SUCCESS)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t nsrm_nwservice_instance_addrec (void * config_trans_p,
                        struct cm_array_of_iv_pairs * pMandParams,
                        struct cm_array_of_iv_pairs * pOptParams,
                        struct cm_app_result ** result_p)
{

   struct cm_app_result *nsrm_nwservice_instance_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_instance_key     *nsrm_service_instance_key_info = NULL;
   struct nsrm_nwservice_attach_key       *nschain_nwservice_object_p = NULL;

   CM_CBK_DEBUG_PRINT ("Entered(nsrm_nwservice_instance_addrec)");

   nsrm_service_instance_key_info = (struct nsrm_nwservice_instance_key*)of_calloc(1, sizeof(struct nsrm_nwservice_instance_key));
   nschain_nwservice_object_p = (struct nsrm_nwservice_attach_key*)of_calloc(1, sizeof(struct nsrm_nwservice_attach_key));

  if ((nsrm_nwservice_instance_ucm_setmandparams (pMandParams, nsrm_service_instance_key_info,nschain_nwservice_object_p)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_nwservice_instance_result;
    of_free(nsrm_service_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_service_instance_key_info->tenant_name_p);
    of_free(nsrm_service_instance_key_info->vm_name_p);
    of_free(nsrm_service_instance_key_info->nschain_object_name_p);
    of_free(nsrm_service_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_service_instance_key_info);
    return OF_FAILURE;
  }


  if ((nsrm_nwservice_instance_ucm_setoptparams (pOptParams)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
    *result_p=nsrm_nwservice_instance_result;
    return OF_FAILURE;
  }
  return_value =  nsrm_register_launched_nwservice_object_instance(nsrm_service_instance_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm chain add  Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SERVICE_INSTANCE_ADD_FAILED);
    *result_p = nsrm_nwservice_instance_result;
    of_free(nsrm_service_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_service_instance_key_info->tenant_name_p);
    of_free(nsrm_service_instance_key_info->vm_name_p);
    of_free(nsrm_service_instance_key_info->nschain_object_name_p);
    of_free(nsrm_service_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_service_instance_key_info);
    return OF_FAILURE;
  }

  CM_CBK_DEBUG_PRINT ("NSRM chain added successfully");
  of_free(nsrm_service_instance_key_info->nwservice_instance_name_p);
  of_free(nsrm_service_instance_key_info->tenant_name_p);
  of_free(nsrm_service_instance_key_info->vm_name_p);
  of_free(nsrm_service_instance_key_info->nschain_object_name_p);
  of_free(nsrm_service_instance_key_info->nwservice_object_name_p);
  of_free(nsrm_service_instance_key_info);
  return OF_SUCCESS;
}



int32_t nsrm_nwservice_instance_delrec (void * config_transaction_p,
                        struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_app_result ** result_p)
{
   struct cm_app_result *nsrm_nwservice_instance_result = NULL;
   int32_t return_value = OF_FAILURE;
   struct nsrm_nwservice_instance_key     *nsrm_service_instance_key_info = NULL;
   struct nsrm_nwservice_attach_key       *nschain_nwservice_object_p = NULL;
   
   nsrm_service_instance_key_info = (struct nsrm_nwservice_instance_key*)of_calloc(1, sizeof(struct nsrm_nwservice_instance_key));
   nschain_nwservice_object_p = (struct nsrm_nwservice_attach_key*)of_calloc(1, sizeof(struct nsrm_nwservice_attach_key));

  if ((nsrm_nwservice_instance_ucm_setmandparams (keys_arr_p, nsrm_service_instance_key_info,nschain_nwservice_object_p)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    *result_p=nsrm_nwservice_instance_result;
    of_free(nsrm_service_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_service_instance_key_info->tenant_name_p);
    of_free(nsrm_service_instance_key_info->vm_name_p);
    of_free(nsrm_service_instance_key_info->nschain_object_name_p);
    of_free(nsrm_service_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_service_instance_key_info);
    return OF_FAILURE;
  }

  return_value =  nsrm_register_delaunch_nwswervice_object_instance(nsrm_service_instance_key_info);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("nsrm service instance delete  Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SERVICE_INSTANCE_DEL_FAILED);
    *result_p = nsrm_nwservice_instance_result;
    of_free(nsrm_service_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_service_instance_key_info->tenant_name_p);
    of_free(nsrm_service_instance_key_info->vm_name_p);
    of_free(nsrm_service_instance_key_info->nschain_object_name_p);
    of_free(nsrm_service_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_service_instance_key_info);
    return OF_FAILURE;
  }
  CM_CBK_DEBUG_PRINT ("NSRM service instance deleted successfully");
  of_free(nsrm_service_instance_key_info->nwservice_instance_name_p);
  of_free(nsrm_service_instance_key_info->tenant_name_p);
  of_free(nsrm_service_instance_key_info->vm_name_p);
  of_free(nsrm_service_instance_key_info->nschain_object_name_p);
  of_free(nsrm_service_instance_key_info->nwservice_object_name_p);
  of_free(nsrm_service_instance_key_info);
  
  return OF_SUCCESS;
}


int32_t nsrm_nwservice_instance_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
                        struct nsrm_nwservice_instance_key*  nsrm_service_instance_key_info,
                        struct nsrm_nwservice_attach_key    *nschain_nwservice_object_p)

{
   uint32_t uiMandParamCnt;

   CM_CBK_DEBUG_PRINT ("Entered");
   nsrm_service_instance_key_info->nwservice_instance_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   nsrm_service_instance_key_info->tenant_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   nsrm_service_instance_key_info->vm_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   nsrm_service_instance_key_info->nschain_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   nsrm_service_instance_key_info->nwservice_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
   for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;uiMandParamCnt++)
   {
     switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
     {

       case CM_DM_CHAIN_TENANT_ID:
       CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
       of_strncpy (nsrm_service_instance_key_info->tenant_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
       

       case CM_DM_SERVICE_INSTANCE_INSTANCE_NAME_ID:
       CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
       of_strncpy (nsrm_service_instance_key_info->nwservice_instance_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
  
       case CM_DM_SERVICES_SRVNAME_ID:
       CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
       nschain_nwservice_object_p->name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
       of_strncpy (nsrm_service_instance_key_info->nwservice_object_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
       of_strncpy (nschain_nwservice_object_p->name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
       break;
  
       case CM_DM_CHAIN_NAME_ID:
       CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
       nschain_nwservice_object_p->nschain_object_name_p = (char *) of_calloc(1, NSRM_MAX_NAME_LENGTH);
       of_strncpy (nsrm_service_instance_key_info->nschain_object_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
        of_strncpy (nschain_nwservice_object_p->nschain_object_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);

        break;
  
      case CM_DM_SERVICE_INSTANCE_VM_NAME_ID:
       CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
       of_strncpy (nsrm_service_instance_key_info->vm_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
  
      /*
      case CM_DM_SERVICE_INSTANCE_VMNS_PORT_NAME_ID:
       CM_CBK_DEBUG_PRINT ("name_p:%s",(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
       of_strncpy (nsrm_service_instance_key_info->vm_ns_port_name_p,(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                   pMandParams->iv_pairs[uiMandParamCnt].value_length);
        break;
 
      case CM_DM_SERVICE_INSTANCE_PORT_ID_ID:
         nsrm_service_instance_key_info->port_id  = of_atoi((char*)pMandParams->iv_pairs[uiMandParamCnt].value_p);
         CM_CBK_DEBUG_PRINT ("port id:%d",nsrm_service_instance_key_info->port_id);
      break;
      */     

     case CM_DM_SERVICE_INSTANCE_VLANID_IN_ID:
         nsrm_service_instance_key_info->vlan_id_in  = of_atoi((char*)pMandParams->iv_pairs[uiMandParamCnt].value_p);
         CM_CBK_DEBUG_PRINT ("vlan id in:%d",nsrm_service_instance_key_info->vlan_id_in);
      break;
  
    case CM_DM_SERVICE_INSTANCE_VLANID_OUT_ID:
         nsrm_service_instance_key_info->vlan_id_out  = of_atoi((char*)pMandParams->iv_pairs[uiMandParamCnt].value_p);
         CM_CBK_DEBUG_PRINT ("vlan id out:%d",nsrm_service_instance_key_info->vlan_id_out);
      break;
 
     }
   }
  
   return NSRM_SUCCESS;
}
  
    
int32_t nsrm_nwservice_instance_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams)
{
  
   return NSRM_SUCCESS;
}


 
int32_t nsrm_nwservice_instance_ucm_getparams (struct nsrm_nwservice_instance_record *nwservice_instance_info_p,
                                   struct cm_array_of_iv_pairs * result_iv_pairs_p)

{
   uint32_t uindex_i = 0;
   char buf[128] = "";
   CM_CBK_DEBUG_PRINT ("Entered");
#define CM_INSTANCE_CHILD_COUNT 11

   result_iv_pairs_p->iv_pairs =(struct cm_iv_pair *) of_calloc (CM_INSTANCE_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
     CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
     return OF_FAILURE;
   }
 
   CM_CBK_DEBUG_PRINT("debug");
   sprintf(buf,"%s",nwservice_instance_info_p->key.nwservice_instance_name_p);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICE_INSTANCE_INSTANCE_NAME_ID ,
                   CM_DATA_TYPE_STRING, buf);

   uindex_i++;


   CM_CBK_DEBUG_PRINT("debug");
   sprintf(buf,"%s",nwservice_instance_info_p->key.vm_name_p);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICE_INSTANCE_VM_NAME_ID,
                   CM_DATA_TYPE_STRING, buf);

   uindex_i++;
  
   /*
   CM_CBK_DEBUG_PRINT("debug");
   sprintf(buf,"%s",nwservice_instance_info_p->key.vm_ns_port_name_p);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICE_INSTANCE_VMNS_PORT_NAME_ID,
                   CM_DATA_TYPE_STRING, buf);

   uindex_i++;
   
   CM_CBK_DEBUG_PRINT("debug");
   of_memset(buf, 0, sizeof(buf));
   of_itoa(nwservice_instance_info_p->key.port_id , buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICE_INSTANCE_PORT_ID_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;
   */

   CM_CBK_DEBUG_PRINT("debug");
   of_memset(buf, 0, sizeof(buf));
   of_itoa(nwservice_instance_info_p->key.vlan_id_in , buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICE_INSTANCE_VLANID_IN_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;


   CM_CBK_DEBUG_PRINT("debug");
   of_memset(buf, 0, sizeof(buf));
   of_itoa(nwservice_instance_info_p->key.vlan_id_out , buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_SERVICE_INSTANCE_VLANID_OUT_ID,
                   CM_DATA_TYPE_STRING, buf);
   uindex_i++;
 
   CM_CBK_DEBUG_PRINT("debug");
   result_iv_pairs_p->count_ui = uindex_i;
   CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);

   return OF_SUCCESS;
}

 
int32_t nsrm_nwservice_instance_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
   struct    cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   int32_t   return_value = OF_FAILURE;
   uint32_t  uiRecCount = 0;
   int32_t   uiRequestedCount = 1, uiReturnedCnt=0;
   struct    nsrm_nwservice_instance_record *nsrm_instance_record = NULL;
   struct    cm_app_result *nsrm_nwservice_instance_result = NULL;
   struct    nsrm_nwservice_instance_key  *nsrm_nwservice_instance_key_info = NULL;
   struct    nsrm_nwservice_attach_key     *nschain_nwservice_object_p = NULL;
 
   CM_CBK_DEBUG_PRINT ("Entered");
   
   nschain_nwservice_object_p   = (struct nsrm_nwservice_attach_key*)of_calloc(1,sizeof(struct nsrm_nwservice_attach_key));
   nsrm_nwservice_instance_key_info = (struct nsrm_nwservice_instance_key*) of_calloc(1,sizeof(struct nsrm_nwservice_instance_key));
   nsrm_instance_record = (struct nsrm_nwservice_instance_record*) of_calloc(1,sizeof(struct nsrm_nwservice_instance_record));
   nsrm_instance_record->key.nwservice_instance_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.nwservice_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.tenant_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.vm_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.nschain_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);

   if ((nsrm_nwservice_instance_ucm_setmandparams (keys_arr_p,nsrm_nwservice_instance_key_info,nschain_nwservice_object_p)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    of_free(nsrm_nwservice_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_nwservice_instance_key_info->tenant_name_p);
    of_free(nsrm_nwservice_instance_key_info->vm_name_p);
    of_free(nsrm_nwservice_instance_key_info->nschain_object_name_p);
    of_free(nsrm_nwservice_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_nwservice_instance_key_info);
    return OF_FAILURE;
  }
   return_value = nsrm_get_first_nwservice_instance(nschain_nwservice_object_p,uiRequestedCount , &uiReturnedCnt ,nsrm_instance_record);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM service instance  Table");
      return OF_FAILURE;
   }
   
   result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (result_iv_pairs_p == NULL)
   {
     CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     of_free(nsrm_instance_record->key.nwservice_instance_name_p);
     of_free(nsrm_instance_record->key.tenant_name_p);
     of_free(nsrm_instance_record->key.vm_name_p);
     of_free(nsrm_instance_record->key.nschain_object_name_p);
     of_free(nsrm_instance_record->key.nwservice_object_name_p);
     of_free(nsrm_instance_record);

     return OF_FAILURE;
   }
  
   nsrm_nwservice_instance_ucm_getparams(nsrm_instance_record, &result_iv_pairs_p[uiRecCount]);
   uiRecCount++;
   *array_of_iv_pair_arr_p = result_iv_pairs_p;
   of_free(nsrm_instance_record->key.nwservice_instance_name_p);
   of_free(nsrm_instance_record->key.tenant_name_p);
   of_free(nsrm_instance_record->key.vm_name_p);
   of_free(nsrm_instance_record->key.nschain_object_name_p);
   of_free(nsrm_instance_record->key.nwservice_object_name_p);
   of_free(nsrm_instance_record);
 
   return OF_SUCCESS;
}


int32_t nsrm_nwservice_instance_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
                        struct cm_array_of_iv_pairs *prev_record_key_p,  uint32_t * count_p,
                        struct cm_array_of_iv_pairs ** next_n_record_data_p)

{
   struct    cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   int32_t   return_value = OF_FAILURE;
   uint32_t  uiRecCount = 0;
   int32_t   uiRequestedCount = 1, uiReturnedCnt=0;
   struct    nsrm_nwservice_instance_record *nsrm_instance_record = NULL;
   struct    cm_app_result *nsrm_nwservice_instance_result = NULL;
   struct    nsrm_nwservice_instance_key  *nsrm_nwservice_instance_key_info = NULL;
   struct    nsrm_nwservice_attach_key    *nschain_nwservice_object_p = NULL;
 
   
   nschain_nwservice_object_p   = (struct nsrm_nwservice_attach_key*)of_calloc(1,sizeof(struct nsrm_nwservice_attach_key));
   nsrm_nwservice_instance_key_info = (struct nsrm_nwservice_instance_key*) of_calloc(1,sizeof(struct nsrm_nwservice_instance_key));
   nsrm_instance_record = (struct nsrm_nwservice_instance_record*) of_calloc(1,sizeof(struct nsrm_nwservice_instance_record));
   nsrm_instance_record->key.nwservice_instance_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.nwservice_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.tenant_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.vm_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.nschain_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);

   nsrm_nwservice_instance_ucm_setmandparams (keys_arr_p, nsrm_nwservice_instance_key_info,nschain_nwservice_object_p);

   CM_CBK_DEBUG_PRINT("chain name : %s",nschain_nwservice_object_p->nschain_object_name_p);
   if ((nsrm_nwservice_instance_ucm_setmandparams (prev_record_key_p, nsrm_nwservice_instance_key_info,nschain_nwservice_object_p)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    of_free(nsrm_nwservice_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_nwservice_instance_key_info->tenant_name_p);
    of_free(nsrm_nwservice_instance_key_info->vm_name_p);
    of_free(nsrm_nwservice_instance_key_info->nschain_object_name_p);
    of_free(nsrm_nwservice_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_nwservice_instance_key_info);
    return OF_FAILURE;
  }
   CM_CBK_DEBUG_PRINT("chain name : %s",nschain_nwservice_object_p->nschain_object_name_p);
   CM_CBK_DEBUG_PRINT("chain name : %s",nschain_nwservice_object_p->name_p);
   return_value = nsrm_get_next_nwservice_instance(nschain_nwservice_object_p,nsrm_nwservice_instance_key_info,uiRequestedCount , &uiReturnedCnt ,nsrm_instance_record);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM service instance  Table");
      return OF_FAILURE;
   }

   result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (result_iv_pairs_p == NULL)
   {
     CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     of_free(nsrm_instance_record->key.nwservice_instance_name_p);
     of_free(nsrm_instance_record->key.tenant_name_p);
     of_free(nsrm_instance_record->key.vm_name_p);
     of_free(nsrm_instance_record->key.nschain_object_name_p);
     of_free(nsrm_instance_record->key.nwservice_object_name_p);
     of_free(nsrm_instance_record);

     return OF_FAILURE;
   }
   nsrm_nwservice_instance_ucm_getparams(nsrm_instance_record, &result_iv_pairs_p[uiRecCount]);
   uiRecCount++;
   *next_n_record_data_p = result_iv_pairs_p;
   *count_p = uiRecCount;  
  
   of_free(nsrm_instance_record->key.nwservice_instance_name_p);
   of_free(nsrm_instance_record->key.tenant_name_p);
   of_free(nsrm_instance_record->key.vm_name_p);
   of_free(nsrm_instance_record->key.nschain_object_name_p);
   of_free(nsrm_instance_record->key.nwservice_object_name_p);
   of_free(nsrm_instance_record);
   
   return NSRM_SUCCESS;

}


int32_t nsrm_nwservice_instance_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs_p,
                                        struct cm_array_of_iv_pairs ** pIvPairArr)
{
   struct    cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   int32_t   return_value = OF_FAILURE;
   struct    cm_app_result  *nsrm_nwservice_instance_result = NULL;
   struct    nsrm_nwservice_instance_record *nsrm_instance_record = NULL;
   struct    nsrm_nwservice_instance_key  *nsrm_nwservice_instance_key_info = NULL;
   struct    nsrm_nwservice_attach_key     *nschain_nwservice_object_p = NULL;
 
   CM_CBK_DEBUG_PRINT ("Entered");
   
   nschain_nwservice_object_p   = (struct nsrm_nwservice_attach_key*)of_calloc(1,sizeof(struct nsrm_nwservice_attach_key));
   nsrm_nwservice_instance_key_info = (struct nsrm_nwservice_instance_key*) of_calloc(1,sizeof(struct nsrm_nwservice_instance_key));
   nsrm_instance_record = (struct nsrm_nwservice_instance_record*) of_calloc(1,sizeof(struct nsrm_nwservice_instance_record));
   nsrm_instance_record->key.nwservice_instance_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.nwservice_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.tenant_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.vm_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
   nsrm_instance_record->key.nschain_object_name_p = (char *)of_calloc(1,NSRM_MAX_NAME_LENGTH);
  
   if ((nsrm_nwservice_instance_ucm_setmandparams (key_iv_pairs_p, nsrm_nwservice_instance_key_info,nschain_nwservice_object_p)) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    fill_app_result_struct (&nsrm_nwservice_instance_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
    of_free(nsrm_nwservice_instance_key_info->nwservice_instance_name_p);
    of_free(nsrm_nwservice_instance_key_info->tenant_name_p);
    of_free(nsrm_nwservice_instance_key_info->vm_name_p);
    of_free(nsrm_nwservice_instance_key_info->nschain_object_name_p);
    of_free(nsrm_nwservice_instance_key_info->nwservice_object_name_p);
    of_free(nsrm_nwservice_instance_key_info);
    return OF_FAILURE;
  }
   return_value = nsrm_get_exact_nwservice_instance(nschain_nwservice_object_p,nsrm_nwservice_instance_key_info ,nsrm_instance_record);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Get first record failed for NSRM service instance  Table");
      return OF_FAILURE;
   }

   result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (result_iv_pairs_p == NULL)
   {
     CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
     of_free(nsrm_instance_record->key.nwservice_instance_name_p);
     of_free(nsrm_instance_record->key.tenant_name_p);
     of_free(nsrm_instance_record->key.vm_name_p);
     of_free(nsrm_instance_record->key.nschain_object_name_p);
     of_free(nsrm_instance_record->key.nwservice_object_name_p);
     of_free(nsrm_instance_record);

     return OF_FAILURE;
   }
   nsrm_nwservice_instance_ucm_getparams(nsrm_instance_record, result_iv_pairs_p);
   *pIvPairArr = result_iv_pairs_p;
  
   of_free(nsrm_instance_record->key.nwservice_instance_name_p);
   of_free(nsrm_instance_record->key.tenant_name_p);
   of_free(nsrm_instance_record->key.vm_name_p);
   of_free(nsrm_instance_record->key.nschain_object_name_p);
   of_free(nsrm_instance_record->key.nwservice_object_name_p);
   of_free(nsrm_instance_record);
   
   return NSRM_SUCCESS;

}

int32_t nsrm_nwservice_instance_verifycfg (struct cm_array_of_iv_pairs *key_iv_pairs_p,
                        uint32_t command_id, struct cm_app_result ** result_p)
{

  return NSRM_SUCCESS;
}

