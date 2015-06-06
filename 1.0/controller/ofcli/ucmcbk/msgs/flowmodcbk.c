/*
 * $Date: 2014/03/21 15:10:04 $
 */ 

/**************************************************************************/
/* Copyright (c) 2010, 2011 Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
/****************************************************************************
 *  File                : 
 *  Description         : This file contains source code implmenting 
 *                        configuration middleware data model tree.
 *  Revision History    :
 *    Version    Date          Author           Change Description
              13/05/2013     Kumara Swamy G.
	      18/11/2013     Vivek Sen Reddy K.	 Command parameter added.Support for
	      					 Strict Commands.
****************************************************************************/
#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_flowmod.h"
#include "of_flowinfo.h"


#define ADD_COMMAND 1
#define STRICT_COMMAND 2

int32_t of_flow_appl_ucmcbk_init (void);


void* of_flow_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t  of_flow_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp);

int32_t of_flow_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_flow_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp);

int32_t of_flow_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp);


int32_t of_flow_ucm_validatemandparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct cm_app_result ** app_result_pp);

int32_t of_flow_ucm_validateoptparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct cm_app_result ** app_result_pp);

static int32_t of_flow_ucm_getparams (struct ofi_flow_mod_info *flow_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p);

int32_t of_flow_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp);

static int32_t of_flow_ucm_setmandparams (struct cm_array_of_iv_pairs *
		mand_iv_pairs,
		struct dprm_datapath_general_info *datapath_info,
		struct ofi_flow_mod_info *flow_info,
		struct cm_app_result ** app_result_pp);

static int32_t of_flow_ucm_setoptparams (struct cm_array_of_iv_pairs *
		                                  opt_iv_pairs,
                                          struct ofi_flow_mod_info * flow_info,
                                          struct cm_app_result ** app_result_pp);

int32_t of_flow_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs,
                struct cm_array_of_iv_pairs **result_iv_pairs_pp);


struct cm_dm_app_callbacks of_flow_ucm_callbacks = 
{
	of_flow_starttrans,
	of_flow_addrec,
	of_flow_setrec,
	of_flow_delrec,
	of_flow_endtrans,
	NULL,
	NULL,
	of_flow_getexactrec,
	of_flow_verifycfg,
	NULL
};

