
#include "controller.h"
#include "cntrl_queue.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "dprm.h"
#include "dprmldef.h"
#include "crmapi.h"
#include "crm_vm_api.h"
#include "crm_vn_api.h"
#include "crm_port_api.h"
#include "rrm_crm_rtlns_mgr.h"

extern void* crm_port_ref_mempool_g;

int32_t rrm_check_crm_nwport_resource_relationships(char* switch_name_p,char* br_name_p,
                                                    uint64_t* switch_handle_p,uint64_t* logical_switch_handle_p,
                                                    uint32_t* switch_ip_p)
{
  int32_t  ret_val;
  uint64_t switch_handle;
  struct   dprm_switch_entry* switch_entry_p;

  ret_val = dprm_get_switch_handle(switch_name_p,&switch_handle);
  if(ret_val != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Failed to get switch handle for switch name:%s",switch_name_p);
    return RRM_FAILURE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  ret_val = get_switch_byhandle(switch_handle,&switch_entry_p);
  if(ret_val != DPRM_SUCCESS)
  {
    CNTLR_RCU_READ_LOCK_RELEASE();
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"Failed to get switch entry");
    return RRM_FAILURE;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();


  ret_val = dprm_get_logical_switch_handle(switch_handle,br_name_p,logical_switch_handle_p);
  if(ret_val != DPRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Failed to get bridge handle for bridge name:%s",br_name_p);
    return RRM_FAILURE;
  }

  *switch_handle_p = switch_handle;
  *switch_ip_p     = switch_entry_p->ipv4addr.addr;
  return RRM_SUCCESS;
}
/******************************************************************************************************************/
int32_t rrm_check_crmport_resource_relationships(char* switch_name_p,char* br_name_p,
                                                 char* vn_name_p, char* vm_name_p,
                                                 uint64_t* switch_handle_p,uint64_t* logical_switch_handle_p,
                                                 uint64_t* vn_handle_p, uint64_t* vm_handle_p,
                                                 uint32_t* switch_ip_p,uint8_t* nw_type_p)
{
  int32_t  ret_val = RRM_SUCCESS;

  uint64_t switch_handle,vn_handle,vm_handle;
  struct   dprm_switch_entry* switch_entry_p;
  struct   crm_virtual_network* crm_vn_info_p;
  struct   crm_virtual_machine* crm_vm_info_p;
  uint8_t  crm_compute_node_found;
  struct   crm_compute_node* crm_compute_node_entry_scan_p = NULL;

  do
  { 
    /* get the vn handle and vn info */
    ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
    if(ret_val != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Unknown Virtual Network Name");
      ret_val = RRM_ERROR_INVALID_VN_NAME;
      break;
    }
    ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
    if(ret_val != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Failed to get Virtual Network Information");
      ret_val = RRM_ERROR_INVALID_VN_HANDLE;
      break;
    }
    ret_val = crm_get_vm_handle(vm_name_p, &vm_handle);
    if(ret_val != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR,"Unknown Virtual Machine Nmae");
      ret_val= RRM_ERROR_INVALID_VM_NAME;
      break;
    }
    
    CNTLR_RCU_READ_LOCK_TAKE();
   
    ret_val = get_vm_byhandle(vm_handle, &crm_vm_info_p);
    if(ret_val != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Failed to get Virtual Machine Information");
      ret_val = RRM_ERROR_INVALID_VM_HANDLE;
      break;
    }
    ret_val = dprm_get_switch_handle(switch_name_p,&switch_handle);
    if(ret_val != DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR,"Invalid Switch Name");
      ret_val = RRM_ERROR_INVALID_SWITCH_NAME;
      break;
    }
    ret_val = get_switch_byhandle(switch_handle,&switch_entry_p);
    if(ret_val != DPRM_SUCCESS)
    {  
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR,"Failed to get Switch Entry");
      ret_val = RRM_ERROR_INVALID_SWITCH_INFO;
      break;
    }
    *switch_handle_p = switch_handle; 
    *vn_handle_p     = vn_handle;
    *vm_handle_p     = vm_handle; 
    *switch_ip_p     = switch_entry_p->ipv4addr.addr;
    *nw_type_p       = crm_vn_info_p->nw_type;

    /* Add compute node and logical switch to VN if not found */

    crm_compute_node_found = 0;

    /* check the compute node in vn db */
    OF_LIST_SCAN(crm_vn_info_p->crm_compute_nodes,
                 crm_compute_node_entry_scan_p,
                 struct crm_compute_node*,
                 CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET)
    {
      if(!strcmp(crm_compute_node_entry_scan_p->switch_name,switch_name_p))
      {
        crm_compute_node_found = 1;
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"CRM Compute Node Found");
        break;
      }
    }
    if(crm_compute_node_found == 0)
    {
      /* Add compute node and the logical switch */
      ret_val = crm_add_crm_compute_node_to_vn(vn_name_p,switch_name_p,br_name_p);
      if(ret_val != CRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG, "Failed to Add CRM Compute Node and logical switch to VN");
        ret_val = RRM_ERROR_FAILED_TO_ADD_CRM_COMPUTE_NODE;
        break;
      }
    }
    ret_val = dprm_get_logical_switch_handle(switch_handle,br_name_p,logical_switch_handle_p);
    if(ret_val !=DPRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Failed to get bridge handle for bridge name:%s",br_name_p);
      ret_val = RRM_FAILURE;
      break;
    }
  }while(0);  
  CNTLR_RCU_READ_LOCK_RELEASE(); 
  return  ret_val;
}
/********************************************************************************************************************************/
int32_t rrm_setup_crm_vmport_resource_relationships(char* port_name_p,uint64_t crm_port_handle,uint8_t port_type,
                                                char* vm_name_p,char* vn_name_p,
                                                char* switch_name_p,char* br_name_p)

