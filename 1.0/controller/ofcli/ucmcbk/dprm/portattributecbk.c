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
 * File name: portcbk.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"


int32_t dprm_datapath_port_attribute_appl_ucmcbk_init (void);


int32_t dprm_datapath_port_attribute_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** result_p);


int32_t dprm_datapath_port_attribute_modrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** result_p);



int32_t dprm_datapath_port_attribute_delrec (void * config_transaction_p,
		struct cm_array_of_iv_pairs * keys_arr_p,
		struct cm_app_result ** result_p);


int32_t dprm_datapath_port_attribute_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
		uint32_t command_id, struct cm_app_result ** result_p);


int32_t dprm_datapath_port_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
		uint32_t * count_p,
		struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p);



int32_t dprm_datapath_port_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
		struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
		struct cm_array_of_iv_pairs ** next_n_record_data_p);


int32_t dprm_get_datapath_port_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
		struct cm_array_of_iv_pairs ** pIvPairArr);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * dpattribute  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t dprm_datapath_port_attribute_ucm_validatemandparams (struct cm_array_of_iv_pairs *pMandParams,
		struct cm_app_result **presult_p);

 int32_t dprm_datapath_port_attribute_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p);



int32_t dprm_datapath_port_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
		struct   dprm_port_info  		  *of_port_info_p,
		struct   dprm_datapath_general_info       *of_datapath_info_p,
		struct   dprm_attribute_name_value_pair   *of_attribute_info_p,
		struct   cm_app_result             	  **presult_p);


int32_t dprm_datapath_port_attribute_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
		struct dprm_port_info                   *of_port_info_p,
                struct dprm_datapath_general_info       *of_datapath_info_p,
		struct dprm_attribute_name_value_pair   *of_attribute_info_p,
		struct cm_app_result                   **presult_p);




struct cm_dm_app_callbacks dprm_datapath_port_attribute_ucm_callbacks = {
	NULL,
	dprm_datapath_port_attribute_addrec,
	dprm_datapath_port_attribute_modrec,
	dprm_datapath_port_attribute_delrec,
	NULL,
	dprm_datapath_port_attribute_getfirstnrecs,
	dprm_datapath_port_attribute_getnextnrecs,
	dprm_get_datapath_port_attribute_getexactrec,
	dprm_datapath_port_attribute_verifycfg,
	NULL
};

int32_t dprm_datapath_port_attribute_appl_ucmcbk_init (void)
{
	CM_CBK_DEBUG_PRINT ("Entry");
	cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_PORT_PORTATTRIBUTE_APPL_ID, 
				   &dprm_datapath_port_attribute_ucm_callbacks);
	return OF_SUCCESS;
}