uint32_t transId_g;
of_list_t flow_trans_list_g;
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_appl_ucmcbk_init (void)
{
	int32_t return_value;

	CM_CBK_DEBUG_PRINT ("Entered");
	return_value= cm_register_app_callbacks(CM_ON_DIRECTOR_DATAPATH_FLOWMOD_APPL_ID,
			&of_flow_ucm_callbacks);
	if(return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT("flow application registration failed");
		return OF_FAILURE;
	}
	OF_LIST_INIT(flow_trans_list_g,NULL);
        transId_g = 0;
	return OF_SUCCESS;
}
uint64_t atox64(char *cPtr)
{
  uint8_t ucbyte = 0;
  char char_byte[3] = {};
  uint64_t ui64_data = 0;
  char *data = cPtr;
  int ii;
  uint8_t isOddNum = 0;

  if (*(data) == '\0')
  {
    return 0;
  }
  isOddNum = strlen(data) % 2;

  for (ii = 0; ii < 8; ii++)
  {
    if ((ii==0) && (isOddNum))
    {
      strncat(char_byte, "0", 1);
      strncat(char_byte+1, data, 1);
      data += 1;
    }
    else
    {
      strncpy(char_byte, data, 2);
      data += 2;
    }
    ucbyte = atox(char_byte);

    ui64_data = ui64_data << 8;
    ui64_data |= ((uint64_t)ucbyte & 0x00000000000000ff);

    if (*(data) == '\0')
    {
      break;
    }
    ucbyte = 0;
  }

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "returning2 %llx",ui64_data);
  return ui64_data;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs,
		uint32_t command_id, struct cm_app_result ** result_pp)
{
	struct cm_app_result *flow_result = NULL;
	int32_t return_value = OF_FAILURE;

	uint32_t param_cnt;
	uint32_t flow_id;
	char *command;
	uint32_t command_type;

	uint32_t is_priority_set=0;
	uint64_t is_cookie_set=0;
	uint64_t is_cookie_mask_set=0;

	CM_CBK_DEBUG_PRINT ("Entered");
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"Entered");

	//Validating Mandatory parameters 
	for (param_cnt = 0; param_cnt < key_iv_pairs->count_ui;
			param_cnt++)
	{
		switch (key_iv_pairs->iv_pairs[param_cnt].id_ui)
		{
			case CM_DM_DATAPATH_FLOWMOD_ID:
				flow_id=of_atoi((char *) key_iv_pairs->iv_pairs[param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("flow id is %d", flow_id);
				break;
			case CM_DM_FLOWMOD_COMMAND_ID:
	                        command = ((char *) key_iv_pairs->iv_pairs[param_cnt].value_p);
	                        CM_CBK_DEBUG_PRINT ("commnd id is %d", command);
               	                OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"command value is is %s",command);
	                        if(!strcmp(command,"modify_strict") || 
				    !strcmp(command,"delete_strict"))
				 {
				     command_type = STRICT_COMMAND;
				  }
			         else if(!strcmp(command,"add")){
	                             command_type = ADD_COMMAND;
	                          }
				 break;
                    	case CM_DM_FLOWMOD_COOKIE_ID:
				 is_cookie_set=1;
				 break;
				
                    	case CM_DM_FLOWMOD_COOKIEMASK_ID:
				 is_cookie_mask_set=1;
				 break;
                    	case CM_DM_FLOWMOD_PRIORITY_ID:
				 is_priority_set=1;
				 break;

			default:
				 break;
			
		}
	}
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"command_type  value is %d ",command_type);
        CM_CBK_DEBUG_PRINT ("command_type value is %d",command_type);

	if(command_type == STRICT_COMMAND){
	  if(is_cookie_set == 0 || is_cookie_mask_set == 0 || is_priority_set == 0){
	   	  fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_COOKIE_AND_MASK_MISSED);
	          CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
		  *result_pp = flow_result;
		  return OF_FAILURE;
	  }
	}
	else if(command_type == ADD_COMMAND){
		if(is_priority_set == 0){
	   	  fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_PRIORITY_MISSED);
	          CM_CBK_DEBUG_PRINT ("Validate Optional Parameters Failed");
		  *result_pp = flow_result;
		  return OF_FAILURE;
		}
	}
	 *result_pp = flow_result;
	 return OF_SUCCESS;
	
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void* of_flow_starttrans(struct cm_array_of_iv_pairs   * key_iv_pairs,
		                uint32_t command_id,
		                struct cm_app_result ** result_pp)
{
	struct flow_trans *trans_rec=NULL;
	struct ofi_flow_mod_info *flow_info;
	struct dprm_datapath_general_info datapath_info={};
	struct cm_app_result *flow_result = NULL;
	uint64_t datapath_handle;
	int32_t return_value;
	uchar8_t offset;
	offset = OF_FLOW_TRANS_LISTNODE_OFFSET;

	CM_CBK_DEBUG_PRINT ("Entered");

	flow_info=(struct ofi_flow_mod_info * ) calloc(1,sizeof(struct ofi_flow_mod_info ));
	if ( flow_info == NULL)
	{
		CM_CBK_DEBUG_PRINT ("memory allocation failed");
		return NULL;
	}
    
