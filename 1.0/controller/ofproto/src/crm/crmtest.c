#include "controller.h"
#include "dprm.h"
#include "crmapi.h"
#include "crm_vn_api.h"
#include "crm_port_api.h"

#define crm_debug printf
uint64_t  vm_handle1,vm_handle2,vm_handle3,vm_handle4,vm_handle5;
uint64_t  tenant_handle1,tenant_handle2,tenant_handle3,tenant_handle4,tenant_handle5;

struct crm_tenant_config_info  tenant_config_info1={};
struct crm_tenant_config_info  tenant_config_info2={};
struct crm_tenant_config_info  tenant_config_info={};

struct    crm_vm_config_info  vm_config_info1={}, vm_config_info2={};
struct    crm_vm_config_info  vm_config_info={};

struct    vm_runtime_info  runtime_info;
int32_t   lRet_value0,lRet_value1,lRet_value2,lRet_value3,lRet_value4,lRet_value5;

void printcrmhandle(uint64_t handle);

void printcrmretvalue(int32_t ret_value);

void crm_vm_notifications_cbk(uint32_t notification_type,
		uint64_t crm_vm_handle,
		struct    crm_vm_notification_data  notification_data,
		void     *callback_arg1,
		void     *callback_arg2);

void crm_test();

void getall_vms(void);
int32_t  crm_vm_test();


uint64_t crm_port_handle1,crm_port_handle2,crm_port_handle3,crm_port_handle4,crm_port_handle5;
uint64_t crm_port_handle6,crm_port_handle7,crm_port_handle8,crm_port_handle9,crm_port_handle10;
uint64_t crm_port_handle11;


void printvalues_vm()
{
  printf("\r\n Virtual Machine Name = %s",vm_config_info.vm_name);
  printf("\r\n Virtual Machin  Type = %d",vm_config_info.vm_type);

  printf("\r\n Virtual Machine Switch Name = %s",vm_config_info.switch_name);
  printf("\r\n Virtual Machine Tenant Name = %s",vm_config_info.tenant_name);

}

void printvalues_tenant()
{

  printf("\r\n Tenant Name = %s",tenant_config_info.tenant_name);
}
/*****************************************************************************************************/
void printcrmhandle(uint64_t handle)
{
  int ii;
  uchar8_t *crmhandle;

  printf(" handle = ");
  crmhandle =(uchar8_t *)(&handle);
  for(ii=0;ii<8;ii++)
    printf(" %x",crmhandle[ii]);
}

/******************************************************************************************************/
void printcrmretvalue(int32_t ret_value)
{
  if(ret_value == CRM_ERROR_INVALID_VM_TYPE)
    crm_debug("\r\n Virtual Machine Type Invalid!. \r\n");
  else if(ret_value == CRM_ERROR_DUPLICATE_VM_RESOURCE)
    crm_debug("\r\n Virtual Machine Type Duplicate!. \r\n");
  else if(ret_value == CRM_ERROR_VM_NAME_INVALID)
    crm_debug("\r\n Virtual Machine Name not in database!. \r\n");
  else if(ret_value == CRM_ERROR_VM_NAME_NULL)
    crm_debug("\r\n Virtual Machine Name null!. \r\n");
  else if(ret_value == CRM_ERROR_NO_MORE_ENTRIES)
    crm_debug("\r\n No More Entries in the database \r\n");
  else if(ret_value == CRM_ERROR_TENANT_NAME_INVALID)
    crm_debug("\r\n Tenant name invalid \r\n");
  else if(ret_value == CRM_ERROR_DUPLICATE_TENANT_RESOURCE)
    crm_debug("\r\n Tenant name duplicate \r\n");
  else if(ret_value == CRM_ERROR_TENANT_ADD_FAIL)
    crm_debug("\r\n Tenant add fail\r\n");

	
}

void crm_vm_notifications_cbk(uint32_t notification_type,
		uint64_t crm_vm_handle,
		struct    crm_vm_notification_data  notification_data,
		void     *callback_arg1,
		void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n CRM Virtial Machine Notificatio Call back invoked");

  printf("\r\n Virtual Machine Name  = %s",notification_data.vm_name);
  printf("\r\n crm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n CRM_VM_ALL_NOTIFICATIONS");
  if(notification_type == 1) 
    printf("\r\n CRM_VM_ADDED");
  if(notification_type == 2)
    printf("\r\n CRM_VM_DELETE");
  if(notification_type == 3)
    printf("\r\n CRM_VM_MODIFIED");

  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}




