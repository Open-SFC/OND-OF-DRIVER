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
 * File name: protocolcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "of_proto.h"



int32_t of_protocol_appl_ucmcbk_init (void);

int32_t protocol_addrec (void * config_trans_p,
      struct cm_array_of_iv_pairs * mand_params,
      struct cm_array_of_iv_pairs * opt_params,
      struct cm_app_result ** result_p);


int32_t protocol_modrec (void * config_trans_p,
      struct cm_array_of_iv_pairs * mand_params,
      struct cm_array_of_iv_pairs * opt_params,
      struct cm_app_result ** result_p);


int32_t protocol_delrec (void * config_transaction_p,
      struct cm_array_of_iv_pairs * keys_arr_p,
      struct cm_app_result ** result_p);

int32_t protocol_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
      uint32_t * count_p,
      struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);

int32_t protocol_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
      struct cm_array_of_iv_pairs * prev_record_key_p, uint32_t * count_p,
      struct cm_array_of_iv_pairs ** next_n_record_data_p);

int32_t protocol_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
      struct cm_array_of_iv_pairs ** pIvPairArr);

int32_t protocol_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
      uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * P R O T O C O L  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t protocol_ucm_validatemandparams (struct cm_array_of_iv_pairs *
      params,
      struct cm_app_result **
      presult_p);

int32_t protocol_ucm_validateoptparams (struct cm_array_of_iv_pairs *
      params,
      struct cm_app_result ** presult_p);


int32_t protocol_ucm_set_mand_params (struct cm_array_of_iv_pairs *
      params,
      of_trnsprt_proto_info_t* protocol_info,
      struct cm_app_result ** presult_p);

int32_t protocol_ucm_set_opt_params (struct cm_array_of_iv_pairs *
      params,
      of_trnsprt_proto_info_t * protocol_info,
      struct cm_app_result ** presult_p);


int32_t protocol_ucm_getparams ( of_trnsprt_proto_info_t *protocol_info,
      struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks protocol_ucm_callbacks = {
   NULL,
   protocol_addrec,
   protocol_modrec,
   protocol_delrec,
   NULL,
   protocol_getfirstnrecs,
   protocol_getnextnrecs,
   protocol_getexactrec,
   protocol_verifycfg,
   NULL
};
int8_t port_no_b=FALSE;
int8_t dev_name_b=FALSE;
int8_t conn_path_b=FALSE;
char protocol_type[16];
extern char  cntlr_xprt_protocols[CNTLR_TRANSPORT_TYPE_MAX][XPRT_PROTO_MAX_STRING_LEN];
extern int32_t cntlr_xprt_protocol_id_g;
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Protocol  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_protocol_appl_ucmcbk_init (void)
{
   CM_CBK_DEBUG_PRINT ("Entry");
   cm_register_app_callbacks (CM_ON_DIRECTOR_TRANSPORT_PROTOCOL_APPL_ID,&protocol_ucm_callbacks);
   return OF_SUCCESS;
}

int32_t protocol_addrec (void * config_trans_p,
      struct cm_array_of_iv_pairs * mand_params,
      struct cm_array_of_iv_pairs * opt_params,
      struct cm_app_result ** result_p)
{
   struct cm_app_result *protocol_result = NULL;
   int32_t return_value = OF_FAILURE;
   of_trnsprt_proto_info_t protocol_info = { };

   CM_CBK_DEBUG_PRINT ("Entered");

   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));

   if ((protocol_ucm_set_mand_params (mand_params, &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
      *result_p=protocol_result;
      return OF_FAILURE;
   }

   return_value = protocol_ucm_set_opt_params (opt_params, &protocol_info, &protocol_result);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
      *result_p = protocol_result;
      return OF_FAILURE;
   }
 /* later extend it to the UCM -- Hard coding here*/
  // strcpy(protocol_info.path_name,"/path/to/name" );
  // protocol_info.transport_type = 6;

   return_value=cntlr_add_transport_proto(&protocol_info);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("add transport info Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_ADD_FAILED);
      *result_p = protocol_result;
      return OF_FAILURE;
   }

   CM_CBK_DEBUG_PRINT ("transport protocols added successfully");
   return OF_SUCCESS;
}