int32_t dprm_datapath_port_attribute_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** result_p)
{
	struct   cm_app_result                    *of_attribute_result_p = NULL;
	struct   dprm_attribute_name_value_pair   of_dprm_attribute_name_value_info={};
	struct   dprm_port_info                   of_port_info = { };
	struct   dprm_datapath_general_info       of_datapath_info={};

	uint64_t datapath_handle,port_handle;
	int32_t  return_value = OF_FAILURE;

        uint32_t no_of_namespaces_count;
        uint8_t multi_namespaces_flag;

	CM_CBK_DEBUG_PRINT ("Entered");

	of_memset (&of_dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));


	if ((dprm_datapath_port_attribute_ucm_setmandparams (pMandParams, 
					&of_port_info, 
					&of_datapath_info,
					&of_dprm_attribute_name_value_info, 
					&of_attribute_result_p)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_p=of_attribute_result_p;
		return OF_FAILURE;
	}


	return_value = dprm_datapath_port_attribute_ucm_setoptparams (pOptParams, 
			&of_port_info, &of_datapath_info,
			&of_dprm_attribute_name_value_info, 
			&of_attribute_result_p);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
		*result_p = of_attribute_result_p;
		return OF_FAILURE;
	}

	 return_value=dprm_get_datapath_handle(of_datapath_info.dpid, &datapath_handle);
         if (return_value != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Error:Get DPRM Handle failed ");
                        fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_DATAPATH_ID_INVALID);
                        *result_p = of_attribute_result_p;
                        return OF_FAILURE;
         }

         return_value=dprm_get_port_handle(datapath_handle, 
					  of_port_info.port_name, 
					  &port_handle);
         if (return_value != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", of_port_info.port_name);
                        fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
                        *result_p = of_attribute_result_p;
                        return OF_FAILURE;
         }


	if(!strcmp(of_dprm_attribute_name_value_info.name_string, "namespace"))
         {
                 return_value = dprm_get_datapath_port_namespace_count(of_datapath_info.dpid, of_port_info.port_name,&no_of_namespaces_count);
                 if (return_value != OF_SUCCESS)
                 {
                         CM_CBK_DEBUG_PRINT ("Get datapath namespaces count failed!");
                         fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GROUP_DATAPATH_GET_NAMESPACE_COUNT_FAIL);
                         *result_p = of_attribute_result_p;
                         return OF_FAILURE;
                 }

                 return_value = dprm_get_datapath_multinamespaces(of_datapath_info.dpid, &multi_namespaces_flag);
                 if (return_value != OF_SUCCESS)
                 {
                         CM_CBK_DEBUG_PRINT ("Get datapath namespaces flag failed !");
                         fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GROUP_DATAPATH_GET_NAMESPACE_FLAG_FAIL);
                         *result_p = of_attribute_result_p;
                         return OF_FAILURE;
                 }


                if((multi_namespaces_flag==1)&&(no_of_namespaces_count>0))
                {
                         CM_CBK_DEBUG_PRINT ("Multiplenamespaces enabled and Already namespaces added!");
                         fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GROUP_DATAPATH_NAMESPACE_ENABLED);
                         *result_p = of_attribute_result_p;
                         return OF_FAILURE;
                }
         }




	return_value = dprm_add_attribute_to_datapath_port(datapath_handle,
							   port_handle, 
							  &of_dprm_attribute_name_value_info);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Add Attribute To Datapath Port Failed");
                fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_ATTRIBUTE_ADD_FAILED);
		*result_p = of_attribute_result_p;
		return OF_FAILURE;
	}

	CM_CBK_DEBUG_PRINT ("Attribute added to datapath port succesfully");
	return OF_SUCCESS;

}
/******************************************************************************/

int32_t dprm_datapath_port_attribute_modrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * pMandParams,
		struct cm_array_of_iv_pairs * pOptParams,
		struct cm_app_result ** result_p)
{
	struct   cm_app_result                    *of_attribute_result_p = NULL;
	struct   dprm_attribute_name_value_pair   of_dprm_attribute_name_value_info={};
	struct   dprm_port_info                   of_port_info = { };
	struct   dprm_datapath_general_info       of_datapath_info={};

	uint64_t datapath_handle,port_handle;
	int32_t  return_value = OF_FAILURE;
        uint32_t no_of_namespaces_count;
        uint8_t multi_namespaces_flag;

	CM_CBK_DEBUG_PRINT ("Entered");