void crm_tenant_notification_cbk(uint32_t notification_type,
                                 uint64_t tenant_handle,
                                 struct  crm_tenant_notification_data  notification_data,
                                 void   *callback_arg1, 
                                 void  *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n CRM Tenant Notification Call back invoked");

  printf("\r\n Tenant Name  = %s",notification_data.tenant_name);
  printf("\r\n crm notification type = %d",notification_type);
  printcrmhandle(tenant_handle);

  if(notification_type == 0)
    printf("\r\n CRM_TENANT_ALL_NOTIFICATIONS");
  if(notification_type == 1) 
    printf("\r\n CRM_TENANT_ADDED");
  if(notification_type == 2)
    printf("\r\n CRM_TENANT_DELETE");
  if(notification_type == 3)
    printf("\r\n CRM_TENANT_MODIFIED");

  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");


}

void crm_test()
{
  crm_tenant_test();

  crm_vm_test();

  crm_vn_test();

  crm_computenode_test();

  crm_vmport_test();
  crm_get_vmport_view_db();
  crm_vmport_del_test();
  crm_get_vmport_view_db();


  crm_nwport_unicast_test();
  crm_get_nwport_unicast_view_db();
  crm_nwport_unicast_del_test();
  crm_get_nwport_unicast_view_db();

  crm_nwport_broadcast_add_test();
  crm_get_nwport_broadcast_view_db();
  crm_nwport_broadcast_del_test();
  crm_get_nwport_broadcast_view_db();
}









int32_t crm_tenant_test()
{
  int32_t ret_val;
  uint32_t  callback_arg1 = 2;
  uint32_t  callback_arg2 = 3;


  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM tenant  test begins\n");
  printf("\r\n*********************************************************************\n");
  printf("\r\n Adding tenant1.....\n");
  strcpy(tenant_config_info1.tenant_name, "tenant1");
  ret_val = crm_add_tenant(&tenant_config_info1,&tenant_handle1);
  printcrmretvalue(ret_val);
  if(ret_val==CRM_SUCCESS)
  {
    printcrmhandle(tenant_handle1);
    printf("\r\n tenant1 added successfully!.\n");
  }
  else
    printf("\r\n failed to add tenant1!.\n");

 return 0;
}

int32_t  crm_vm_test()
{
  uint32_t  callback_arg1 = 2;
  uint32_t  callback_arg2 = 3;
  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Machine  test begins\n");
  printf("\r\n*********************************************************************\n");

  printf("\r\n Adding virtual machine to DB = (VM1,VM_TYPE_NORMAL_APPLICATION,tenant1,SWITCH-1)\n");

  strcpy(vm_config_info1.vm_name, "VM1");
  strcpy(vm_config_info1.tenant_name,"tenant1");
  strcpy(vm_config_info1.switch_name,"SWITCH-1");
  vm_config_info1.vm_type = VM_TYPE_NORMAL_APPLICATION;
  lRet_value1 = crm_add_virtual_machine(&vm_config_info1, &vm_handle1);  
  printf("lRet_value1=:%d\n",lRet_value1);
  printcrmretvalue(lRet_value1);
  if(lRet_value1 == CRM_SUCCESS)
  {
    printcrmhandle(vm_handle1);
    printf("\r\n **** Virtual Machine Added to DB successfully****\n");
  }
  else
   printf("\r\n  Virtual Machine Added to DB failed\n"); 


  return 0;
}  
int32_t crm_vn_test()
{
  struct crm_vn_config_info config_info={};
  int32_t return_value;
  uint64_t crm_vn_handle;

  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network  test begins\n");
  printf("\r\n*********************************************************************\n");

  printf("\r\n Adding virtual network to DB = (VN1,tenant1,VXLNA_TYPE,10,5000)\n");

  strcpy(config_info.nw_name,"VN1");
  strcpy(config_info.tenant_name,"tenant1");
  config_info.nw_type=VXLAN_TYPE;
  config_info.nw_params.vxlan_nw.vxlan_vni=10;
  config_info.nw_params.vxlan_nw.vxlan_service_port=5000;
  return_value =crm_add_virtual_network(&config_info,  &crm_vn_handle);
  if (return_value != CRM_SUCCESS)
  {
    printf("\r\nfailed to add (VN1,tenant1,VXLNA_TYPE,10,5000)\n");
  }
  else
    printf("\r\n Virtual network added successfully\n");

  return CRM_SUCCESS;
}