	if ((of_flow_ucm_setmandparams (key_iv_pairs,&datapath_info, flow_info, &flow_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		return NULL;
	}

	return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %llx",datapath_info.dpid);
		fill_app_result_struct (&flow_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
		*result_pp = flow_result;
		return NULL;
	}

	OF_FLOW_GENERATE_TRANS_ID(transId_g);
	trans_rec=(struct flow_trans *) calloc(1,sizeof(struct flow_trans));	
	trans_rec->datapath_handle=datapath_handle;
	trans_rec->trans_id=transId_g;
	trans_rec->command_id=command_id;
	trans_rec->flow_id=flow_info->flow_id;
     if(flow_info->priority)
       trans_rec->priority = flow_info->priority;

    switch (command_id)
	{
		case CM_CMD_ADD_PARAMS:
         trans_rec->sub_command_id = ADD_FLOW;
         return_value = of_datapath_add_static_flow(datapath_handle, flow_info);
         if (return_value != OF_SUCCESS)
         { 
           CM_CBK_DEBUG_PRINT ("Static Flow Addition Failed");
           fill_app_result_struct (&flow_result, NULL, CM_GLU_STATIC_FLOW_ADD_FAILED);
           *result_pp = flow_result;
           free(trans_rec);
           return NULL;
         }
         break;

       case CM_CMD_SET_PARAMS:
         trans_rec->sub_command_id = MODIFY_FLOW;
         free(flow_info);
         break;

       case CM_CMD_DEL_PARAMS:
         trans_rec->sub_command_id = DEL_FLOW;
         free(flow_info);
         break;
    }
	CM_CBK_DEBUG_PRINT ("node appended to list");
	OF_APPEND_NODE_TO_LIST(flow_trans_list_g,trans_rec,offset);

	return (void *)trans_rec;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_flow_addrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *flow_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_flow_mod_info flow_info = { };
	struct dprm_datapath_general_info datapath_info={};
	uint64_t datapath_handle;

	CM_CBK_DEBUG_PRINT ("Entered");

	if ((of_flow_ucm_setmandparams (mand_iv_pairs,&datapath_info, &flow_info, &flow_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_pp=flow_result;
		return OF_FAILURE;
	}

	return_value = of_flow_ucm_setoptparams (opt_iv_pairs, &flow_info, &flow_result);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Optional Parameters Failed");
		*result_pp =flow_result;
		return OF_FAILURE;
	}

	return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %llx",datapath_info.dpid);
		fill_app_result_struct (&flow_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
		*result_pp = flow_result;
		return OF_FAILURE;
	}
	

	return_value = of_datapath_update_flow(datapath_handle, &flow_info);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Flow Addition Failed");
		fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_ADD_FAILED);
		*result_pp =flow_result;
		return OF_FAILURE;
	}

	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_setrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * mand_iv_pairs,
		struct cm_array_of_iv_pairs * opt_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *flow_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_flow_mod_info flow_info = { };
	struct dprm_datapath_general_info datapath_info={};
	uint64_t datapath_handle;

	CM_CBK_DEBUG_PRINT ("Entered");
   	return OF_SUCCESS;
	memset (&flow_info, 0, sizeof (struct ofi_flow_mod_info));

	if ((of_flow_ucm_setmandparams (mand_iv_pairs,&datapath_info, &flow_info, &flow_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		*result_pp=flow_result;
		return OF_FAILURE;
	}

	return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %llx",datapath_info.dpid);
		fill_app_result_struct (&flow_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
		*result_pp = flow_result;
		return OF_FAILURE;
	}

    #if 0 //need to check with atmaram
	return_value = of_group_unregister_buckets(datapath_handle, flow_info.flow_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Flow does not exist with name %d",flow_info.flow_id);
		fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_DOESNOT_EXIST);
		*result_pp =flow_result;
		return OF_FAILURE;
	}
    #endif


	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_delrec (void * config_trans_p,
		struct cm_array_of_iv_pairs * key_iv_pairs,
		struct cm_app_result ** result_pp)
{
	struct cm_app_result *flow_result = NULL;
	int32_t return_value = OF_FAILURE;
	struct ofi_flow_mod_info flow_info = { };
	struct dprm_datapath_general_info datapath_info={};
	uint64_t datapath_handle;
  struct flow_trans *trans_rec=(struct flow_trans *)config_trans_p;

	CM_CBK_DEBUG_PRINT ("Entered");

	if ((of_flow_ucm_setmandparams (key_iv_pairs,&datapath_info, &flow_info, &flow_result)) !=
			OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
		fill_app_result_struct (&flow_result, NULL, CM_GLU_SET_MAND_PARAM_FAILED);
		*result_pp =flow_result;
		return OF_FAILURE;
	}
  trans_rec->table_id = flow_info.table_id;
  trans_rec->priority = flow_info.priority;

	return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Datapath does not exist with id %llx",datapath_info.dpid);
		fill_app_result_struct (&flow_result, NULL, CM_GLU_GROUP_DATAPATH_DOESNOT_EXIST);
		*result_pp = flow_result;
		return OF_FAILURE;
	}


  #if 0
	return_value=of_datapath_remove_static_flow(datapath_handle, flow_info.flow_id);
	if (return_value != OF_SUCCESS)
	{
		CM_CBK_DEBUG_PRINT ("Flow does not exist with id %d",flow_info.flow_id);
		fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_DOESNOT_EXIST);
		*result_pp =flow_result;
		return OF_FAILURE;
	}
  #endif

	return OF_SUCCESS;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t  of_flow_endtrans(void *config_trans_p,
		uint32_t command_id,
		struct cm_app_result ** result_pp)
{
	struct flow_trans *trans_rec=config_trans_p;
	struct flow_trans *trans_rec_entry_p, *prev_trans_rec_p;
	struct ofi_flow_mod_info flow_info={};
	struct cm_app_result *flow_result = NULL;
	int32_t return_value = OF_FAILURE;
	uchar8_t offset;
	offset = OF_FLOW_TRANS_LISTNODE_OFFSET;

	CM_CBK_DEBUG_PRINT ("Entered");


	CM_CBK_DEBUG_PRINT("trans id %d command id %d sub cmd id %d flow id %d",
                      trans_rec->trans_id,trans_rec->command_id,
                      trans_rec->sub_command_id,trans_rec->flow_id);

	switch(command_id)
	{

		case CM_CMD_CONFIG_SESSION_REVOKE:
			   CM_CBK_DEBUG_PRINT ("Revoke command");
         OF_LIST_SCAN(flow_trans_list_g, trans_rec_entry_p, struct flow_trans *,offset)
         {
           if (trans_rec_entry_p->trans_id == trans_rec->trans_id )
           {
             return_value=of_datapath_remove_static_flow(
                 trans_rec_entry_p->datapath_handle,
                 trans_rec_entry_p->flow_id);
             if (return_value != OF_SUCCESS)
             {
               CM_CBK_DEBUG_PRINT ("Flow does not exist with id %d",flow_info.flow_id);
               fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_DOESNOT_EXIST);
               *result_pp =flow_result;
             }

             CM_CBK_DEBUG_PRINT ("trans rec removed from list and freed");
             OF_REMOVE_NODE_FROM_LIST(flow_trans_list_g, trans_rec_entry_p,  prev_trans_rec_p,offset);
             free(trans_rec_entry_p);
             break;
           }
           prev_trans_rec_p=trans_rec_entry_p;
         }
         break;

		case CM_CMD_CONFIG_SESSION_COMMIT:
         CM_CBK_DEBUG_PRINT ("Commit command");
         /* need to send flow add message to switch */
         return_value=of_flow_mod_frame_and_send_message(trans_rec->datapath_handle,
                                                         trans_rec->flow_id,
                                                         trans_rec->table_id,
                                                         trans_rec->priority,
                                                         trans_rec->sub_command_id);
         if (return_value != OF_SUCCESS)
         {
	   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_INFO,"return_valu = %d",return_value);	
	   if(return_value == OF_FLOWMOD_ERROR_INVALID_MISSENTRY)	
	   {
           CM_CBK_DEBUG_PRINT ("Error: Invalid Miss Entry, Match Fields should be null");
           fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_INVALID_MISSENTRY);
           *result_pp =flow_result;
	   }
	   else if(return_value == OF_FLOWMOD_GOTOTABLE_INVALID)
           {
           CM_CBK_DEBUG_PRINT ("Error: Invalid Gototable instruction");
           fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_INVALID_GOTOTABLE);
           *result_pp =flow_result;
           }