int32_t protocol_modrec (void * config_trans_p,
      struct cm_array_of_iv_pairs * mand_params,
      struct cm_array_of_iv_pairs * opt_params,
      struct cm_app_result ** result_p)
{
   struct cm_app_result *protocol_result = NULL;
   int32_t return_value = OF_FAILURE;
   of_trnsprt_proto_info_t protocol_info = { };

   CM_CBK_DEBUG_PRINT ("Entered");

   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));

   if ((protocol_ucm_set_mand_params (mand_params,  &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
      *result_p=protocol_result;
      return OF_FAILURE;
   }

   return_value = protocol_ucm_set_opt_params (opt_params, &protocol_info, &protocol_result);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
      *result_p = protocol_result;
      return OF_FAILURE;
   }


   return_value=cntlr_modify_transport_proto(&protocol_info);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("protocol Modification Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_ADD_FAILED);
      *result_p = protocol_result;
      return OF_FAILURE;
   }


   CM_CBK_DEBUG_PRINT ("protocol Modifed succesfully");
   return OF_SUCCESS;
}




int32_t protocol_delrec (void * config_transaction_p,
      struct cm_array_of_iv_pairs * keys_arr_p,
      struct cm_app_result ** result_p)
{
   struct cm_app_result *protocol_result = NULL;
   int32_t return_value = OF_FAILURE;
   of_trnsprt_proto_info_t protocol_info = { };

   CM_CBK_DEBUG_PRINT ("Entered");
   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));

   if ((protocol_ucm_set_mand_params (keys_arr_p, &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
      *result_p = protocol_result;
      return OF_FAILURE;
   }

   return_value=cntlr_delete_transport_proto(&protocol_info);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Delete Table record failed");
      fill_app_result_struct (&protocol_result, NULL, CM_GLU_DATAPATH_DELETE_FAILED);
      *result_p = protocol_result;
      return OF_FAILURE;
   }

   CM_CBK_DEBUG_PRINT ("protocol deleted succesfully");
   return OF_SUCCESS;
}

int32_t protocol_getfirstnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
      uint32_t * count_p,
      struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{
   struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiRecCount = 0;
   struct cm_app_result *protocol_result = NULL;
   of_trnsprt_proto_info_t protocol_info = { };

   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));
   if ((protocol_ucm_set_mand_params (keys_arr_p, &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
   }

   return_value=cntlr_get_first_transport_proto(&protocol_info);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Get first record failed for protocol Table");
      return OF_FAILURE;
   }

   result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (result_iv_pairs_p == NULL)
   {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
   }
   protocol_ucm_getparams (&protocol_info, &result_iv_pairs_p[uiRecCount]);
   uiRecCount++;
   *array_of_iv_pair_arr_p = result_iv_pairs_p;
   *count_p = uiRecCount;
   return OF_SUCCESS;
}


int32_t protocol_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
      struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
      struct cm_array_of_iv_pairs ** next_n_record_data_p)
{

   struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   struct cm_app_result *protocol_result = NULL;
   int32_t return_value = OF_FAILURE;
   uint32_t uiRecCount = 0;
   of_trnsprt_proto_info_t protocol_info={};