int32_t crm_computenode_test()
{
  int32_t return_value = crm_add_crm_compute_node_to_vn("VN1", "SWITCH-1", "dp1");
  if (return_value != CRM_SUCCESS)
  {
    printf("\r\nfailed to add (VN1,SWITCH-1,dp1)\n");
  }
  else
    printf("\r\n compute node added successfully\n");
  return CRM_SUCCESS;
}
int32_t crm_nwport_unicast_del_test()
{
  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network Unicast Port del test begins\n");
  printf("\r\n*********************************************************************\n");

  int32_t ret_val;
  ret_val= crm_del_nwport_from_vxn_unicast_port_view(crm_port_handle6,1000);
  if(ret_val== CRM_SUCCESS)
  {
    printf("\r\n unicast port(eth6) view node deleted\n");
  }
  else
    printf("\r\n failed to delete unicast port (eth6)\n");

  ret_val= crm_del_nwport_from_vxn_unicast_port_view(crm_port_handle7,2000);
  if(ret_val== CRM_SUCCESS)
  {
    printf("\r\n unicast port(eth7) view node deleted\n");
  }
  else
    printf("\r\n failed to delete unicast port (eth7)\n");

  ret_val= crm_del_nwport_from_vxn_unicast_port_view(crm_port_handle8,3000);
  if(ret_val== CRM_SUCCESS)
  {
    printf("\r\n unicast port(eth8) view node deleted\n");
  }
  else
    printf("\r\n failed to delete unicast port (eth8)\n");




}
int32_t crm_nwport_unicast_test()
{
  int32_t ret_val;
  struct crm_nwport_config_info nwport_config_info ={};
  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network Unicast Port test begins\n");
  printf("\r\n*********************************************************************\n");

  printf("\r\n Adding virtual network nwport to DB = (eth6)\n");
  strcpy(nwport_config_info.port_name,"eth6");
  nwport_config_info.portId=111;
  nwport_config_info.nw_type=VXLAN_TYPE;
  strcpy(nwport_config_info.switch_name,"SWITCH-1");
  strcpy(nwport_config_info.br_name,"dp1");
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_vni=10;
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port=1000;
  nwport_config_info.nw_port_config_info.vxlan_info.remote_ip=0;
  ret_val = crm_add_crm_nwport(&nwport_config_info, &crm_port_handle6);
  if(ret_val== CRM_SUCCESS)
  {
    ret_val= crm_add_nwport_to_vxn_unicast_port_view(crm_port_handle6,1000);
    if(ret_val== CRM_SUCCESS)
    {
        printf("\r\n unicast port(eth6) view node added\n");
    }
    else
       printf("\r\n failed to add unicast port (eth6)\n");
  }
  else
    printf("\r\n failed to add network port eth6\n");


  printf("\r\n Adding virtual network nwport to DB = (eth7)\n");
  strcpy(nwport_config_info.port_name,"eth7");
  nwport_config_info.portId=222;
  nwport_config_info.nw_type=VXLAN_TYPE;
  strcpy(nwport_config_info.switch_name,"SWITCH-1");
  strcpy(nwport_config_info.br_name,"dp1");
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_vni=20;
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port=2000;
  nwport_config_info.nw_port_config_info.vxlan_info.remote_ip=0;
  ret_val = crm_add_crm_nwport(&nwport_config_info, &crm_port_handle7);
  if(ret_val== CRM_SUCCESS)
  {
    ret_val= crm_add_nwport_to_vxn_unicast_port_view(crm_port_handle7,2000);
    if(ret_val== CRM_SUCCESS)
    {
        printf("\r\n unicast port(eth7) view node added\n");
    }
    else
       printf("\r\n failed to add unicast port (eth7)\n");
  }
  else
    printf("\r\n failed to add network port eth7\n");

  printf("\r\n Adding virtual network nwport to DB = (eth8)\n");
  strcpy(nwport_config_info.port_name,"eth8");
  nwport_config_info.portId=333;
  nwport_config_info.nw_type=VXLAN_TYPE;
  strcpy(nwport_config_info.switch_name,"SWITCH-1");
  strcpy(nwport_config_info.br_name,"dp1");
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_vni=30;
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port=3000;
  nwport_config_info.nw_port_config_info.vxlan_info.remote_ip=0;
  ret_val = crm_add_crm_nwport(&nwport_config_info, &crm_port_handle8);
  if(ret_val== CRM_SUCCESS)
  {
    ret_val= crm_add_nwport_to_vxn_unicast_port_view(crm_port_handle8,3000);
    if(ret_val== CRM_SUCCESS)
    {
        printf("\r\n unicast port(eth8) view node added\n");
    }
    else
       printf("\r\n failed to add unicast port (eth8)\n");
  }
  else
    printf("\r\n failed to add network port eth8\n");



}

