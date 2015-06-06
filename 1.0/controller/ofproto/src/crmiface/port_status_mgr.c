
#include "controller.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprm.h"
#include "dprmldef.h"
#include "crmapi.h"
#include "crm_vn_api.h"
#include "crm_port_api.h"
#include "port_status_mgr.h"

/**** Local prototypes */

void psm_dp_port_added_notifications_cbk(uint32_t dprm_notification_type,
                                         uint64_t datapath_handle,
                                         struct   dprm_datapath_notification_data dprm_notification_data,
                                         void*    callback_arg1,
                                         void*    callback_arg2);

void psm_dp_port_delete_notifications_cbk(uint32_t dprm_notification_type,
                                          uint64_t datapath_handle,
                                          struct   dprm_datapath_notification_data dprm_notification_data,
                                          void*    callback_arg1,
                                          void*    callback_arg2);

void psm_dp_port_update_notifications_cbk(uint32_t dprm_notification_type,
                                          uint64_t datapath_handle,
                                          struct   dprm_datapath_notification_data dprm_notification_data,
                                          void*    callback_arg1,
                                          void*    callback_arg2);

void psm_crm_port_added_notifications_cbk(uint32_t notification_type,
                                          uint64_t vn_handle,
                                          struct   crm_port_notification_data notification_data,
                                          void     *callback_arg1,
                                          void     *callback_arg2);