	of_memset (&of_dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));


	if ((dprm_datapath_port_attribute_ucm_setmandparams (pMandParams, 
					&of_port_info, &of_datapath_info, 
					&of_dprm_attribute_name_value_info, 
					&of_attribute_result_p)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_p=of_attribute_result_p;
		return OF_FAILURE;
	}


	return_value = dprm_datapath_port_attribute_ucm_setoptparams (pOptParams, 
			&of_port_info, &of_datapath_info,
			&of_dprm_attribute_name_value_info, 
			&of_attribute_result_p);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_OPT_PARAM_FAILED);
		*result_p = of_attribute_result_p;
		return OF_FAILURE;
	}

	return_value=dprm_get_datapath_handle(of_datapath_info.dpid, &datapath_handle);
         if (return_value != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Error:Get DPRM Handle failed ");
                        fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_DATAPATH_ID_INVALID);
                        *result_p = of_attribute_result_p;
                        return OF_FAILURE;
         }

         return_value=dprm_get_port_handle(datapath_handle, 
					  of_port_info.port_name,
					  &port_handle);
         if (return_value != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", of_port_info.port_name);
                        fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
                        *result_p = of_attribute_result_p;
                        return OF_FAILURE;
         }



	if(!strcmp(of_dprm_attribute_name_value_info.name_string, "namespace"))
         {
                 return_value = dprm_get_datapath_port_namespace_count(of_datapath_info.dpid, of_port_info.port_name,&no_of_namespaces_count);
                 if (return_value != OF_SUCCESS)
                 {
                         CM_CBK_DEBUG_PRINT ("Get datapath namespaces count failed!");
                         fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GROUP_DATAPATH_GET_NAMESPACE_COUNT_FAIL);
                         *result_p = of_attribute_result_p;
                         return OF_FAILURE;
                 }

                 return_value = dprm_get_datapath_multinamespaces(of_datapath_info.dpid, &multi_namespaces_flag);
                 if (return_value != OF_SUCCESS)
                 {
                         CM_CBK_DEBUG_PRINT ("Get datapath namespaces flag failed !");
                         fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GROUP_DATAPATH_GET_NAMESPACE_FLAG_FAIL);
                         *result_p = of_attribute_result_p;
                         return OF_FAILURE;
                 }


                if((multi_namespaces_flag==1)&&(no_of_namespaces_count>0))
                {
                         CM_CBK_DEBUG_PRINT ("Multiplenamespaces enabled and Already namespaces added!");
                         fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GROUP_DATAPATH_NAMESPACE_ENABLED);
                         *result_p = of_attribute_result_p;
                         return OF_FAILURE;
                }
         }



	return_value = dprm_add_attribute_to_datapath_port(datapath_handle,
							   port_handle, 
							   &of_dprm_attribute_name_value_info);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT (" Datapath Port Attribute Modification Failed!");
                fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_ATTRIBUTE_MOD_FAILED);
		*result_p = of_attribute_result_p;
		return OF_FAILURE;
	}

	CM_CBK_DEBUG_PRINT ("Datapath Port Attribute Modified Succesfully");
	return OF_SUCCESS;
}


/******************************************************************************/
int32_t dprm_datapath_port_attribute_getnextnrecs (struct cm_array_of_iv_pairs * keys_arr_p,
		struct cm_array_of_iv_pairs *prev_record_key_p, uint32_t * count_p,
		struct cm_array_of_iv_pairs ** next_n_record_data_p)
{
	struct   cm_array_of_iv_pairs                    *result_iv_pairs_p = NULL;
	struct   dprm_attribute_name_value_pair          of_dprm_attribute_name_value_info={};
	struct   dprm_attribute_name_value_output_info   dprm_attribute_output={};
	struct   dprm_port_info                          of_port_info = { };
	struct   dprm_datapath_general_info              of_datapath_info={};
	struct   cm_app_result                           *of_attribute_result_p = NULL;


	char     current_name_string[DPRM_MAX_ATTRIBUTE_NAME_LEN];
	char     current_value_string[DPRM_MAX_ATTRIBUTE_VALUE_LEN];
	uint64_t datapath_handle,port_handle;
	int32_t  return_value = OF_FAILURE;