int32_t crm_get_nwport_unicast_view_db()
{
  int32_t ret_val;
  uint32_t port_id;

  printf("####################### crm_get_nwport_unicast_view_db #############################\n");
  ret_val = crm_get_vxn_unicast_nwport_by_swname_sp("SWITCH-1",1000,  &port_id);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "portid:%d\r\n",port_id);

  }
  else
    printf("\r\n failed to get view node\n");

  ret_val = crm_get_vxn_unicast_nwport_by_swname_sp("SWITCH-1",2000,  &port_id);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "portid:%d\r\n",port_id);

  }
  else
    printf("\r\n failed to get view node\n");

  ret_val = crm_get_vxn_unicast_nwport_by_swname_sp("SWITCH-1",3000,  &port_id);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "portid:%d\r\n",port_id);

  }
  else
    printf("\r\n failed to get view node\n");

  printf("####################################################\n");
  return CRM_SUCCESS;

}
int32_t crm_nwport_broadcast_del_test()
{
  int32_t ret_val;
  ret_val = crm_del_nwport_from_vxn_broadcast_port_view(crm_port_handle9,
      1000,
      0x01020304,
      10);
  if(ret_val== CRM_SUCCESS)
  {
    printf("\r\n broadcast port(eth9) view node deleted\n");
  }
  else
    printf("\r\n failed to delete broadcast port (eth9)\n");   

  ret_val = crm_del_nwport_from_vxn_broadcast_port_view(crm_port_handle10,
      2000,
      0x05060708,
      20);
  if(ret_val== CRM_SUCCESS)
  {
    printf("\r\n broadcast port(eth10) view node deleted\n");
  }
  else
    printf("\r\n failed to delete broadcast port (eth10)\n");   

  ret_val = crm_del_nwport_from_vxn_broadcast_port_view(crm_port_handle11,
      3000,
      0x09010203,
      30);
  if(ret_val== CRM_SUCCESS)
  {
    printf("\r\n broadcast port(eth11) view node deleted\n");
  }
  else
    printf("\r\n failed to delete broadcast port (eth11)\n");   


}
int32_t crm_nwport_broadcast_add_test()
{
  int32_t ret_val;
  uint64_t crm_port_handle=0;
  struct crm_nwport_config_info nwport_config_info ={};
  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network  Broadcast Port Test Begins\n");
  printf("\r\n*********************************************************************\n");
  printf("\r\n Adding virtual network broadcast nwport to DB = (eth9)\n");
  strcpy(nwport_config_info.port_name,"eth9");
  nwport_config_info.portId=111;
  nwport_config_info.nw_type=VXLAN_TYPE;
  strcpy(nwport_config_info.switch_name,"SWITCH-1");
  strcpy(nwport_config_info.br_name,"dp1");
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_vni=10;
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port=1000;
  nwport_config_info.nw_port_config_info.vxlan_info.remote_ip=0x01020304;
  ret_val = crm_add_crm_nwport(&nwport_config_info, &crm_port_handle9);
  if(ret_val== CRM_SUCCESS)
  {
    ret_val= crm_add_nwport_to_vxn_broadcast_port_view(crm_port_handle9,
                                                       nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port,
                                                       10,
                                                       nwport_config_info.nw_port_config_info.vxlan_info.remote_ip);
    if(ret_val== CRM_SUCCESS)
    {
        printf("\r\n broadcast port(eth9) view node added\n");
    }
    else
       printf("\r\n failed to add broadcast port (eth9)\n");   
 }
  else
    printf("\r\n failed to add network port eth9\n");




  printf("\r\n Adding virtual network broadcast nwport to DB = (eth10)\n");
  strcpy(nwport_config_info.port_name,"eth10");
  nwport_config_info.portId=222;
  nwport_config_info.nw_type=VXLAN_TYPE;
  strcpy(nwport_config_info.switch_name,"SWITCH-1");
  strcpy(nwport_config_info.br_name,"dp1");
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_vni=20;
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port=2000;
  nwport_config_info.nw_port_config_info.vxlan_info.remote_ip=0x05060708;
  ret_val = crm_add_crm_nwport(&nwport_config_info, &crm_port_handle10);
  if(ret_val== CRM_SUCCESS)
  {
    ret_val= crm_add_nwport_to_vxn_broadcast_port_view(crm_port_handle10,
                                                       nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port,
                                                       20,
                                                       nwport_config_info.nw_port_config_info.vxlan_info.remote_ip);
    if(ret_val== CRM_SUCCESS)
    {
        printf("\r\n broadcast port(eth10) view node added\n");
    }
    else
       printf("\r\n failed to add broadcast port (eth10)\n");
  }
  else
    printf("\r\n failed to add network broadcast port eth10\n");


  printf("\r\n Adding virtual network broadcast nwport to DB = (eth11)\n");
  strcpy(nwport_config_info.port_name,"eth11");
  nwport_config_info.portId=333;
  nwport_config_info.nw_type=VXLAN_TYPE;
  strcpy(nwport_config_info.switch_name,"SWITCH-1");
  strcpy(nwport_config_info.br_name,"dp1");
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_vni=30;
  nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port=3000;
  nwport_config_info.nw_port_config_info.vxlan_info.remote_ip=0x09010203;
  ret_val = crm_add_crm_nwport(&nwport_config_info, &crm_port_handle11);
  if(ret_val== CRM_SUCCESS)
  {
    ret_val= crm_add_nwport_to_vxn_broadcast_port_view(crm_port_handle11,
                                                       nwport_config_info.nw_port_config_info.vxlan_info.vxlan_service_port,
                                                       30,
                                                       nwport_config_info.nw_port_config_info.vxlan_info.remote_ip);
    if(ret_val== CRM_SUCCESS)
    {
        printf("\r\n broadcast port(eth11) view node added\n");
    }
    else
       printf("\r\n failed to add broadcast port (eth11)\n");
  }
  else
    printf("\r\n failed to add network port eth11\n");

}

