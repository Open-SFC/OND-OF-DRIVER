/** Cloud resource manager Virtual Network source file 
* Copyright (c) 2012, 2013  Freescale.
*  
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
* 
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
* implied.
* See the License for the specific language governing permissions
* and
* limitations under the License.
***/

/* File  :      crm_view_api.c
*/
/*
*  File name: crm_view_api.c
*  Author: Varun Kumar Reddy <B36461@freescale.com>
*  Date:   11/10/2013
*  Description: 
*/

#include "controller.h"
#include "dprm.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "viewldef.h"
#include "of_view.h"




/*******************************************************************************************/
int32_t  of_create_crm_view_entry(uint64_t crm_handle, char *view_name_p, char *view_value_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered view_name:%s,view_value:%s",view_name_p,view_value_p); 
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view index");
      return OF_FAILURE;
    }
    ret_value= of_create_view_entry(view_index, view_name_p, view_value_p, crm_handle);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to create view entry");
      status = OF_FAILURE;
      break;
    }
  }while(0);
  return status;
}

/*********************************************************************************************/
int32_t  of_remove_crm_view_entry(char *view_name_p, char *view_value_p )
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered view_name:%s,view_value:%s",view_name_p,view_value_p); 
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }

    ret_value=of_remove_view_entry(view_index, view_name_p, view_value_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to remove view entry");
      status = OF_FAILURE;
      break;
    }
  }while(0);

  return status;
}

/********************************************************************************************/
int32_t  of_view_get_crm_view_handle_by_name(struct database_view_node_info *db_view_node_info, char *view_name_p, uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered .view_name:%s",view_name_p);
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_view_handle_by_name(view_index, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }
  }while(0);
  return status;
}

/******************************************************************************************/
int32_t  of_view_get_first_crm_view_entry(struct database_view_node_info *db_view_node_info, 
                                          char *view_name_p, uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_first_view_entry(view_index, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);

  return status;
}

/******************************************************************************************/
int32_t  of_view_get_next_crm_view_entry(struct database_view_node_info *db_view_node_info,
                                         char *view_name_p, uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_next_view_entry(view_index, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);

  return status;
}

/******************************************************************************************/
int32_t  of_view_get_first_crm_view_using_view_value(char *view_name_p, char *view_value_p, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  int32_t  status,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_first_view_entry_using_key(view_index, view_value_p, db_view_node_info, view_handle_p);

    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);

  return status;
}
 
/******************************************************************************************/
int32_t  of_view_get_next_crm_view_using_value( char *view_name_p, char *view_value_p, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(view_name_p);
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " view_index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || view_index < 0 )
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }

    ret_value=of_view_get_next_view_entry_using_key(view_index, view_value_p, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);
  return status;
}