   CM_CBK_DEBUG_PRINT ("Entered");
   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));
   result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (result_iv_pairs_p == NULL)
   {
      CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
      return OF_FAILURE;
   }

   if ((protocol_ucm_set_mand_params (keys_arr_p, &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
   }

   if ((protocol_ucm_set_mand_params (prev_record_key_p, &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
   }

   return_value=cntlr_get_next_transport_proto(&protocol_info);
   if (return_value == OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Next protocol  record is : %d",
            protocol_info.id);
      protocol_ucm_getparams (&protocol_info, &result_iv_pairs_p[uiRecCount]);
      uiRecCount++;
   }

   CM_CBK_DEBUG_PRINT ("Number of records requested were %ld ", *count_p);
   CM_CBK_DEBUG_PRINT ("Number of records found are %ld", uiRecCount+1);
   *next_n_record_data_p = result_iv_pairs_p;
   *count_p = uiRecCount;
   return OF_SUCCESS;
}

int32_t protocol_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
      struct cm_array_of_iv_pairs ** pIvPairArr)
{
   struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
   struct cm_app_result *protocol_result = NULL;
   uint32_t ii = 0;
   int32_t return_value = OF_FAILURE;
   of_trnsprt_proto_info_t protocol_info={};

   CM_CBK_DEBUG_PRINT ("Entered");
   result_iv_pairs_p =
      (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
   if (result_iv_pairs_p == NULL)
   {
      CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
      return OF_FAILURE;
   }

   if ((protocol_ucm_set_mand_params (keys_arr_p, &protocol_info, &protocol_result)) !=
         OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
      return OF_FAILURE;
   }


   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));
   return_value=cntlr_get_exact_transport_proto(&protocol_info);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Error: get dprm table handle failed");
      return OF_FAILURE;
   }

   CM_CBK_DEBUG_PRINT ("Exact matching record found");
   protocol_ucm_getparams (&protocol_info, result_iv_pairs_p);
   *pIvPairArr = result_iv_pairs_p;
   return return_value;
}