int32_t crm_get_nwport_broadcast_view_db()
{
  int32_t ret_val;
  uint32_t port_ids[10];
  uint8_t no_of_ports=10;

  printf("####################### crm_get_nwport_broadcast_view_db #############################\n");
  ret_val = crm_get_vxn_broadcast_nwports_by_swname_vni_sp("SWITCH-1",1000, 10, &no_of_ports, port_ids);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "no_of_ports:%d,portid:%d\r\n",no_of_ports,port_ids[0]);

  }
  else
    printf("\r\n failed to get view node\n");

   no_of_ports=10;
  ret_val = crm_get_vxn_broadcast_nwports_by_swname_vni_sp("SWITCH-1",2000,  20, &no_of_ports,   port_ids);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "no_of_ports:%d,portid:%d\r\n",no_of_ports,port_ids[0]);

  }
  else
    printf("\r\n failed to get view node\n");

   no_of_ports=10;
  ret_val = crm_get_vxn_broadcast_nwports_by_swname_vni_sp("SWITCH-1",3000, 30, &no_of_ports,port_ids);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "no_of_ports:%d,portid:%d\r\n",no_of_ports,port_ids[0]);

  }
  else
    printf("\r\n failed to get view node\n");

  printf("####################################################\n");
  return CRM_SUCCESS;


}

int32_t crm_vmport_del_test()
{
  int32_t ret_val;
  printf("####################################################\n");
  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network VM Port Delete Test Begins\n");
  printf("\r\n*********************************************************************\n");

  printf("\r\n Deleting eth1 view node from dmac view\n");
  ret_val = crm_del_vmport_from_vmside_portmac_view(crm_port_handle1,10);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth1) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth1) from the db view\n");

  printf("\r\n Deleting eth1 view node from swname view\n");
  ret_val = crm_del_vmport_from_vmside_swname_view(crm_port_handle1,10);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth1) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth1) from the db view\n"); 

  printf("\r\n Deleting eth2 view node from dmac view\n");
  ret_val = crm_del_vmport_from_vmside_portmac_view(crm_port_handle2,20);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth1) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth1) from the db view\n");
  printf("\r\n Deleting eth2 view node from swname view\n");
  ret_val = crm_del_vmport_from_vmside_swname_view(crm_port_handle2,20);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth2) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth2) from the db view\n"); 

  printf("\r\n Deleting eth3 view node from dmac view\n");
  ret_val = crm_del_vmport_from_vmside_portmac_view(crm_port_handle3,30);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth3) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth3) from the db view\n");
  printf("\r\n Deleting eth3 view node from swname view\n");
  ret_val = crm_del_vmport_from_vmside_swname_view(crm_port_handle3,30);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth3) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth3) from the db view\n"); 

  printf("\r\n Deleting eth4 view node from dmac view\n");
  ret_val = crm_del_vmport_from_vmside_portmac_view(crm_port_handle4,40);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth4) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth4) from the db view\n");
  printf("\r\n Deleting eth4 view node from swname view\n");
  ret_val = crm_del_vmport_from_vmside_swname_view(crm_port_handle4,40);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth4) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth4) from the db view\n"); 


  printf("\r\n Deleting eth5 view node from dmac view\n");
  ret_val = crm_del_vmport_from_vmside_portmac_view(crm_port_handle5,50);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth5) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth5) from the db view\n");
  printf("\r\n Deleting eth5 view node from swname view\n");
  ret_val = crm_del_vmport_from_vmside_swname_view(crm_port_handle5,50);
  if(ret_val==CRM_SUCCESS)
    printf("\r\n vm port(eth5) view node deleted successfully\n");
  else
    printf("\r\n failed to delete view node(eth5) from the db view\n"); 

  printf("####################################################\n");

}

