


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
#include "of_view.h"
#include "viewldef.h"
#include "dprmldef.h"

/* VIEW Namespace to DPID */


int32_t  of_create_namespace_dpid_view_entry(uint64_t datapath_handle, uint64_t dpid, char *ns_name_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  char node_name[64];
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {

    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      /* Need to be discussed:
         Is it allowed to create a view if view doesn't exists?
         If yes - remove this comment
         If No - remove following 3 lines of code */
      view_index= of_create_view(OF_VIEW_NSNAME_DPID);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "index = %d",view_index);
      if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
      {
        OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to create view");
        status = OF_FAILURE;
        break;
      }

    }

//    of_view_ltoa(dpid,node_name);
    sprintf(node_name,"%llx",dpid);
    ret_value= of_create_view_entry(view_index, node_name, ns_name_p, datapath_handle);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }
  }while(0);

  return status;
}

int32_t  of_remove_namespace_dpid_view_entry( uint64_t datapath_handle, uint64_t dpid, char *ns_name_p )
{
  char node_name[64];
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

    of_view_ltoa(dpid, node_name);
    ret_value=of_remove_view_entry(view_index, node_name, ns_name_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }


  }while(0);

  return status;
}

int32_t  of_view_get_namespace_dpid_view_handle_by_name( struct database_view_node_info *db_view_node_info,   uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

    ret_value=of_view_get_view_handle_by_name(view_index, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }


  }while(0);

return status;

}

int32_t  of_view_get_first_namespace_dpid_view( struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_first_view_entry(view_index, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);

  return status;
}


int32_t  of_view_get_next_namespace_dpid_view( struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_next_view_entry(view_index, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);

  return status;
}


int32_t  of_view_get_first_namespace_dpid_view_using_ns( char *ns_name, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  int32_t  status,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }
    ret_value=of_view_get_first_view_entry_using_key(view_index, ns_name, db_view_node_info, view_handle_p);

    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);

  return status;
}

int32_t  of_view_get_next_namespace_dpid_view_using_ns( char *ns_name, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  struct database_view_node *view_node_entry_p;
  int32_t  status=OF_SUCCESS,ret_value;
  int32_t view_index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {
    view_index= of_get_view_index(OF_VIEW_NSNAME_DPID);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, " index = %d",view_index);
    if(view_index >= DPRM_MAX_VIEW_DATABASE || index < 0 )
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

    ret_value=of_view_get_next_view_entry_using_key(view_index, ns_name, db_view_node_info, view_handle_p);
    if(ret_value == OF_FAILURE)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "failed to get view");
      status = OF_FAILURE;
      break;
    }

  }while(0);
  return status;
}


int32_t  of_view_ltoa(uint64_t ulInt ,char* cTmpBuf)
{
        uint32_t uiRem;
        uint64_t ulQuot;
        int32_t index_i = 0,ii,jj;
        char cBuf[32];

        ulQuot = ulInt;
        do
        {
                uiRem = (uint64_t)OF_VIEW_MODU32((uint64_t)ulQuot,(uint64_t)10);
                ulQuot = (uint64_t)OF_VIEW_DIVU32((uint64_t)ulQuot,(uint64_t)10);
                cBuf[index_i] = uiRem+48;
                index_i++;
        }while(ulQuot !=0);

        for(ii=index_i-1,jj=0;ii>=0;ii--,jj++)
        {
                cTmpBuf[jj] = cBuf[ii];
        }
        cTmpBuf[jj] = '\0';
        return OF_SUCCESS;
}