 	   else{	
           CM_CBK_DEBUG_PRINT ("Error: of_flow_frame_and_send_message_request failed");
           fill_app_result_struct (&flow_result, NULL, CM_GLU_FLOW_ADD_TO_SWITCH_FAILED);
           *result_pp =flow_result;
           }
           return OF_FAILURE;
         }

         OF_LIST_SCAN(flow_trans_list_g, trans_rec_entry_p, struct flow_trans *,offset)
         {
           if (trans_rec_entry_p->trans_id == trans_rec->trans_id)
           {
             OF_REMOVE_NODE_FROM_LIST(flow_trans_list_g, trans_rec_entry_p,  prev_trans_rec_p,offset);
             CM_CBK_DEBUG_PRINT ("trans rec removed from list and freed");
             free(trans_rec_entry_p);
             break;
           }
           prev_trans_rec_p=trans_rec_entry_p;
         }
         break;

    default:
         CM_CBK_DEBUG_PRINT ("unknown command");

	}
	return OF_SUCCESS;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_getexactrec(struct cm_array_of_iv_pairs * key_iv_pairs,
                            struct cm_array_of_iv_pairs **result_iv_pairs_pp)
{

	return OF_FAILURE;
 // Return failure for all contexts	
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  struct cm_app_result *flow_result = NULL;
  uint32_t ii = 0;
  int32_t return_value = OF_FAILURE;
  struct ofi_flow_mod_info *flow_entry_p;
  struct ofi_flow_mod_info flow_info={};
  struct dprm_datapath_general_info datapath_info={};
  uint64_t datapath_handle;