int32_t crm_vmport_test()
{
  int32_t ret_val;
  uint8_t  vm_port_mac[6];
  char mac_buf[20];

  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network VM Port Test Begins\n");
  printf("\r\n*********************************************************************\n");

  printf("\r\n Adding virtual network vmport to DB = (eth1)\n");
  strcpy(mac_buf,"11:11:11:11:11:11");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);

  ret_val = crm_add_crm_vmport("eth1","SWITCH-1","dp1",VM_PORT,"VN1","VM1",vm_port_mac,&crm_port_handle1);
  if(ret_val == CRM_SUCCESS)
  {
    printf("\r\n vmport(eth1) added successfully\n");
    printf("\r\n crm virtual network vm port(eth1) dmac view test begins\n");
    ret_val = tsc_vxn_add_vmport_to_portmac_view(crm_port_handle1,10);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth1) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth1) to the db view\n");


    printf("\r\n crm virtual network vm port(eth1) switch name view test begins\n");
    ret_val = crm_add_vmport_to_vmside_swname_view(crm_port_handle1,10);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth1) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth1) to the db view\n"); 

   }
  else
    printf("\r\n Failed to add vmport(eth1)\n");




  printf("\r\n Adding virtual network vmport to DB = (eth2)\n");
  strcpy(mac_buf,"22:22:22:22:22:22");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  ret_val = crm_addh_crm_vmport("eth2","SWITCH-1","dp1",VM_PORT,"VN1","VM1",vm_port_mac,&crm_port_handle2);
  if(ret_val == CRM_SUCCESS)
  {
    printf("\r\n vmport(eth2) added successfully\n");
    printf("\r\n CRM Virtual Network VM PORT(eth2) DMAC VIEW test begins\n");
    ret_val = tsc_vxn_add_vmport_to_portmac_view(crm_port_handle2,20);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth2) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth2) to the db view\n"); 

    printf("\r\n CRM Virtual Network VM PORT(eth2) SWITCH NAME VIEW test begins\n");
    ret_val = crm_add_vmport_to_vmside_swname_view(crm_port_handle2,20);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth2) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth2) to the db view\n");
  }
  else
    printf("\r\n Failed to add vmport(eth2)\n");



  printf("\r\n Adding virtual network vmport to DB = (eth3)\n");
  strcpy(mac_buf,"33:33:33:33:33:33");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  ret_val = crm_add_crm_vmport("eth3","SWITCH-1","dp1",VM_PORT, "VN1","VM1",vm_port_mac,&crm_port_handle3);
  if(ret_val == CRM_SUCCESS)
  {
    printf("\r\n vmport(eth3) added successfully\n");
    printf("\r\n CRM Virtual Network VM PORT(eth3) DMAC VIEW test begins\n");
    ret_val = tsc_vxn_add_vmport_to_portmac_view(crm_port_handle3,30);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth3) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth3) to the db view\n");

    printf("\r\n CRM Virtual Network VM PORT(eth3) SWITCH NAME VIEW test begins\n");
    ret_val = crm_add_vmport_to_vmside_swname_view(crm_port_handle3,30);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth3) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth3) to the db view\n"); 
  }
  else
    printf("\r\n Failed to add vmport(eth3)\n");

  printf("\r\n Adding virtual network vmport to DB = (eth4)\n");
  strcpy(mac_buf,"44:44:44:44:44:44");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  ret_val = crm_add_crm_vmport("eth4","SWITCH-1","dp1",VM_PORT, "VN1","VM1",vm_port_mac,&crm_port_handle4);
  if(ret_val == CRM_SUCCESS)
  {
    printf("\r\n vmport(eth4) added successfully\n");
    printf("\r\n CRM Virtual Network VM PORT(eth4) DMAC VIEW test begins\n");
    ret_val = tsc_vxn_add_vmport_to_portmac_view(crm_port_handle4,40);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port view node(eth4) added successfully\n");
    else
      printf("\r\n failed to add view node(eth4) to the db view\n");


    printf("\r\n CRM Virtual Network VM PORT(eth4) SWITCH NAME VIEW test begins\n");
    ret_val = crm_add_vmport_to_vmside_swname_view(crm_port_handle4,40);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth4) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth4) to the db view\n"); 

  }
  else
    printf("\r\n Failed to add vmport(eth4)\n");





  printf("\r\n Adding virtual network vmport to DB = (eth5)\n");
  strcpy(mac_buf,"55:55:55:55:55:55");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  ret_val = crm_add_crm_vmport("eth5","SWITCH-1","dp1",VM_PORT, "VN1","VM1",vm_port_mac,&crm_port_handle5);
  if(ret_val == CRM_SUCCESS)
  {
    printf("\r\n vmport(eth5) added successfully\n");
    printf("\r\n CRM Virtual Network VM PORT(eth5) DMAC VIEW test begins\n");
    ret_val = tsc_vxn_add_vmport_to_portmac_view(crm_port_handle5,50);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port view node(eth5) added successfully\n");
    else
      printf("\r\n failed to add view node(eth5) to the db view\n");
    printf("\r\n CRM Virtual Network VM PORT(eth5) SWITCH NAME VIEW test begins\n");
    ret_val = crm_add_vmport_to_vmside_swname_view(crm_port_handle5,50);
    if(ret_val==CRM_SUCCESS)
      printf("\r\n vm port(eth5) view node added successfully\n");
    else
      printf("\r\n failed to add view node(eth5) to the db view\n"); 

  }
  else
    printf("\r\n Failed to add vmport(eth5)\n");

}
int32_t crm_get_vmport_view_db()
{
  struct crm_port *crm_port_node_p1=NULL;
  struct crm_port *crm_port_node_p2=NULL;
  struct crm_port *crm_port_node_p3=NULL;
  struct crm_port *crm_port_node_p4=NULL;
  struct crm_port *crm_port_node_p5=NULL;
  struct crm_port *crm_port_node_p6=NULL;
  uint8_t vm_port_mac[6];
  int32_t ret_val;
  uint8_t no_of_vmside_ports;
  int32_t vmside_ports_id[100];
  char mac_buf[20];

  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network get vm port view test begins\n");
  printf("\r\n*********************************************************************\n");


  strcpy(mac_buf,"11:11:11:11:11:11");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  printf("\r\n*********************************************************************\n");
  printf("\r\n CRM Virtual Network get  view node from dmac view(11:11:11:11:11:11,10)\n");
  ret_val = tsc_vxn_get_vmport_by_dmac_nid(vm_port_mac,10, &crm_port_node_p1);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "port_name:%s,port_type:%d,vm_name:%s,br_name:%s,portId:%d\n",crm_port_node_p1->port_name,crm_port_node_p1->crm_port_type,crm_port_node_p1->vmInfo.vm_name,crm_port_node_p1->br1_name,crm_port_node_p1->port_id);

  }
  else
    printf("\r\n failed to get view node from the dmac view(11:11:11:11:11:11,10)\n");
  printf("\r\n CRM Virtual Network get  view node from swname-vni view(SWITCH-1,10)\n");
  no_of_vmside_ports=10;
  ret_val = crm_get_vxn_vmports_by_swname_vni("SWITCH-1",10, &no_of_vmside_ports, &vmside_ports_id);
  if(ret_val==CRM_SUCCESS)
  {
  printf( "no_of_vmside_ports:%d,vmside_ports_id[0]:%d\r\n",no_of_vmside_ports,vmside_ports_id[0]);

  }
  else
    printf("\r\n failed to get view node from the dmac view(SWITCH-1,10)\n");

  printf("\r\n*********************************************************************\n");
  strcpy(mac_buf,"22:22:22:22:22:22");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  printf("\r\n CRM Virtual Network get  view node from dmac view (22:22:22:22:22:22,20)\n");
  ret_val = tsc_vxn_get_vmport_by_dmac_nid(vm_port_mac,20, &crm_port_node_p2);
  if(ret_val==CRM_SUCCESS)
  {

    printf( "port_name:%s,port_type:%d,vm_name:%s,br_name:%s,portId:%d",crm_port_node_p2->port_name,crm_port_node_p2->crm_port_type,crm_port_node_p2->vmInfo.vm_name,crm_port_node_p2->br1_name,crm_port_node_p2->port_id);
  }
  else
    printf("\r\n failed to get view node from the dmac view(22:22:22:22:22:22,20)\n");

  printf("\r\n CRM Virtual Network get  view node from swname-vni view(SWITCH-1,20)\n");
  no_of_vmside_ports=1;
  ret_val = crm_get_vxn_vmports_by_swname_vni("SWITCH-1",20, &no_of_vmside_ports,vmside_ports_id);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "no_of_vmside_ports:%d,vmside_ports_id[0]:%d",no_of_vmside_ports,vmside_ports_id[0]);

  }
  else
    printf("\r\n failed to get view node from the dmac view(SWITCH-1,20)\n");

  printf("\r\n*********************************************************************\n");
  strcpy(mac_buf,"33:33:33:33:33:33");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  printf("\r\n CRM Virtual Network get  view node from dmac view (33:33:33:33:33:33,30)\n");
  ret_val = tsc_vxn_get_vmport_by_dmac_nid(vm_port_mac,30, &crm_port_node_p3);
  if(ret_val==CRM_SUCCESS)
  {

    printf( "port_name:%s,port_type:%d,vm_name:%s,br_name:%s,portId:%d",crm_port_node_p3->port_name,crm_port_node_p3->crm_port_type,crm_port_node_p3->vmInfo.vm_name,crm_port_node_p3->br1_name,crm_port_node_p3->port_id);
  }
  else
    printf("\r\n failed to get view node from the dmac view(33:33:33:33:33:33,30)\n");
  printf("\r\n CRM Virtual Network get  view node from swname-vni view(SWITCH-1,30)\n");
  no_of_vmside_ports=1;
  ret_val = crm_get_vxn_vmports_by_swname_vni("SWITCH-1",30, &no_of_vmside_ports,vmside_ports_id);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "no_of_vmside_ports:%d,vmside_ports_id[0]:%d",no_of_vmside_ports,vmside_ports_id[0]);

  }
  else
    printf("\r\n failed to get view node from the dmac view(SWITCH-1,30)\n");

  printf("\r\n*********************************************************************\n");
  strcpy(mac_buf,"44:44:44:44:44:44");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  printf("\r\n CRM Virtual Network get  view node from dmac view (44:44:44:44:44:44,40)\n");
  ret_val = tsc_vxn_get_vmport_by_dmac_nid(vm_port_mac,40, &crm_port_node_p4);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "port_name:%s,port_type:%d,vm_name:%s,br_name:%s,portId:%d",crm_port_node_p4->port_name,crm_port_node_p4->crm_port_type,crm_port_node_p4->vmInfo.vm_name,crm_port_node_p4->br1_name,crm_port_node_p4->port_id);

  }
  else
    printf("\r\n failed to get view node from the dmac view(44:44:44:44:44:44,40)\n");

  printf("\r\n CRM Virtual Network get  view node from swname-vni view(SWITCH-1,40)\n");
  no_of_vmside_ports=1;
  ret_val = crm_get_vxn_vmports_by_swname_vni("SWITCH-1",40, &no_of_vmside_ports,vmside_ports_id);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "no_of_vmside_ports:%d,vmside_ports_id[0]:%d",no_of_vmside_ports,vmside_ports_id[0]);

  }
  else
    printf("\r\n failed to get view node from the dmac view(SWITCH-1,40)\n");


  printf("\r\n*********************************************************************\n");
  strcpy(mac_buf,"55:55:55:55:55:55");
  of_flow_match_atox_mac_addr(mac_buf,vm_port_mac);
  printf("\r\n CRM Virtual Network get  view node from dmac view (55:55:55:55:55:55,50)\n");
  ret_val = tsc_vxn_get_vmport_by_dmac_nid(vm_port_mac,50, &crm_port_node_p4);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "port_name:%s,port_type:%d,vm_name:%s,br_name:%s,portId:%d",crm_port_node_p4->port_name,crm_port_node_p4->crm_port_type,crm_port_node_p4->vmInfo.vm_name,crm_port_node_p4->br1_name,crm_port_node_p4->port_id);

  }
  else
    printf("\r\n failed to get view node from the dmac view(55:55:55:55:55:55,50)\n");

  printf("\r\n CRM Virtual Network get  view node from swname-vni view(SWITCH-1,50)\n");
  no_of_vmside_ports=1;
  ret_val = crm_get_vxn_vmports_by_swname_vni("SWITCH-1",50, &no_of_vmside_ports,vmside_ports_id);
  if(ret_val==CRM_SUCCESS)
  {
    printf( "no_of_vmside_ports:%d,vmside_ports_id[0]:%d",no_of_vmside_ports,vmside_ports_id[0]);

  }
  else
    printf("\r\n failed to get view node from the dmac view(SWITCH-1,50)\n");

  printf("\r\n*********************************************************************\n");
}
#if 0
/************************************************************************************/
void getall_vms(void)
{
  int32_t lRet_value0;

  printf("\r\n****************************************************************");
  printf("\r\n Get all the VMs\n");

  lRet_value0 =  crm_get_first_vm(&vm_config_info, &runtime_info, &vm_handle1);
  printcrmretvalue(lRet_value0);
  if(lRet_value0 == CRM_SUCCESS)
  {
    printvalues_vm();
    printcrmhandle(vm_handle1);
  }
  for(lRet_value0 = CRM_SUCCESS;lRet_value0 == CRM_SUCCESS;)
  {
    printf("\r\n crmtest - Get the next Datapath\n");
    lRet_value0 = crm_get_next_vm(&vm_config_info, &runtime_info,&vm_handle1);
    printcrmretvalue(lRet_value0);
    if(lRet_value0 == CRM_SUCCESS)
    {
      printvalues_vm();
      printcrmhandle(vm_handle1);
    }
    else
    {
      if(lRet_value0 == CRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n crm test -  No more crms found\n");
      }
      else if(lRet_value0 == CRM_ERROR_INVALID_VM_HANDLE)
      {
        printf("\r\n crm test - Invalid handle\n");
      }
      break;
    }
  }
}

/************************************************************************************/
void getall_tenants(void)
{
  int32_t lRet_value0;

  printf("\r\n****************************************************************");
  printf("\r\n Get all the tenants\n");

  lRet_value0 =  crm_get_first_tenant(&tenant_config_info, &runtime_info, &tenant_handle1);
  printcrmretvalue(lRet_value0);
  if(lRet_value0 == CRM_SUCCESS)
  {
    printvalues_tenant();
    printcrmhandle(tenant_handle1);
  }
  for(lRet_value0 = CRM_SUCCESS;lRet_value0 == CRM_SUCCESS;)
  {
    printf("\r\n crmtest - Get the next tenant\n");
    lRet_value0 = crm_get_next_tenant(&tenant_config_info, &runtime_info,&tenant_handle1);
    printcrmretvalue(lRet_value0);
    if(lRet_value0 == CRM_SUCCESS)
    {
      printvalues_tenant();
      printcrmhandle(tenant_handle1);
    }
    else
    {
      if(lRet_value0 == CRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n crm test -  No more crms found\n");
      }
      else if(lRet_value0 == CRM_ERROR_INVALID_VM_HANDLE)
      {
        printf("\r\n crm test - Invalid handle\n");
      }
      break;
    }
  }
}
#endif