{
  struct   crm_port* crm_port_node_p;
  struct   crm_port_ref* crm_logical_switch_ports_node_p = NULL;
  uint8_t  heap_b;
  int32_t  ret_val;
  struct   crm_virtual_network* crm_vn_info_p;
  uint64_t vm_handle,vn_handle;
  struct   crm_compute_node* crm_compute_node_entry_scan_p = NULL; 
  uint8_t  crm_compute_node_found   = 0;
  uint8_t  crm_logical_switch_found = 0;
  uint32_t index;

  /* Accesses VM structure and updates the VM side port list with port name and port handle for VM and VM_NS ports. */

  ret_val = crm_attach_logical_switch_side_port_to_vm(port_name_p,vm_name_p,vn_name_p,crm_port_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR,"Failed to add vm side ports to VM");
    /* Not treating it as a show stopper but it effects service chaining. Ensure that it is successful. */
  }

  /* Accesses VN structure and updates its internal lists with switch and logical switch and port names and handles. */
  ret_val = crm_get_vm_handle(vm_name_p,&vm_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Failed to get vm handle");
    return CRM_FAILURE;
  }
  ret_val = crm_attach_vm_handle_to_vn(vn_name_p,vm_name_p,vm_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Failed to attach vm handle to vn node");
    /* Not treating it as a show stopper */
  }
  /* check the compute node in vn db */

  /* get the vn handle and vn info */
  ret_val = crm_get_vn_handle(vn_name_p, &vn_handle);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Unknown Virtual Network Name");
    return RRM_ERROR_INVALID_VN_NAME;
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Failed to get Virtual Network Information");
    return RRM_ERROR_INVALID_VN_HANDLE;
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  
  ret_val = crm_get_port_byhandle(crm_port_handle,&crm_port_node_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Failed to get crm port Information");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return RRM_ERROR_INVALID_CRM_PORT_HANDLE;
  }

  if(crm_port_node_p->nw_type == VXLAN_TYPE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"nw_type is VXLAN");
    (crm_port_node_p->nw_params).vxlan_nw.nid         = (crm_vn_info_p->nw_params).vxlan_nw.nid;
     crm_port_node_p->nw_params.vxlan_nw.service_port = (crm_vn_info_p->nw_params).vxlan_nw.service_port;
     //crm_port_node_p->nw_params.vxlan_nw.remote_ip    = (crm_vn_info_p->nw_params).vxlan.nw.remote_ip;
  }
  else if(crm_port_node_p->nw_type == NVGRE_TYPE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"nw_type is NVGRE");
    (crm_port_node_p->nw_params).nvgre_nw.nid         = (crm_vn_info_p->nw_params).nvgre_nw.nid;
     crm_port_node_p->nw_params.nvgre_nw.service_port = (crm_vn_info_p->nw_params).nvgre_nw.service_port;
     //crm_port_node_p->nw_params.nvgre_nw.remote_ip    = (crm_vn_info_p->nw_params).nvgre_nw.remote_ip;
  }
  else if(crm_port_node_p->nw_type == VLAN_TYPE)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"port type is vlan");
    crm_port_node_p->nw_params.vlan_nw.vlan_id = (crm_vn_info_p->nw_params).vlan_nw.vlan_id;
  }

  OF_LIST_SCAN(crm_vn_info_p->crm_compute_nodes,
               crm_compute_node_entry_scan_p,
               struct crm_compute_node*,
               CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET)
  {
    if(!strcmp(crm_compute_node_entry_scan_p->switch_name,switch_name_p))
    {
      OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG, "CRM Compute Node Found");
      crm_compute_node_found = 1;
      break;
    }
  }

  if(crm_compute_node_found == 1)
  {
    /* check the logical switch */
    for(index = 0; index < CRM_MAX_DPS_IN_A_COMPUTE_NODE; index++)
    {
      if(strlen(crm_compute_node_entry_scan_p->dp_references[index].logical_switch_name))
      {
        if(!strcmp((crm_compute_node_entry_scan_p->dp_references[index]).logical_switch_name, br_name_p))
        {
          OF_LOG_MSG(OF_LOG_MOD,OF_LOG_DEBUG,"CRM Logical Switch found");
          crm_logical_switch_found = 1;
          break;
        }
      }
    }
  }
 

  if((!crm_compute_node_found) || (!crm_logical_switch_found))
  {
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Either CRM Compute Node or CRM Logical Switch is NOT found");
    /* This may happen only due to memory allocation problems */
    CNTLR_RCU_READ_LOCK_RELEASE();
    return RRM_ERROR_CRM_CN_NODE_OR_CRM_LOGICAL_SWITCH_NOT_FOUND;
  }

  ret_val = mempool_get_mem_block(crm_port_ref_mempool_g,(uchar8_t **)&crm_logical_switch_ports_node_p,&heap_b);

  if(ret_val != MEMPOOL_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG," failed to allocate memory block for ports to attach vn.");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;
  }

  crm_logical_switch_ports_node_p->heap_b = heap_b;

  crm_logical_switch_ports_node_p->port_saferef = crm_port_handle;
  crm_logical_switch_ports_node_p->port_type    = port_type;
  strcpy(crm_logical_switch_ports_node_p->port_name, port_name_p);

  if(port_type == VM_PORT)
  {
    OF_APPEND_NODE_TO_LIST((crm_compute_node_entry_scan_p->dp_references[index]).vmside_ports,
                            crm_logical_switch_ports_node_p,
                            CRM_LOGICAL_SWITCH_PORT_NODE_OFFSET);
  
    (crm_compute_node_entry_scan_p->dp_references[index]).no_of_vmside_ports++;
    (crm_compute_node_entry_scan_p->dp_references[index]).no_of_ports++;
  }
  else if(port_type == VMNS_PORT)
  {
    OF_APPEND_NODE_TO_LIST((crm_compute_node_entry_scan_p->dp_references[index]).vmns_ports,
                            crm_logical_switch_ports_node_p,
                            CRM_LOGICAL_SWITCH_PORT_NODE_OFFSET);

   (crm_compute_node_entry_scan_p->dp_references[index]).no_of_vmns_ports++;
   (crm_compute_node_entry_scan_p->dp_references[index]).no_of_ports++;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return RRM_SUCCESS;
}
/****************************************************************************************************************************/
int32_t rrm_delete_vmport_resource_relationships(char* port_name, uint64_t port_handle,uint8_t port_type,
                                                 char* switch_name, char* vm_name, char* vn_name,
                                                 uint64_t  vn_handle,  
                                                 uint64_t  logical_switch_handle)
{
  int32_t  ret_val;
  struct   crm_compute_node*     crm_compute_node_entry_scan_p = NULL;
  struct   crm_virtual_network*  crm_vn_info_p = NULL;
  uint8_t  no_of_ports,ii;
  uint8_t  crm_compute_node_found = 0;
  uint32_t lstoffset;

  struct   crm_port_ref* crm_logical_switch_ports_node_p = NULL;
  struct   crm_port_ref* crm_logical_switch_ports_prev_node_p = NULL;

  ret_val = crm_detach_logical_switch_side_port_from_vm(port_name,vm_name,vn_name,&no_of_ports);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"crm port deleted from the crm port table but unable to delete its details from VM");
    /* Not treating it as a show stopper but it may be a problem in some cases. Ensure that it is successful. */
  }

  ret_val = crm_get_vn_byhandle(vn_handle, &crm_vn_info_p);
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD,OF_LOG_ERROR, "Failed to get Virtual Network Information");
    return RRM_ERROR_INVALID_VN_HANDLE;
  }
   OF_LOG_MSG(OF_LOG_CRM,OF_LOG_ERROR,"no of ports : %d",no_of_ports);
  if(no_of_ports == 0)
  {
    ret_val = crm_dettach_vm_handle_from_vn(vm_name,vn_name);
    if(ret_val != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"Failed to detach vm handle to vn node");
      /* Not treating it as a show stopper */
    }
  }

  CNTLR_RCU_READ_LOCK_TAKE();
  do
  { 
    /* check the compute node in  vn db */
    OF_LIST_SCAN(crm_vn_info_p->crm_compute_nodes,crm_compute_node_entry_scan_p,
                 struct crm_compute_node*,
                 CRM_VN_COMPUTE_NODE_ENTRY_LISTNODE_OFFSET
                )
    {
      if(!strcmp(crm_compute_node_entry_scan_p->switch_name, switch_name))
      {
        OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm compute node  found");
        crm_compute_node_found = 1;
        break;
      }
    }

    if(!crm_compute_node_found)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "crm compute node not found!.");
      ret_val= CRM_FAILURE;
      break;
    }

    lstoffset = CRM_LOGICAL_SWITCH_PORT_NODE_OFFSET;
    OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "detaching port handles  from the ");
    ii=0;
    if(port_type == VM_PORT)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "detaching vmside port   from the ");
      OF_LIST_SCAN((crm_compute_node_entry_scan_p->dp_references[ii]).vmside_ports, crm_logical_switch_ports_node_p, struct crm_port_ref*, lstoffset)
      {
        if(strcmp(crm_logical_switch_ports_node_p->port_name, port_name))
        {
          crm_logical_switch_ports_prev_node_p = crm_logical_switch_ports_node_p;
          continue;
        }

        OF_REMOVE_NODE_FROM_LIST((crm_compute_node_entry_scan_p->dp_references[ii]).vmside_ports, crm_logical_switch_ports_node_p,crm_logical_switch_ports_prev_node_p,lstoffset);
        (crm_compute_node_entry_scan_p->dp_references[ii]).no_of_vmside_ports--;
        break;
      }
    }
    else if(port_type == VMNS_PORT)
    {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "detaching vmns port   from the ");
      OF_LIST_SCAN((crm_compute_node_entry_scan_p->dp_references[ii]).vmns_ports, crm_logical_switch_ports_node_p, struct crm_port_ref*, lstoffset)
      {
        if(strcmp(crm_logical_switch_ports_node_p->port_name, port_name))
        {
          crm_logical_switch_ports_prev_node_p = crm_logical_switch_ports_node_p;
          continue;
        }

        OF_REMOVE_NODE_FROM_LIST((crm_compute_node_entry_scan_p->dp_references[ii]).vmns_ports, crm_logical_switch_ports_node_p,crm_logical_switch_ports_prev_node_p,lstoffset);
        (crm_compute_node_entry_scan_p->dp_references[ii]).no_of_vmns_ports--;
        break;
      }
    }
  }while(0);
  
  (crm_compute_node_entry_scan_p->dp_references[ii]).no_of_ports--;
  OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG, "number of ports attached to the  :%d",(crm_compute_node_entry_scan_p->dp_references[ii]).no_of_ports);
  /* TBD Delete compute node and logical switch if number of ports attached becomes 0. */ 
  CNTLR_RCU_READ_LOCK_RELEASE();
  return CRM_SUCCESS;
}
/***************************************************************************************************************************/
/* Utility Functions */
int32_t rrm_get_vm_logicalswitch_side_ports(char*      vm_name,
                                            uint64_t   vm_handle,
                                            struct     crm_virtual_machine* crm_vm_node_p,
                                            uint64_t*  port_saferef_p)
{
  int32_t ret_val;
  struct  crm_vm_port* crm_vm_port_node_scan_entry_p = NULL; 
  struct  crm_port* port_info_p; 
 
  if(crm_vm_node_p == NULL)
  {
    if(vm_handle == 0)
    {
      ret_val = crm_get_vm_handle(vm_name,&vm_handle);
      if(ret_val != CRM_SUCCESS)
      {
        OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Failed to get vm handle");
        return CRM_FAILURE;
      }
    }
    CNTLR_RCU_READ_LOCK_TAKE();

    ret_val = get_vm_byhandle(vm_handle,&crm_vm_node_p);
    if(ret_val != CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"Failed to get vm node\n");
      CNTLR_RCU_READ_LOCK_RELEASE();
      return CRM_FAILURE;
    }
  }
  if(crm_vm_node_p->no_of_ports == 0)
  {
    OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR,"No logical switch side ports attached");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return CRM_FAILURE;
  }
  ret_val = CRM_FAILURE;

  OF_LIST_SCAN((crm_vm_node_p->list_of_logical_switch_side_ports_connected),
                crm_vm_port_node_scan_entry_p,
                struct crm_vm_port *,
                CRM_VM_PORT_NODE_ENTRY_LISTNODE_OFFSET)
  {
    /* At present we assume only one port per Service VM.
       if(strcmp(crm_vm_port_node_scan_entry_p->vn_name, notification_data.launch.vm_name_p))
       continue;
    */

    ret_val = crm_get_port_byhandle(crm_vm_port_node_scan_entry_p->port_saferef,&port_info_p);
    
    if((ret_val == CRM_SUCCESS) && (port_info_p->crm_port_type == VMNS_PORT))    
    {
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"An attached VMNS_PORT is found."); 
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"crm vm port name = %s",port_info_p->port_name);
      OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"crm vm port name = %s",port_info_p->vmInfo.vm_name);
      break;
    }
    if(ret_val == CRM_SUCCESS)
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ERROR, "An attached VM_PORT is found.port_type = %d",port_info_p->crm_port_type);
      ret_val = CRM_FAILURE;
    }
  }
  if(ret_val != CRM_SUCCESS)
  {
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"failed to get vm port\n");
    CNTLR_RCU_READ_LOCK_RELEASE();
    return RRM_FAILURE;
  }
  else
  {
    *port_saferef_p = crm_vm_port_node_scan_entry_p->port_saferef;
    CNTLR_RCU_READ_LOCK_RELEASE();
  }
  return ret_val;
}
/****************************************************************************************************************************/