	uint32_t uiRecCount = 0;
	uint32_t uindex_i = 0;

	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&of_dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));

	if ((dprm_datapath_port_attribute_ucm_setmandparams (keys_arr_p, 
					&of_port_info, 
					&of_datapath_info,
					&of_dprm_attribute_name_value_info, 
					&of_attribute_result_p)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		return OF_FAILURE;
	}




 	 return_value = dprm_get_datapath_handle(of_datapath_info.dpid, &datapath_handle);
         if (return_value != OF_SUCCESS)
         {
		CM_CBK_DEBUG_PRINT ("Error:Get DPRM Handle failed ");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_DATAPATH_ID_INVALID);
		return OF_FAILURE;
         }

	 return_value = dprm_get_port_handle(datapath_handle, of_port_info.port_name, &port_handle);
         if (return_value != OF_SUCCESS)
         {
		CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", of_port_info.port_name);
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
		return OF_FAILURE;
         }



	strcpy(current_name_string,  prev_record_key_p->iv_pairs[uindex_i].value_p);
	strcpy(current_value_string, prev_record_key_p->iv_pairs[uindex_i+1].value_p);

	strcpy(dprm_attribute_output.name_string, current_name_string);

	dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
	dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;

	return_value = dprm_get_datapath_port_attribute_next_value(datapath_handle, 
								port_handle, 
								current_name_string,
								current_value_string, 
								&dprm_attribute_output);
	if (return_value != OF_SUCCESS)
	{
		do
		{
			dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
			dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;

			return_value = dprm_get_datapath_port_next_attribute_name(datapath_handle,
									          port_handle, 
								  	          dprm_attribute_output.name_string,
									          &dprm_attribute_output);
			if (return_value != OF_SUCCESS)
			{
				CM_CBK_DEBUG_PRINT ("dprm_get_first_datapath_attribute_name failed!");
				return OF_FAILURE;
			}

			return_value = dprm_get_datapath_port_attribute_first_value(datapath_handle, 
     			            port_handle,  &dprm_attribute_output);
		}
		while(return_value!=OF_SUCCESS);

	}

	if (return_value != OF_SUCCESS)
	{
		return OF_FAILURE;
	}

	result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	if (result_iv_pairs_p == NULL)
	{
		CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
		return OF_FAILURE;
	}
	dprm_datapath_port_attribute_getparams(&dprm_attribute_output, &result_iv_pairs_p[uiRecCount]);
	uiRecCount++;
	*next_n_record_data_p = result_iv_pairs_p;
	*count_p = uiRecCount;	
	return DPRM_SUCCESS;  
}


/****************************************************************************************/
int32_t dprm_datapath_port_attribute_getfirstnrecs(struct cm_array_of_iv_pairs * keys_arr_p,
						uint32_t * count_p,
						struct cm_array_of_iv_pairs ** array_of_iv_pair_arr_p)
{

	struct   cm_array_of_iv_pairs                 *result_iv_pairs_p = NULL;
	struct   dprm_attribute_name_value_pair        of_dprm_attribute_name_value_info={};
	struct   dprm_attribute_name_value_output_info of_dprm_attribute_output={};
	struct   dprm_port_info                        of_port_info = {};
	struct   dprm_datapath_general_info            of_datapath_info={};
	struct   cm_app_result                        *of_attribute_result_p = NULL;

	uint64_t datapath_handle,port_handle;
	int32_t  return_value = OF_FAILURE;
	uint32_t uiRecCount = 0;

	CM_CBK_DEBUG_PRINT ("Entered");


	if ((dprm_datapath_port_attribute_ucm_setmandparams (keys_arr_p, 
					&of_port_info, &of_datapath_info, 
					&of_dprm_attribute_name_value_info, 
					&of_attribute_result_p)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		return OF_FAILURE;
	}




 	 return_value = dprm_get_datapath_handle(of_datapath_info.dpid, &datapath_handle);
         if (return_value != OF_SUCCESS)
         {
		CM_CBK_DEBUG_PRINT ("Error:Get DPRM Handle failed ");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_DATAPATH_ID_INVALID);
		return OF_FAILURE;
         }

	 return_value = dprm_get_port_handle(datapath_handle, of_port_info.port_name, &port_handle);
         if (return_value != OF_SUCCESS)
         {
		CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", of_port_info.port_name);
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
		return OF_FAILURE;
         }



  	 of_dprm_attribute_output.name_length = DPRM_MAX_ATTRIBUTE_NAME_LEN -1;
	 of_dprm_attribute_output.value_length = DPRM_MAX_ATTRIBUTE_VALUE_LEN -1;