  CM_CBK_DEBUG_PRINT ("Entered");

  of_memset (&flow_info, 0, sizeof (struct ofi_flow_mod_info));
  if ((of_flow_ucm_setmandparams(key_iv_pairs,&datapath_info, &flow_info, &flow_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Error: datapath does not exist with id %llx",datapath_info.dpid);
    return OF_FAILURE;
  }

  return_value=of_get_flow(datapath_handle, flow_info.flow_id, &flow_entry_p);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_get_flow failed");
    return OF_FAILURE;
  }

  result_iv_pairs_p =
    (struct cm_array_of_iv_pairs *) of_calloc (1, sizeof (struct cm_array_of_iv_pairs));
  if (result_iv_pairs_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("Memory allocation failed for pResultiv_pairs");
    return OF_FAILURE;
  }

  return_value= of_flow_ucm_getparams(flow_entry_p, &result_iv_pairs_p[0]);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("of_flow_ucm_getparams failed");
    return OF_FAILURE;
  } 

  *result_iv_pairs_pp = result_iv_pairs_p;

  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int32_t of_flow_ucm_setmandparams(struct cm_array_of_iv_pairs *
                                         mand_iv_pairs,
                                         struct dprm_datapath_general_info *datapath_info,
                                         struct ofi_flow_mod_info *flow_info,
                                         struct cm_app_result ** app_result_pp)
{
	uint32_t mand_param_cnt;
	uint32_t flow_id;
	uint64_t idpId;
	uint8_t type;
	char *command;
	uint64_t command_len;