int32_t psm_register_dprm_port_events();
int32_t psm_unregister_dprm_port_events();
/**********************************************************************************************************/
int32_t psm_module_init()
{
  int32_t ret_val = PSM_SUCCESS;
  
 
  ret_val =  psm_register_dprm_port_events(); 
  if(ret_val != PSM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Registrtion to receive DPRM port events failed");
    return PSM_FAILURE;
  }

  ret_val = crm_register_port_notifications(CRM_PORT_STATUS,
                                            psm_crm_port_added_notifications_cbk,
                                            NULL,
                                            NULL);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Registrtion to receive CRM Add port event failed");
    psm_unregister_dprm_port_events();  
    return PSM_FAILURE;
  }
  return PSM_SUCCESS;
}
/********************************************************************************************************/
int32_t psm_module_uninit()
{
  int32_t ret_val;
 
  ret_val = psm_unregister_dprm_port_events();
  if(ret_val != PSM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"PSM module un-initialization is failed");
  }
 
  ret_val = crm_deregister_port_notifications(CRM_PORT_STATUS,psm_crm_port_added_notifications_cbk); 
  if(ret_val != PSM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"PSM module un-initialization failed");
  }
  return ret_val;
}
/*******************************************************************************************************
* Function : psm_reg_dprm_port_events
* Description:
*       This function registers with the dprm module to get the notifications
*       when ever port add/mod/delete occurs in the dprm module.
* Return :
*       DPRM_SUCCESS :call back reg success
*       DPRM_FAILURE: call back reg fails
******************************************************************************************************/
int32_t psm_register_dprm_port_events()
{
  int32_t ret_val = DPRM_SUCCESS;

  do
  {
    ret_val = dprm_register_datapath_notifications(DPRM_DATAPATH_PORT_ADDED,
                                                   psm_dp_port_added_notifications_cbk,
                                                   NULL,
                                                   NULL);
    if(ret_val != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Registration to receive DPRM Port Add Notifications failed!");
      break;
    }

    ret_val = dprm_register_datapath_notifications(DPRM_DATAPATH_PORT_DESC_UPDATE,
                                                   psm_dp_port_update_notifications_cbk,
                                                   NULL,
                                                   NULL);
    if(ret_val != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Registration to receive DPRM Port Update Notifications failed!");
      break;
    }

    ret_val = dprm_register_datapath_notifications(DPRM_DATAPATH_PORT_DELETE,
                                                   psm_dp_port_delete_notifications_cbk,
                                                   NULL,
                                                   NULL);
    if(ret_val != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "Registration to receive DPRM Port Delete Notifications failed!");
      break;
    }
  }while(0);
 
  if(ret_val != DPRM_SUCCESS)
  {
    psm_unregister_dprm_port_events();
    ret_val = PSM_FAILURE;
  } 
  else
  { 
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, " Registration to receive DPRM Port Notifications is successful.");
    return DPRM_SUCCESS;
  }
  return ret_val;
}
/**********************************************************************************************************/
int32_t psm_unregister_dprm_port_events()
{
  dprm_deregister_datapath_notifications(DPRM_DATAPATH_PORT_ADDED,psm_dp_port_added_notifications_cbk);
  dprm_deregister_datapath_notifications(DPRM_DATAPATH_PORT_DESC_UPDATE,psm_dp_port_update_notifications_cbk);
  dprm_deregister_datapath_notifications(DPRM_DATAPATH_PORT_DELETE,psm_dp_port_delete_notifications_cbk);
  return PSM_SUCCESS;
}
/**********************************************************************************************************/
void psm_dp_port_added_notifications_cbk(uint32_t dprm_notification_type,
                                         uint64_t datapath_handle,
                                         struct   dprm_datapath_notification_data dprm_notification_data,
                                         void*    callback_arg1,
                                         void*    callback_arg2)
{
  int32_t  ret_val; 
  uint64_t crm_port_handle;
  uint8_t  port_status;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "DPRM Add Port Notification is received");

  ret_val = crm_get_port_handle(dprm_notification_data.port_name,&crm_port_handle); 
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "PORT is not present in CRM port database = %s",dprm_notification_data.port_name);
    return;
  }

  if(dprm_notification_data.port_id != 0) 
  {
    port_status = PORT_STATUS_UP;
    crm_set_port_status(dprm_notification_data.port_name,
                        crm_port_handle,
                        dprm_notification_data.port_handle,
                        port_status,
                        dprm_notification_data.port_id);
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "After crm_set_port_status call ");
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Ignoring the DPRM Port notification due to local Port registration by Applications");
    return;
  }
}
/*******************************************************************************************************************************/
void psm_dp_port_delete_notifications_cbk(uint32_t dprm_notification_type,
                                          uint64_t datapath_handle,
                                          struct   dprm_datapath_notification_data dprm_notification_data,
                                          void*    callback_arg1,
                                          void*    callback_arg2)
{
  int32_t  ret_val;
  uint64_t crm_port_handle;
  uint8_t  port_status;

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "DPRM Delete Port Notification is received");

  ret_val = crm_get_port_handle(dprm_notification_data.port_name,&crm_port_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "PORT is not present in CRM port database = %s",dprm_notification_data.port_name);
    return;
  }

  if(dprm_notification_data.port_id != 0)
  {
    port_status = PORT_STATUS_DOWN;
    crm_set_port_status(dprm_notification_data.port_name,crm_port_handle,
                        dprm_notification_data.port_handle,
                        port_status,dprm_notification_data.port_id);
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Ignoring the DPRM Port notification due to local Port deregistration by Applications");
    return;
  }
}
/***************************************************************************************************************************/
void psm_dp_port_update_notifications_cbk(uint32_t dprm_notification_type,
                                          uint64_t datapath_handle,
                                          struct   dprm_datapath_notification_data dprm_notification_data,
                                          void*    callback_arg1,
                                          void*    callback_arg2)
{
  int32_t  ret_val;
  uint64_t crm_port_handle;
  uint8_t  port_status;

  ret_val = crm_get_port_handle(dprm_notification_data.port_name,&crm_port_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "PORT is not present in CRM port database = %s",dprm_notification_data.port_name);
    return;
  }

  if(dprm_notification_data.port_id != 0)
  {
    port_status = PORT_STATUS_UPDATE;
    crm_set_port_status(dprm_notification_data.port_name,crm_port_handle,
                        dprm_notification_data.port_handle,
                        port_status,dprm_notification_data.port_id);
  }
  else
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Ignoring the DPRM Port notification due to local Port deregistration by Applications");
    return;
  }
}
/***************************************************************************************************************************/
void  psm_get_port_status(uint64_t logical_switch_handle,
                          char* port_name_p,uint64_t* dp_port_handle_p,
                          uint8_t* port_status_p, uint32_t* port_id_p)
{
  int32_t  ret_val;
  uint64_t port_handle;
  struct   dprm_port_entry* port_entry_p;
  uint64_t dp_port_handle;
  struct   dprm_port_info dprm_port_info;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"port name = %s",port_name_p);
  
  ret_val = dprm_get_port_handle(logical_switch_handle,port_name_p,&port_handle);
  if(ret_val == DPRM_SUCCESS)
  {
    ret_val = get_datapath_port_byhandle(logical_switch_handle,port_handle,&port_entry_p);
    if(ret_val == DPRM_SUCCESS)
    {
      /* check the dprm port id . if it is ready copy to crm port. otherwise
       * make crm port id to zero */

      *dp_port_handle_p = port_handle;

      if((port_entry_p->port_id) >0)
      {
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG, "DPRM Port Ready");
        *port_status_p = PORT_STATUS_UP;
        *port_id_p     = port_entry_p->port_id;
      }
      else
      {
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG, "DPRM Port present but NOT Ready");
        *port_status_p = PORT_STATUS_DOWN;
        *port_id_p = 0;
      }
    }
  }
  else
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," No such port in DPRM: port_name =  %s",port_name_p);
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG," Adding port to DPRM");

    strcpy(dprm_port_info.port_name,port_name_p);
    dprm_port_info.port_id = 0;
    ret_val = dprm_add_port_to_datapath(logical_switch_handle,&dprm_port_info,&dp_port_handle);
 
    *port_status_p = PORT_STATUS_CREATED;
    *port_id_p = 0;
     OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG, "DPRM Port created and NOT Ready");
  }
}
/**************************************************************************************************************/
void psm_crm_port_added_notifications_cbk(uint32_t notification_type,
                                          uint64_t vn_handle,
                                          struct   crm_port_notification_data notification_data,
                                          void     *callback_arg1,
                                          void     *callback_arg2)

{
  uint8_t   port_status;
  uint32_t  port_id;
  uint64_t  dp_port_handle;

  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"notification type = %d",notification_type);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"port name = %s",notification_data.port_name);

  if(notification_type != CRM_PORT_STATUS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Not interested in port notifications other than Add port Notification");
    return;
  }
  
  psm_get_port_status(notification_data.dp_handle,notification_data.port_name,&dp_port_handle,&port_status,&port_id);

  crm_set_port_status(notification_data.port_name,
                      notification_data.crm_port_handle,
                      dp_port_handle,
                      port_status,
                      port_id);

  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "After crm_set_port_status call ");
}
/*************************************************************************************************************/