	 return_value = dprm_get_datapath_port_first_attribute_name(datapath_handle,
								    port_handle, 
								    &of_dprm_attribute_output);
	 if (return_value != OF_SUCCESS)
	 {
		CM_CBK_DEBUG_PRINT ("Get Datapath-port first attribute name failed!");
                fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_GET_FIRST_ATTRIBUTE_NAME_FAILED);
		return OF_FAILURE;
	 }

 	 return_value = dprm_get_datapath_port_attribute_first_value(datapath_handle, 
								     port_handle, 
								     &of_dprm_attribute_output);
	 if(return_value == OF_SUCCESS)
	 { 
		result_iv_pairs_p = (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
		if (result_iv_pairs_p == NULL)
		{
			CM_CBK_DEBUG_PRINT ("Memory allocation failed for result_iv_pairs_p");
			return OF_FAILURE;
		}
		dprm_datapath_port_attribute_getparams(&of_dprm_attribute_output, &result_iv_pairs_p[uiRecCount]);
		uiRecCount++;
		*array_of_iv_pair_arr_p = result_iv_pairs_p;
		*count_p = uiRecCount;  
	}
	return DPRM_SUCCESS;  
}



/****************************************************************************************/
int32_t dprm_get_datapath_port_attribute_getexactrec (struct cm_array_of_iv_pairs * keys_arr_p,
		struct cm_array_of_iv_pairs ** pIvPairArr)

{
	struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
	int32_t  return_value = OF_FAILURE;
	struct   dprm_attribute_name_value_pair        of_dprm_attribute_name_value_info={};
	struct   dprm_attribute_name_value_output_info of_dprm_attribute_output={};
	struct   dprm_port_info                        of_port_info = { };
	struct   dprm_datapath_general_info            of_datapath_info={};
	struct   cm_app_result *of_attribute_result_p = NULL;
	

	uint64_t datapath_handle,port_handle;

	CM_CBK_DEBUG_PRINT ("Entered");


	of_memset (&of_dprm_attribute_name_value_info, 0, sizeof (struct dprm_attribute_name_value_pair));

	if ((dprm_datapath_port_attribute_ucm_setmandparams (keys_arr_p, 
					&of_port_info, 
					&of_datapath_info, 
					&of_dprm_attribute_name_value_info, 
					&of_attribute_result_p)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		return OF_FAILURE;
	}




 	 return_value = dprm_get_datapath_handle(of_datapath_info.dpid, &datapath_handle);
         if (return_value != OF_SUCCESS)
         {
		CM_CBK_DEBUG_PRINT ("Error:Get DPRM Handle failed ");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_DATAPATH_ID_INVALID);
		return OF_FAILURE;
         }

	 return_value = dprm_get_port_handle(datapath_handle, of_port_info.port_name, &port_handle);
         if (return_value != OF_SUCCESS)
         {
		CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", of_port_info.port_name);
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
		return OF_FAILURE;
         }



 
	strcpy(of_dprm_attribute_output.name_string, of_dprm_attribute_name_value_info.name_string);
	of_dprm_attribute_output.name_length = strlen(of_dprm_attribute_output.name_string);

	return_value = dprm_get_datapath_port_exact_attribute_name(datapath_handle, 
								   port_handle,
								   of_dprm_attribute_output.name_string);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Unable to find  exact attribute record!");
		return OF_FAILURE;
	}
	result_iv_pairs_p =
		(struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
	if (result_iv_pairs_p == NULL)
	{
		CM_CBK_DEBUG_PRINT ("Failed to allocate memory for result_iv_pairs_p");
		return OF_FAILURE;
	}


	dprm_datapath_port_attribute_getparams (&of_dprm_attribute_name_value_info, result_iv_pairs_p);
	*pIvPairArr = result_iv_pairs_p;
	return return_value;
}

int32_t dprm_datapath_port_attribute_getparams (struct dprm_attribute_name_value_output_info *attribute_info_p,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
	uint32_t uindex_i = 0;
	char buf[64] = "";

	CM_CBK_DEBUG_PRINT ("Entered ");
	result_iv_pairs_p->iv_pairs = (struct cm_iv_pair *) of_calloc (2, sizeof (struct cm_iv_pair));
	if (result_iv_pairs_p->iv_pairs == NULL)
	{
		CM_CBK_DEBUG_PRINT("Memory allocation failed for result_iv_pairs_p->iv_pairs");
		return OF_FAILURE;
	}

	FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PORTATTRIBUTE_ATTRIBUTENAME_ID,
			CM_DATA_TYPE_STRING, attribute_info_p->name_string);
	uindex_i++;

	FILL_CM_IV_PAIR (result_iv_pairs_p->iv_pairs[uindex_i], CM_DM_PORTATTRIBUTE_ATTRIBUTEVAL_ID,
			CM_DATA_TYPE_STRING, attribute_info_p->value_string);
	uindex_i++;

	result_iv_pairs_p->count_ui = uindex_i;
	return OF_SUCCESS;   

}