	CM_CBK_DEBUG_PRINT ("Entered");
	for (mand_param_cnt = 0; mand_param_cnt < mand_iv_pairs->count_ui;
			mand_param_cnt++)
	{
		switch (mand_iv_pairs->iv_pairs[mand_param_cnt].id_ui)
		{
			case CM_DM_DATAPATH_DATAPATHID_ID:
				idpId=charTo64bitNum((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT("dpid is: %llx",idpId);
				datapath_info->dpid=idpId;
				break;

			case CM_DM_FLOWMOD_FLOWID_ID:
				flow_info->flow_id = 
                  (uint32_t)of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("flow id is %d", flow_info->flow_id);
				break;
            
			case CM_DM_FLOWMOD_TABLEID_ID:
				flow_info->table_id = 
                  (uint8_t)of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("table id is %d", flow_info->table_id);
				break;
            
		  	case CM_DM_FLOWMOD_COMMAND_ID:
		    		command = ((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
		    		command_len = mand_iv_pairs->iv_pairs[mand_param_cnt].value_length;
		    		if(!strncmp(command,"add",command_len)){
					flow_info->command=OFPFC_ADD;

				}
		    		else if(!strncmp(command,"modify",command_len)){
					flow_info->command=OFPFC_MODIFY;						
				}
		                else if(!strncmp(command,"modify_strict",command_len)){
					flow_info->command=OFPFC_MODIFY_STRICT;						
				}
				else if(!strncmp(command,"delete",command_len)){
					flow_info->command=OFPFC_DELETE;						
				}
		    		else if(!strncmp(command,"delete_strict",command_len)){
					flow_info->command=OFPFC_DELETE_STRICT;						
				}
		    		else {
					OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR,"Invalid command arguement type");
					return OF_FAILURE;
				}
				OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"Command is %s ,id is %d",command, flow_info->command);
				}
	}
	CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs);
	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t of_flow_ucm_validatemandparams(struct cm_array_of_iv_pairs *
                                       mand_iv_pairs,
                                       struct cm_app_result ** app_result_pp)
{
	uint32_t mand_param_cnt;
	uint32_t flow_id;
	uint8_t type;
	char *command;
	//uint64_t command_len;

	CM_CBK_DEBUG_PRINT ("Entered");
	for (mand_param_cnt = 0; mand_param_cnt < mand_iv_pairs->count_ui;
			mand_param_cnt++)
	{
		switch (mand_iv_pairs->iv_pairs[mand_param_cnt].id_ui)
		{
			case CM_DM_DATAPATH_FLOWMOD_ID:
				flow_id=of_atoi((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
				CM_CBK_DEBUG_PRINT ("flow id is %d", flow_id);
				break;
			/*
			case CM_DM_FLOWMOD_COMMAND_ID:
	                        command = ((char *) mand_iv_pairs->iv_pairs[mand_param_cnt].value_p);
	                       // command_len = mand_iv_pairs->iv_pairs[mand_param_cnt].value_length;
	                        CM_CBK_DEBUG_PRINT ("commnd id is %d", command);
               	                OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"command value is is %s",command);
	                        if(!strcmp(command,"modify_strict") || 
				    !strcmp(command,"delete_strict"))
				 {
				      is_command_strict = STRICT_COMMAND;
			         	OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"is_command_strict value is is %d",is_command_strict);
				  }
			         else{
	                              is_command_strict = 0;
			         	OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"is_command_strict value is is %d",is_command_strict);
	                          }
			*/
		}
	}
	CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs);
	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_flow_ucm_validateoptparams(struct cm_array_of_iv_pairs *
                                       opt_iv_pairs,
				       struct cm_app_result ** app_result_pp)
{
	uint32_t uiOptParamCnt;
	uint8_t type;
	uint64_t cookie=0;
	uint64_t cookie_mask=0;


        CM_CBK_DEBUG_PRINT("Entered");
        for (uiOptParamCnt = 0; uiOptParamCnt < opt_iv_pairs->count_ui; uiOptParamCnt++)
         {
              switch (opt_iv_pairs->iv_pairs[uiOptParamCnt].id_ui)
              {
                    case CM_DM_FLOWMOD_COOKIE_ID:
                    cookie = (uint64_t)atox64((char *) opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p);
                    //CM_CBK_DEBUG_PRINT ("cookie is %d", cookie);
               	    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"cookie value is is %d",cookie);
                    cookie=1;
                    break;

                    case CM_DM_FLOWMOD_COOKIEMASK_ID:
                    cookie_mask  = (uint64_t)atox64((char *) opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p);
               	    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"cookie_mask value is is %d",cookie_mask);
                    cookie_mask = 1;
                    break;
                    default:
                    break;
													                 }
          }
          CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs);
	  if(cookie == 0 || cookie_mask == 0)
		  return OF_FAILURE;
	  else 
          	return OF_SUCCESS;

}








/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int32_t of_flow_ucm_setoptparams(struct cm_array_of_iv_pairs *
		                                    opt_iv_pairs,
                                        struct ofi_flow_mod_info * flow_info,
                                        struct cm_app_result ** app_result_pp)
{
	uint32_t uiOptParamCnt;
	uint8_t switch_type;

	CM_CBK_DEBUG_PRINT ("Entered");