int32_t protocol_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
      uint32_t command_id, struct cm_app_result ** result_p)
{
   struct cm_app_result *protocol_result = NULL;
   int32_t return_value = OF_FAILURE;
   of_trnsprt_proto_info_t protocol_info = { };

   CM_CBK_DEBUG_PRINT ("Entered");
   of_memset (&protocol_info, 0, sizeof (of_trnsprt_proto_info_t));

   port_no_b=FALSE;
   dev_name_b=FALSE;
   conn_path_b=FALSE;	
   memset(protocol_type,0,16);
   return_value = protocol_ucm_validatemandparams (keys_arr_p, &protocol_result);
   if (return_value != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
      *result_p = protocol_result;
      return OF_FAILURE;
   }

   if (protocol_ucm_validateoptparams (keys_arr_p, &protocol_result)
         != OF_SUCCESS)
   {
      CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
      *result_p = protocol_result;
      return OF_FAILURE;
   }

   if (strcmp(protocol_type,"VIRTIO") && (port_no_b == FALSE ))
   {
      CM_CBK_DEBUG_PRINT ("port number not set ");
      return OF_FAILURE;
   }

   if (!strcmp(protocol_type,"VIRTIO") && (dev_name_b == FALSE ))
   {
      CM_CBK_DEBUG_PRINT ("device name not set ");
      return OF_FAILURE;
   }
   if (!strcmp(protocol_type,"UNIXDM_TCP") && (conn_path_b == FALSE ))
   {
      CM_CBK_DEBUG_PRINT ("Connection Path  not set ");
      return OF_FAILURE;
   }
   *result_p = protocol_result;
   return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * P R O T O C O L  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t protocol_ucm_validatemandparams (struct cm_array_of_iv_pairs *
      params,   struct cm_app_result **presult_p)
{
   uint32_t count;
   struct cm_app_result *protocol_result = NULL;
   uint32_t switch_type, uiTableCnt;
   uint64_t uldpId;
   int32_t id;

   CM_CBK_DEBUG_PRINT ("Entered");
   for (count = 0; count < params->count_ui;
         count++)
   {
      switch (params->iv_pairs[count].id_ui)
      {
         case CM_DM_PROTOCOL_TYPE_ID:
            if (params->iv_pairs[count].value_p == NULL)
            {
               CM_CBK_DEBUG_PRINT ("Protocol Type is NULL");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
            of_strncpy (protocol_type,
                  (char *) params->iv_pairs[count].value_p,
                  params->iv_pairs[count].value_length);
            break;

         case CM_DM_PROTOCOL_ID_ID: 
            if (params->iv_pairs[count].value_p == NULL)
            {
               CM_CBK_DEBUG_PRINT ("Protocol ID is NULL");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_DATAPATH_ID_NULL);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
            id=of_atoi(params->iv_pairs[count].value_p);
            if (id  < 0 || id > 255)
            {
               CM_CBK_DEBUG_PRINT ("Invalid protocol ID");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_CNTLR_XPRT_INVALID_PROTOCOL_ID);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
	    if(id <= cntlr_xprt_protocol_id_g){
               CM_CBK_DEBUG_PRINT ("Invalid protocol ID cntlr_xprt_protocol_id_g is %d",cntlr_xprt_protocol_id_g);
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_CNTLR_XPRT_INVALID_PROTOCOL_ID);
               *presult_p = protocol_result;
               return OF_FAILURE;
		
	    }
            break;
      }
   }
   CM_CBK_PRINT_IVPAIR_ARRAY (params);
   *presult_p = protocol_result;
   return OF_SUCCESS;
}

int32_t protocol_ucm_validateoptparams (struct cm_array_of_iv_pairs *
      params,
      struct cm_app_result ** presult_p)
{
   uint32_t count;
   struct cm_app_result * protocol_result;

   CM_CBK_DEBUG_PRINT ("Entered");
   for (count = 0; count < params->count_ui; count++)
   {
      switch (params->iv_pairs[count].id_ui)
      {
         case CM_DM_PROTOCOL_PORT_NO_ID:
            if (params->iv_pairs[count].value_p == NULL)
            {
               CM_CBK_DEBUG_PRINT ("Protocol Type is NULL");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
            port_no_b=TRUE;
            break;
#if 0
         case CM_DM_PROTOCOL_SOCKET_PROTO_ID:
            if (params->iv_pairs[count].value_p == NULL)
            {
               CM_CBK_DEBUG_PRINT ("Protocol Type is NULL");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
            break;
#endif
         case CM_DM_PROTOCOL_DEV_NAME_ID:
            if (params->iv_pairs[count].value_p == NULL)
            {
               CM_CBK_DEBUG_PRINT ("Protocol Type is NULL");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
            dev_name_b=TRUE;
            break;
	 case CM_DM_PROTOCOL_CONN_PATH_ID:
            if (params->iv_pairs[count].value_p == NULL)
            {
               CM_CBK_DEBUG_PRINT ("Conn path  is NULL");
               fill_app_result_struct (&protocol_result, NULL, CM_GLU_DOMAIN_NAME_NULL);
               *presult_p = protocol_result;
               return OF_FAILURE;
            }
            conn_path_b=TRUE;
            break;

         default:
            break;
      }
   }
   CM_CBK_PRINT_IVPAIR_ARRAY (params);
   return OF_SUCCESS;
}

int32_t protocol_ucm_set_mand_params (struct cm_array_of_iv_pairs *
      params,
      of_trnsprt_proto_info_t* protocol_info,     
      struct cm_app_result ** presult_p)
{
   uint32_t count;
   uint32_t switch_type;
   int32_t id;
   int32_t type;
   CM_CBK_DEBUG_PRINT ("Entered");
   for (count = 0; count < params->count_ui;
         count++)
   {
      switch (params->iv_pairs[count].id_ui)
      {
         case CM_DM_PROTOCOL_TYPE_ID:
            {
               CNTLR_XPRT_GET_PROTO_TRANSPORT_TYPE((char *) params->iv_pairs[count].value_p,type);
               protocol_info->transport_type=type;
            }
            break;

         case CM_DM_PROTOCOL_ID_ID:
            {
               id= of_atoi(params->iv_pairs[count].value_p);
               protocol_info->id =id;
            }
            break;
      }
   }
   CM_CBK_PRINT_IVPAIR_ARRAY (params);
   return OF_SUCCESS;
}

int32_t protocol_ucm_set_opt_params (struct cm_array_of_iv_pairs *
      params,
      of_trnsprt_proto_info_t * protocol_info,
      struct cm_app_result ** presult_p)
{
   uint32_t count;

   CM_CBK_DEBUG_PRINT ("Entered");

   for (count = 0; count < params->count_ui; count++)
   {
      switch (params->iv_pairs[count].id_ui)
      {
         case CM_DM_PROTOCOL_PORT_NO_ID:
            protocol_info->port_number = of_atoi(params->iv_pairs[count].value_p);   
            break;
#if 0
         case CM_DM_PROTOCOL_SOCKET_PROTO_ID:
            protocol_info->proto = of_atoi(params->iv_pairs[count].value_p);   
            break;
#endif
         case CM_DM_PROTOCOL_DEV_NAME_ID:
            of_strncpy (protocol_info->dev_name,
                  (char *) params->iv_pairs[count].value_p,
                  params->iv_pairs[count].value_length);
            break;
	 case CM_DM_PROTOCOL_CONN_PATH_ID:
            of_strncpy (protocol_info->path_name,
                  (char *) params->iv_pairs[count].value_p,
                  params->iv_pairs[count].value_length);
            break;
         default:
            break;
      }
   }

   CM_CBK_PRINT_IVPAIR_ARRAY (params);
   return OF_SUCCESS;
}



int32_t protocol_ucm_getparams (of_trnsprt_proto_info_t *protocol_info,
      struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
   uint32_t uindex_i = 0;
   char buf[64] = "";
   int32_t transport_type;

#define CM_PROTOCOL_CHILD_COUNT 5

   result_iv_pairs_p->iv_pairs =
      (struct cm_iv_pair *) of_calloc (CM_PROTOCOL_CHILD_COUNT, sizeof (struct cm_iv_pair));
   if (result_iv_pairs_p->iv_pairs == NULL)
   {
      CM_CBK_DEBUG_PRINT
         ("Memory allocation failed for result_iv_pairs_p->iv_pairs");
      return OF_FAILURE;
   }

   /* id */
   of_itoa(protocol_info->id,buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i],CM_DM_PROTOCOL_ID_ID,
         CM_DATA_TYPE_STRING, buf);
   uindex_i++;

   /* transport type */
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PROTOCOL_TYPE_ID,
         CM_DATA_TYPE_STRING, cntlr_xprt_protocols[protocol_info->transport_type]);
   uindex_i++;

#if 0
   /* protocol*/
   CNTLR_XPRT_GET_PROTO_NAME(protocol_info->proto,buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PROTOCOL_SOCKET_PROTO_ID,
         CM_DATA_TYPE_STRING, buf);
   uindex_i++;
#endif

   /* port number*/
  
   if ( protocol_info->port_number != 0){
   of_itoa(protocol_info->port_number,buf);
   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PROTOCOL_PORT_NO_ID,
         CM_DATA_TYPE_STRING, buf);
   uindex_i++;
   }

   if ( protocol_info->dev_name[0] != '\0')
   { 
	   /* device name*/
	   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PROTOCOL_DEV_NAME_ID,
			   CM_DATA_TYPE_STRING, protocol_info->dev_name);
	   uindex_i++;
   }
   if ( protocol_info->path_name[0] != '\0')
   { 
	   /* conn path_name*/
	   FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PROTOCOL_CONN_PATH_ID,
			   CM_DATA_TYPE_STRING, protocol_info->path_name);
	   uindex_i++;
   }

   result_iv_pairs_p->count_ui = uindex_i;
   return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