int32_t dprm_datapath_port_attribute_ucm_setmandparams (struct cm_array_of_iv_pairs *pMandParams,
		struct   dprm_port_info  		  *of_port_info_p,
		struct   dprm_datapath_general_info       *of_datapath_info_p,
		struct   dprm_attribute_name_value_pair   *of_attribute_info_p,
		struct   cm_app_result             	  **presult_p)
{
	uint32_t uiMandParamCnt;
	uint64_t idpId;
        int32_t iportId;


	CM_CBK_DEBUG_PRINT ("Entered");
	for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
			uiMandParamCnt++)
	{
		switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
		{
			case CM_DM_DATAPATH_DATAPATHID_ID:
				idpId=charTo64bitNum((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
				CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
				of_datapath_info_p->dpid=idpId;
				break;

			case CM_DM_PORT_PORTNAME_ID:
				of_strncpy (of_port_info_p->port_name,
					    (char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
                                            pMandParams->iv_pairs[uiMandParamCnt].value_length);
				CM_CBK_DEBUG_PRINT ("port name %s",of_port_info_p->port_name);
				break;

			case CM_DM_PORT_PORTID_ID:
				iportId=of_atoi((char *) pMandParams->iv_pairs[uiMandParamCnt].value_p);
				of_port_info_p->port_id=iportId;
				CM_CBK_DEBUG_PRINT ("port Id %d",of_port_info_p->port_id);
				break;


			case CM_DM_PORTATTRIBUTE_ATTRIBUTENAME_ID:
				of_strncpy (of_attribute_info_p->name_string,
						(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
						pMandParams->iv_pairs[uiMandParamCnt].value_length);
				CM_CBK_DEBUG_PRINT ("Attribute name %s",of_attribute_info_p->name_string);
				break;

			case CM_DM_PORTATTRIBUTE_ATTRIBUTEVAL_ID:
				of_strncpy (of_attribute_info_p->value_string,
						(char *) pMandParams->iv_pairs[uiMandParamCnt].value_p,
						pMandParams->iv_pairs[uiMandParamCnt].value_length);
				CM_CBK_DEBUG_PRINT ("Attribute value %s",of_attribute_info_p->value_string);
				break;
		}
	}
	CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	return OF_SUCCESS;
}

int32_t dprm_datapath_port_attribute_ucm_setoptparams (struct cm_array_of_iv_pairs *pOptParams,
		struct dprm_port_info                   *of_port_info_p,
                struct dprm_datapath_general_info       *of_datapath_info_p,
		struct dprm_attribute_name_value_pair   *of_attribute_info_p,
		struct cm_app_result                   **presult_p)
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

/*******************************************************************************/
int32_t dprm_datapath_port_attribute_delrec (void * config_transaction_p,
		struct cm_array_of_iv_pairs * keys_arr_p,
		struct cm_app_result ** result_p)
{
	struct   cm_app_result *of_attribute_result_p = NULL;
	int32_t  return_value = OF_FAILURE;
	struct   dprm_attribute_name_value_pair of_attribute_info_p={};
	struct   dprm_port_info                 of_port_info = { };
	struct   dprm_datapath_general_info     of_datapath_info={};

	uint64_t datapath_handle, port_handle;

	CM_CBK_DEBUG_PRINT ("Entered");
	of_memset (&of_attribute_info_p, 0, sizeof (struct dprm_attribute_name_value_pair));

	if ((dprm_datapath_port_attribute_ucm_setmandparams (keys_arr_p, 
							     &of_port_info,
							     &of_datapath_info,
							     &of_attribute_info_p, 
							     &of_attribute_result_p)) != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_p = of_attribute_result_p;
		return OF_FAILURE;
	}

	 return_value=dprm_get_datapath_handle(of_datapath_info.dpid, &datapath_handle);
         if (return_value != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Error:Get DPRM Handle failed ");
                        fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_DATAPATH_ID_INVALID);
                        *result_p = of_attribute_result_p;
                        return OF_FAILURE;
         }

         return_value=dprm_get_port_handle(datapath_handle, 
					  of_port_info.port_name, 
					  &port_handle);
         if (return_value != OF_SUCCESS)
         {
                        CM_CBK_DEBUG_PRINT ("Error: get dprm port handle failed for %s", of_port_info.port_name);
                        fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_NO_MATCHING_PORT_RECORD);
                        *result_p = of_attribute_result_p;
                        return OF_FAILURE;
         }


	return_value= dprm_delete_attribute_from_datapath_port(datapath_handle, 
								port_handle,  
								&of_attribute_info_p);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Delete datapath-port attribute record failed");
                fill_app_result_struct (&of_attribute_result_p, NULL, CM_GLU_ATTRIBUTE_DEL_FAILED);
		*result_p = of_attribute_result_p;
		return OF_FAILURE;
	}

	CM_CBK_DEBUG_PRINT ("Datapath-port attribute deleted succesfully");
	return OF_SUCCESS;
}
/*************************************************************************************/
int32_t dprm_datapath_port_attribute_verifycfg (struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t command_id, struct cm_app_result ** result_p)
{
	 struct cm_app_result *port_result = NULL;
	 int32_t return_value = OF_FAILURE;
	 struct dprm_port_info port_info = { };

	 CM_CBK_DEBUG_PRINT ("Entered");
	 of_memset (&port_info, 0, sizeof (struct dprm_port_info));

	 return_value = dprm_datapath_port_attribute_ucm_validatemandparams (keys_arr_p, &port_result);
	 if (return_value != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 if (dprm_datapath_port_attribute_ucm_validateoptparams (keys_arr_p, &port_result)
				 != OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
			return OF_FAILURE;
	 }

	 *result_p = port_result;
	 return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

 int32_t dprm_datapath_port_attribute_ucm_validatemandparams (struct cm_array_of_iv_pairs *
			pMandParams,	struct cm_app_result **presult_p)
{
	 uint32_t uiMandParamCnt;
	 struct cm_app_result *port_result = NULL;
	 uint32_t uiPortId;


	 CM_CBK_DEBUG_PRINT ("Entered");
	 for (uiMandParamCnt = 0; uiMandParamCnt < pMandParams->count_ui;
				 uiMandParamCnt++)
	 {
			switch (pMandParams->iv_pairs[uiMandParamCnt].id_ui)
			{
				 case CM_DM_PORT_PORTNAME_ID:
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Port Name is NULL");
							 fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_NAME_NULL);
							 *presult_p = port_result;
							 return OF_FAILURE;
						}
						break;

				 case CM_DM_PORT_PORTID_ID: 
						if (pMandParams->iv_pairs[uiMandParamCnt].value_p == NULL)
						{
							 CM_CBK_DEBUG_PRINT ("Port ID is NULL");
							 fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_ID_NULL);
							 *presult_p = port_result;
							 return OF_FAILURE;
						}
						uiPortId =
							 of_atoi ((char *) (pMandParams->iv_pairs[uiMandParamCnt].value_p));
						if (uiPortId < 0 ||  uiPortId > 24)
						{
							 CM_CBK_DEBUG_PRINT ("Invalid Port ID entered");
							 fill_app_result_struct (&port_result, NULL, CM_GLU_PORT_ID_INVALID);
							 *presult_p = port_result;
							 return OF_FAILURE;
						}
						break;
			}
	 }
	 CM_CBK_PRINT_IVPAIR_ARRAY (pMandParams);
	 return OF_SUCCESS;
}

 int32_t dprm_datapath_port_attribute_ucm_validateoptparams (struct cm_array_of_iv_pairs *
			pOptParams,
			struct cm_app_result ** presult_p)
{
	 uint32_t uiOptParamCnt, uiTableCnt;

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
#endif