	for (uiOptParamCnt = 0; uiOptParamCnt < opt_iv_pairs->count_ui; uiOptParamCnt++)
	{
		switch (opt_iv_pairs->iv_pairs[uiOptParamCnt].id_ui)
                {
		   case CM_DM_FLOWMOD_PRIORITY_ID:
			flow_info->priority = 
                	  (uint16_t)of_atoi((char *) opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p);
			CM_CBK_DEBUG_PRINT ("priority id is %d", flow_info->priority);
			break;
                  case CM_DM_FLOWMOD_IDLETIMEOUT_ID:
                    flow_info->idle_timeout = 
                      (uint16_t)of_atoi((char *) opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p);
                    CM_CBK_DEBUG_PRINT ("idle_timeout is %d", flow_info->idle_timeout);
                    break;
                  
                  case CM_DM_FLOWMOD_HARDTIMEOUT_ID:
                    flow_info->hard_timeout = 
                      (uint16_t)of_atoi((char *) opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p);
                    CM_CBK_DEBUG_PRINT ("hard_timeout is %d", flow_info->hard_timeout);
                    break;

		  case CM_DM_FLOWMOD_COOKIE_ID:		
			  flow_info->cookie  = (uint64_t)(atox64((char *) (opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p)));
         	          OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"cookie value is is %llx",flow_info->cookie);
			  CM_CBK_DEBUG_PRINT ("Cookie value is %x", flow_info->cookie);
                  break;

                  case CM_DM_FLOWMOD_COOKIEMASK_ID:
	                    flow_info->cookie_mask  = (uint64_t)atox64((char *) opt_iv_pairs->iv_pairs[uiOptParamCnt].value_p);
               	            OF_LOG_MSG(OF_LOG_MOD,OF_LOG_INFO,"cookie Mask value is is %llx",flow_info->cookie_mask);
	                    CM_CBK_DEBUG_PRINT ("Cookie Mask  value is %x", flow_info->cookie_mask);
	          break;
                  case CM_DM_FLOWMOD_CHECKOVERLAP_ID:
			CM_CBK_DEBUG_PRINT ("flow_flags were  %d", flow_info->flow_flags);
			flow_info->flow_flags |= OFPFF_CHECK_OVERLAP;
			CM_CBK_DEBUG_PRINT ("flow_flags changed to  %d", flow_info->flow_flags);
			break;
                  case CM_DM_FLOWMOD_SENDFLOWREM_ID:
			CM_CBK_DEBUG_PRINT ("flow_flags were  %d", flow_info->flow_flags);
			flow_info->flow_flags |= OFPFF_SEND_FLOW_REM;
			CM_CBK_DEBUG_PRINT ("flow_flags changed to  %d", flow_info->flow_flags);
			break;
                  case CM_DM_FLOWMOD_NOPKTCOUNTS_ID:
			CM_CBK_DEBUG_PRINT ("flow_flags were  %d", flow_info->flow_flags);
			flow_info->flow_flags |= OFPFF_NO_PKT_COUNTS;
			CM_CBK_DEBUG_PRINT ("flow_flags changed to  %d", flow_info->flow_flags);
			break;
                  case CM_DM_FLOWMOD_NOBYTECOUNTS_ID:
			CM_CBK_DEBUG_PRINT ("flow_flags were  %d", flow_info->flow_flags);
			flow_info->flow_flags |= OFPFF_NO_BYT_COUNTS;
			CM_CBK_DEBUG_PRINT ("flow_flags changed to  %d", flow_info->flow_flags);
			break;




                  default:
                    break;
                }
        }

	CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs);
	return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static int32_t of_flow_ucm_getparams (struct ofi_flow_mod_info *flow_info,
		struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0, jj, count_ui, iIndex;

#define CM_FLOWMOD_CHILD_COUNT 1
  count_ui = CM_FLOWMOD_CHILD_COUNT;

	//CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)calloc(count_ui, sizeof(struct cm_iv_pair));
  if (!result_iv_pairs_p->iv_pairs)
  {
      CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
      return OF_FAILURE;
  }

  memset(string, 0, sizeof(string));
  of_itoa(flow_info->flow_id, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_FLOWMOD_FLOWID_ID,
                  CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;

  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}

#endif /*OF_CM_SUPPORT */


