#include "controller.h"
#include "nsrm.h"
#include "nsrmldef.h"
//#include "nsrm_internal.h"



#define nsrm_debug printf

uint64_t appl_handle15,appl_handle1,appl_handle2,appl_handle3,appl_handle4,appl_handle5,appl_handle6,appl_handle7,appl_handle8,appl_handle9,appl_handle10,appl_handle11,appl_handle12,appl_handle13,appl_handle14,appl_handle16,appl_handle17;
struct nsrm_nwservice_object_key          nwservice_key_object1 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nwservice_object_config_info  nwservice_config_object1[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nwservice_object_key          nwservice_key_object2 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nwservice_object_config_info  nwservice_config_object2[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nwservice_object_key          nwservice_key_object3 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nwservice_object_config_info  nwservice_config_object3[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nwservice_object_key          nwservice_key_object5 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nwservice_object_config_info  nwservice_config_object5[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nwservice_object_key          nwservice_key_object6 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nwservice_object_config_info  nwservice_config_object6[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschain_object_key          nschain_key_object1 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschain_object_config_info  nschain_config_object1[5] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschain_object_key          nschain_key_object2 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschain_object_config_info  nschain_config_object2[5] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschain_object_key          nschain_key_object3 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschain_object_config_info  nschain_config_object3[5] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschain_object_key          nschain_key_object4 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschain_object_config_info  nschain_config_object4[5] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschain_object_key          nschain_key_object5 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschain_object_config_info  nschain_config_object5[5] = {};// {"uuida2","appl2","abcffd",0};


struct nsrm_nschainset_object_key          nschainset_key_object1 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschainset_object_config_info  nschainset_config_object1[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschainset_object_key          nschainset_key_object2 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschainset_object_config_info  nschainset_config_object2[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschainset_object_key          nschainset_key_object3 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschainset_object_config_info  nschainset_config_object3[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschainset_object_key          nschainset_key_object4 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschainset_object_config_info  nschainset_config_object4[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_nschainset_object_key          nschainset_key_object5 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_nschainset_object_config_info  nschainset_config_object5[2] = {};// {"uuida2","appl2","abcffd",0};


struct nsrm_l2nw_service_map_key          l2nw_key_object1 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_l2nw_service_map_config_info  l2nw_config_object1[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_l2nw_service_map_key          l2nw_key_object2 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_l2nw_service_map_config_info  l2nw_config_object2[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_l2nw_service_map_key          l2nw_key_object3 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_l2nw_service_map_config_info  l2nw_config_object3[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_l2nw_service_map_key          l2nw_key_object4 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_l2nw_service_map_config_info  l2nw_config_object4[2] = {};// {"uuida2","appl2","abcffd",0};
struct nsrm_l2nw_service_map_key          l2nw_key_object5 ={};// {"uuida1","appl1","abcd",1};
struct nsrm_l2nw_service_map_config_info  l2nw_config_object5[2] = {};// {"uuida2","appl2","abcffd",0};


struct nsrm_nschain_selection_rule_key               key_info1 = {};
struct nsrm_nschain_selection_rule_config_info       config_info1[11] = {};

struct nsrm_nschain_selection_rule_key               key_info2 = {};
struct nsrm_nschain_selection_rule_config_info       config_info2[11] = {};

struct nsrm_nschain_selection_rule_key               key_info3 = {};
struct nsrm_nschain_selection_rule_config_info       config_info3[11] = {};

struct nsrm_nschain_selection_rule_key               key_info4 = {};
struct nsrm_nschain_selection_rule_config_info       config_info4[11] = {};

struct nsrm_nschain_selection_rule_key               key_info5 = {};
struct nsrm_nschain_selection_rule_config_info       config_info5[11] = {};

struct nsrm_nschain_selection_rule_key               key_info6 = {};
struct nsrm_nschain_selection_rule_config_info       config_info6[11] = {};



struct nsrm_nwservice_bypass_rule_key               key_infob1 = {};
struct nsrm_nwservice_bypass_rule_config_info       config_infob1[12] = {};

struct nsrm_nwservice_bypass_rule_key               key_infob2 = {};
struct nsrm_nwservice_bypass_rule_config_info       config_infob2[12] = {};

struct nsrm_nwservice_bypass_rule_key               key_infob3 = {};
struct nsrm_nwservice_bypass_rule_config_info       config_infob3[12] = {};

struct nsrm_nwservice_bypass_rule_key               key_infob4 = {};
struct nsrm_nwservice_bypass_rule_config_info       config_infob4[12] = {};



void printnsrmhandle(uint64_t handle);
void printnsrmretvalue(int32_t ret_value);


void nsrm_test();


int32_t nsrm_appl_test();
int32_t nsrm_nschain_test();


void printvalues_appl()
{
  printf("\r\n nws name1    : %s ", nwservice_key_object1.name_p);
  printf("\r\n nws tenant1  : %s ", nwservice_key_object1.tenant_name_p );
  printf("\r\n form factor1 : %d ",nwservice_config_object1[0].value.nwservice_object_form_factor_type_e );
  printf("\r\n admin status1: %d ", nwservice_config_object1[1].value.admin_status_e);

  printf("\r\n nws name2    : %s ", nwservice_key_object2.name_p);
  printf("\r\n nws tenant2  : %s ", nwservice_key_object2.tenant_name_p );
  printf("\r\n form factor2 : %d ",nwservice_config_object2[0].value.nwservice_object_form_factor_type_e );
  printf("\r\n admin status2: %d ", nwservice_config_object2[1].value.admin_status_e);

  printf("\r\n nws name3    : %s ", nwservice_key_object3.name_p);
  printf("\r\n nws tenant3  : %s ", nwservice_key_object3.tenant_name_p );
  printf("\r\n form factor3 : %d ",nwservice_config_object3[0].value.nwservice_object_form_factor_type_e );
  printf("\r\n admin status3: %d ", nwservice_config_object3[1].value.admin_status_e);

  printf("\r\n nws name4    : %s ", nwservice_key_object5.name_p);
  printf("\r\n nws tenant4  : %s ", nwservice_key_object5.tenant_name_p );
  printf("\r\n form factor4 : %d ",nwservice_config_object5[0].value.nwservice_object_form_factor_type_e );
  printf("\r\n admin status4: %d ", nwservice_config_object5[1].value.admin_status_e);

  printf("\r\n nws name5    : %s ", nwservice_key_object6.name_p);
  printf("\r\n nws tenant5  : %s ", nwservice_key_object6.tenant_name_p );
  printf("\r\n form factor  : %d ",nwservice_config_object6[0].value.nwservice_object_form_factor_type_e );
  printf("\r\n admin status5: %d ", nwservice_config_object6[1].value.admin_status_e);

  printf("\r\n ncs name1    : %s ", nschain_key_object1.name_p);
  printf("\r\n ncs tenant1  : %s ", nschain_key_object1.tenant_name_p );
  printf("\r\n chain type   : %d ", nschain_config_object1[0].value.nschain_type );
  printf("\r\n load sharing : %d ", nschain_config_object1[1].value.load_sharing_algorithm_e );
  printf("\r\n indication   : %d ", nschain_config_object1[2].value.load_indication_type_e );
  printf("\r\n high threshold: %d",nschain_config_object1[3].value.high_threshold );
  printf("\r\n low threshold: %d ", nschain_config_object1[4].value.low_threshold );
  printf("\r\n admin status1: %d ", nschain_config_object1[5].value.admin_status_e);

 
  printf("\r\n ncs name2    : %s ", nschain_key_object2.name_p);
  printf("\r\n ncs tenant2  : %s ", nschain_key_object2.tenant_name_p );
  printf("\r\n chain type   : %d ", nschain_config_object2[0].value.nschain_type );
  printf("\r\n load sharing : %d ", nschain_config_object2[1].value.load_sharing_algorithm_e );
  printf("\r\n indication   : %d ", nschain_config_object2[2].value.load_indication_type_e );
  printf("\r\n high threshold: %d",nschain_config_object2[3].value.high_threshold );
  printf("\r\n low threshold: %d ", nschain_config_object2[4].value.low_threshold );
  printf("\r\n admin status2: %d ", nschain_config_object2[5].value.admin_status_e);


  printf("\r\n ncs name3    : %s ", nschain_key_object3.name_p);
  printf("\r\n ncs tenant3  : %s ", nschain_key_object3.tenant_name_p );
  printf("\r\n chain type   : %d ", nschain_config_object3[0].value.nschain_type );
  printf("\r\n load sharing : %d ", nschain_config_object3[1].value.load_sharing_algorithm_e );
  printf("\r\n indication   : %d ", nschain_config_object3[2].value.load_indication_type_e );
  printf("\r\n high threshold: %d",nschain_config_object3[3].value.high_threshold );
  printf("\r\n low threshold: %d ", nschain_config_object3[4].value.low_threshold );
  printf("\r\n admin status1: %d ", nschain_config_object3[5].value.admin_status_e);

  printf("\r\n ncs name4    : %s ", nschain_key_object4.name_p);
  printf("\r\n ncs tenant4  : %s ", nschain_key_object4.tenant_name_p );
  printf("\r\n chain type   : %d ", nschain_config_object4[0].value.nschain_type );
  printf("\r\n load sharing : %d ", nschain_config_object4[1].value.load_sharing_algorithm_e );
  printf("\r\n indication   : %d ", nschain_config_object4[2].value.load_indication_type_e );
  printf("\r\n high threshold: %d",nschain_config_object4[3].value.high_threshold );
  printf("\r\n low threshold: %d ", nschain_config_object4[4].value.low_threshold );
  printf("\r\n admin status4: %d ", nschain_config_object4[5].value.admin_status_e);

  printf("\r\n ncs name5    : %s ", nschain_key_object5.name_p);
  printf("\r\n ncs tenant5  : %s ", nschain_key_object5.tenant_name_p );
  printf("\r\n chain type   : %d ", nschain_config_object5[0].value.nschain_type );
  printf("\r\n load sharing : %d ", nschain_config_object5[1].value.load_sharing_algorithm_e );
  printf("\r\n indication   : %d ", nschain_config_object5[2].value.load_indication_type_e );
  printf("\r\n high threshold: %d",nschain_config_object5[3].value.high_threshold );
  printf("\r\n low threshold: %d ", nschain_config_object5[4].value.low_threshold );
  printf("\r\n admin status4: %d ", nschain_config_object5[5].value.admin_status_e);

  

  printf("\r\n ncs name1    : %s ", nschainset_key_object1.name_p);
  printf("\r\n ncs tenant1  : %s ", nschainset_key_object1.tenant_name_p );
  printf("\r\n chain type   : %d ", nschainset_config_object1[0].value.nschainset_type );
   printf("\r\n admin status1: %d ", nschainset_config_object1[1].value.admin_status_e);

 
  printf("\r\n ncs name2    : %s ", nschainset_key_object2.name_p);
  printf("\r\n ncs tenant2  : %s ", nschainset_key_object2.tenant_name_p );
  printf("\r\n chain type   : %d ", nschainset_config_object2[0].value.nschainset_type );
   printf("\r\n admin status2: %d ", nschainset_config_object2[1].value.admin_status_e);


  printf("\r\n ncs name3    : %s ", nschainset_key_object3.name_p);
  printf("\r\n ncs tenant3  : %s ", nschainset_key_object3.tenant_name_p );
  printf("\r\n chain type   : %d ", nschainset_config_object3[0].value.nschainset_type );
   printf("\r\n admin status1: %d ", nschainset_config_object3[1].value.admin_status_e);

  printf("\r\n ncs name4    : %s ", nschainset_key_object4.name_p);
  printf("\r\n ncs tenant4  : %s ", nschainset_key_object4.tenant_name_p );
  printf("\r\n chain type   : %d ", nschainset_config_object4[0].value.nschainset_type );
  printf("\r\n admin status4: %d ", nschainset_config_object4[1].value.admin_status_e);

  printf("\r\n ncs name5    : %s ", nschainset_key_object5.name_p);
  printf("\r\n ncs tenant5  : %s ", nschainset_key_object5.tenant_name_p );
  printf("\r\n chain type   : %d ", nschainset_config_object5[0].value.nschainset_type );
   printf("\r\n admin status4: %d ", nschainset_config_object5[1].value.admin_status_e);

 
 printf("\r\n      name1    : %s ", l2nw_key_object1.map_name_p);
  printf("\r\n    tenant1  : %s ",  l2nw_key_object1.tenant_name_p );
  printf("\r\n vn name     : %s ", l2nw_key_object1.vn_name_p );
  printf("\r\n chain set name   : %s ", l2nw_config_object1[0].value.nschainset_object_name_p );
   printf("\r\n admin status1: %d ", l2nw_config_object1[1].value.admin_status_e);


 printf("\r\n      name2    : %s ", l2nw_key_object2.map_name_p);
  printf("\r\n    tenant2  : %s ", l2nw_key_object2.tenant_name_p );
  printf("\r\n vn name type   : %s ", l2nw_key_object2.vn_name_p );
  printf("\r\n chain set name   : %s ", l2nw_config_object2[0].value.nschainset_object_name_p );
   printf("\r\n admin status2: %d ", l2nw_config_object2[1].value.admin_status_e);


 printf("\r\n      name3    : %s ", l2nw_key_object3.map_name_p);
  printf("\r\n    tenant3  : %s ", l2nw_key_object3.tenant_name_p );
  printf("\r\n vn name type   : %s ", l2nw_key_object3.vn_name_p );
  printf("\r\n chain set name   : %s ",l2nw_config_object3[0].value.nschainset_object_name_p );
   printf("\r\n admin status3 : %d ", l2nw_config_object3[1].value.admin_status_e);

   printf("\r\n   rules name : %s", key_info1.name_p);
   printf("\r\n   rules name : %s", key_info1.tenant_name_p);
   printf("\r\n   rules name : %s", config_info1[9].value.relative_name_p);
   printf("\r\n   rules name : %s", config_info1[8].value.nschain_object_name_p);
   printf("\r\n   check      : %d", config_info1[0].value.src_mac.mac_address_type_e);  
 
   printf("\r\n   rules name : %s", key_info2.name_p);
   printf("\r\n   rules name : %s", key_info2.tenant_name_p);
   printf("\r\n   rules name : %s", config_info2[9].value.relative_name_p);
   printf("\r\n   rules name : %s", config_info2[8].value.nschain_object_name_p);
   
   printf("\r\n   rules name : %s", key_info3.name_p);
   printf("\r\n   rules name : %s", key_info3.tenant_name_p);
   printf("\r\n   rules name : %s", config_info3[9].value.relative_name_p);
   printf("\r\n   rules name : %s", config_info3[8].value.nschain_object_name_p);
   
 
 
}


void nsrm_nwservice_object_notifications_cbk(uint8_t notification_type,
                uint64_t nsrm_nwservice_object_handle,
                union    nsrm_nwservice_object_notification_data  notification_data,
                void     *callback_arg1,
                void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n NSRM Nwservice object Notificatio Call back invoked");

  printf("\r\n  Name  = %s",notification_data.add_del.nwservice_object_name_p);
  printf("\r\n nsrm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n NSRM_NWSERVICE_ALL");
  if(notification_type == 1)
    printf("\r\n NSRM_NWSERVICE_ADDED");
  if(notification_type == 2)
    printf("\r\n NSRM_NWSERVICE_DELETE");
  if(notification_type == 3)
    printf("\r\n NSRM_NWSERVICE_ATTACHED_TO_NSCHAIN");
  if(notification_type == 4)
    printf("\r\n NSRM_NWSERVICE_DETACHED_FROM_NSCHAIN");
  if(notification_type == 5)
    printf("\r\n NSRM_NWSERVICE_LAUNCHED");



  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}

void nsrm_nschain_object_notifications_cbk(uint8_t notification_type,
                uint64_t nsrm_nschain_object_handle,
                union    nsrm_nschain_object_notification_data  notification_data,
                void     *callback_arg1,
                void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n NSRM Nwservice object Notificatio Call back invoked");

  printf("\r\n  Name  = %s",notification_data.add_del.nschain_object_name_p);
  printf("\r\n nsrm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n NSRM_NSCHAIN_ALL");
  if(notification_type == 1)
    printf("\r\n NSRM_NSCHAIN_ADDED");
  if(notification_type == 2)
    printf("\r\n NSRM_NSCHAIN_DELETE");
/*
  if(notification_type == 3)
    printf("\r\n CRM_VM_MODIFIED");
*/
  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}

void nsrm_nschainset_object_notifications_cbk(uint8_t notification_type,
                uint64_t nsrm_nschainset_object_handle,
                union    nsrm_nschainset_object_notification_data  notification_data,
                void     *callback_arg1,
                void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n NSRM Nschainset object Notificatio Call back invoked");

  printf("\r\n  Name  = %s",notification_data.add_del.nschainset_object_name_p);
  printf("\r\n nsrm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n NSRM_NSCHAINSET_ALL");
  if(notification_type == 1)
    printf("\r\n NSRM_NSCHAINSET_ADDED");
  if(notification_type == 2)
    printf("\r\n NSRM_NSCHAINSET_DELETE");
/*
  if(notification_type == 3)
    printf("\r\n CRM_VM_MODIFIED");
*/
  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}



void nsrm_l2nw_service_map_notifications_cbk(uint8_t notification_type,
                uint64_t nsrm_handle,
                union    nsrm_l2nw_service_map_notification_data  notification_data,
                void     *callback_arg1,
                void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n NSRM Nwservice object Notificatio Call back invoked");

  printf("\r\n  Name  = %s",notification_data.add_del.l2nw_service_map_name_p);
  printf("\r\n nsrm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n NSRM_L2NW_ALL");
  if(notification_type == 1)
    printf("\r\n NSRM_L2NW_ADDED");
  if(notification_type == 2)
    printf("\r\n NSRM_L2NW_DELETE");
/*
  if(notification_type == 3)
    printf("\r\n CRM_VM_MODIFIED");
*/
  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}


void nsrm_selection_rule_notifications_cbk(uint8_t notification_type,
                uint64_t nsrm_handle,
                union    nsrm_nschain_selection_rule_notification_data  notification_data,
                void     *callback_arg1,
                void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n NSRM Selection rule Notification Call back invoked");

  printf("\r\n  Name  = %s",notification_data.add_del.nschain_selection_rule_name_p);
  printf("\r\n nsrm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n NSRM_SELECTION_RULE_ALL");
  if(notification_type == 1)
    printf("\r\n NSRM_SELECTION_RULE_ADDED");
  if(notification_type == 2)
    printf("\r\n NSRM_SELECTION_RULE_DELETE");
/*
  if(notification_type == 3)
    printf("\r\n CRM_VM_MODIFIED");
*/
  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}


void nsrm_bypass_rule_notifications_cbk(uint8_t notification_type,
                uint64_t nsrm_handle,
                union    nsrm_nwservice_bypass_rule_notification_data  notification_data,
                void     *callback_arg1,
                void     *callback_arg2)
{
  printf("\r\n **************************************************************************");
  printf("\r\n NSRM bypass rule Notification Call back invoked");

  printf("\r\n  Name  = %s",notification_data.add_del.nwservice_bypass_rule_name_p);
  printf("\r\n nsrm notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n NSRM_BYPASSRULE_ALL");
  if(notification_type == 1)
    printf("\r\n NSRM_BYPASS_RULE_ADDED");
  if(notification_type == 2)
    printf("\r\n NSRM_BYPASS_RULE_DELETE");
/*
  if(notification_type == 3)
    printf("\r\n CRM_VM_MODIFIED");
*/
  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n callback arg2 = %d",*(uint32_t *)callback_arg2);
  printf("\r\n ******************************************************************************\r\n");
}


void printnsrmhandle(uint64_t handle)
{
  int ii;
  uchar8_t *nsrmhandle;

  printf(" handle = ");
  nsrmhandle =(uchar8_t *)(&handle);
  for(ii=0;ii<8;ii++)
  printf(" %x",nsrmhandle[ii]);
}
void nsrm_test()
{
  nsrm_appl_test();
  nsrm_nschain_test();
  nsrm_nschainset_test();  
  nsrm_l2nw_test();
  nsrm_selection_rule_test();
  nsrm_bypass_rule_test();
}


int32_t nsrm_appl_test()
{
   int32_t ret_val;
   uint32_t  callback_arg1 = 2;
   uint32_t  callback_arg2 = 3;
   uint32_t*  number;
   number =  malloc(4);
  
   ret_val =  nsrm_register_nwservice_object_notifications(NSRM_NWSERVICE_OBJECT_ALL,
                                                           nsrm_nwservice_object_notifications_cbk,
                                                           (void *)&callback_arg1,
                                                           (void *)&callback_arg2);
 
   printf("\r\n*********************************************************************\n");
   printf("\r\n NSRM Appliance category  test begins\n");
   printf("\r\n*********************************************************************\n");
  

   printf("\r\n Adding applcategory1.....\n");

   nwservice_key_object1.name_p = malloc(10);
   nwservice_key_object1.tenant_name_p = malloc(10); 
   strcpy(nwservice_key_object1.name_p , "nws1");
   strcpy(nwservice_key_object1.tenant_name_p , "t1");
   nwservice_config_object1[0].field_id = 1;
   nwservice_config_object1[0].value.nwservice_object_form_factor_type_e = 2;
   nwservice_config_object1[1].field_id = 2;
   nwservice_config_object1[1].value.admin_status_e = 3;
   
   
 
   
   printf("before add in test");
  
   printvalues_appl();
  
   
   ret_val = nsrm_add_nwservice_object(2,&nwservice_key_object1,&nwservice_config_object1);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n  added successfully!.\n");
   }
   else
     printf("\r\n failed to add !.\n");
    printvalues_appl();
  
printf("*********************************************************");
   nwservice_key_object2.name_p = malloc(10);
   nwservice_key_object2.tenant_name_p = malloc(10); 
   strcpy(nwservice_key_object2.name_p , "nws2");
   strcpy(nwservice_key_object2.tenant_name_p , "t1");
   nwservice_config_object2[0].value.nwservice_object_form_factor_type_e = 1;
   nwservice_config_object2[1].value.admin_status_e = 1;
   
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nwservice_object(2,&nwservice_key_object2,&nwservice_config_object2);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");



   printvalues_appl();
   printf("*********************************************************");
  
   nwservice_key_object3.name_p = malloc(10);
   nwservice_key_object3.tenant_name_p = malloc(10); 
   strcpy(nwservice_key_object3.name_p , "nws3");
   strcpy(nwservice_key_object3.tenant_name_p , "t3");
   nwservice_config_object3[0].value.nwservice_object_form_factor_type_e = 2;
   nwservice_config_object3[1].value.admin_status_e = 1;
   
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nwservice_object(2,&nwservice_key_object3,&nwservice_config_object3);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl(); 


//  ret_val = nsrm_del_nwservice_object(&nwservice_key_object2);

  if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n del successfully!.\n");
   }
   else
     printf("\r\n failed to delete!.\n");

   nwservice_key_object5.name_p = malloc(10);
   nwservice_key_object5.tenant_name_p = malloc(10); 
   strcpy(nwservice_key_object5.name_p , "different");
   strcpy(nwservice_key_object5.tenant_name_p , "t3");
   nwservice_config_object5[0].value.nwservice_object_form_factor_type_e = 2;
   nwservice_config_object5[1].value.admin_status_e = 3;
   
   
   printf("before adding nsw4");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nwservice_object(2,&nwservice_key_object5,&nwservice_config_object5);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n nws4 added successfully!.\n");
   }
   else
     printf("\r\n failed to add nws4!.\n");

   printvalues_appl();

   nwservice_key_object6.name_p = malloc(10);
   nwservice_key_object6.tenant_name_p = malloc(10); 
   strcpy(nwservice_key_object6.name_p , "nws6");
   strcpy(nwservice_key_object6.tenant_name_p , "t3");
   nwservice_config_object6[0].value.nwservice_object_form_factor_type_e = 3;
   nwservice_config_object6[1].value.admin_status_e = 0;
   
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nwservice_object(2,&nwservice_key_object6,&nwservice_config_object6);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl(); 

  printf("/n **********ADD FINSHED *******************");


  struct nsrm_nwservice_object_record *recs = (struct nsrm_nwservice_object_record*)calloc(2,sizeof(struct nsrm_nwservice_object_record));
  int32_t   request = 2;
  ret_val = nsrm_get_first_nwservice_objects(request,&number,&recs[2]);

  ret_val = nsrm_get_next_nwservice_object(&nwservice_key_object1,request,&number,&recs[2]);

  ret_val = nsrm_get_exact_nwservice_object(&nwservice_key_object5,&recs);
  

 }

int32_t nsrm_nschain_test()
{
   int32_t ret_val;
   uint32_t  callback_arg1 = 2;
   uint32_t  callback_arg2 = 3;
   uint32_t*  number1;
   number1 =  malloc(4);
struct nsrm_nwservice_object_record *recs = (struct nsrm_nwservice_object_record*)calloc(2,sizeof(struct nsrm_nwservice_object_record));
 
   ret_val =  nsrm_register_nschain_object_notifications(NSRM_NSCHAIN_OBJECT_ALL,
                                                           nsrm_nschain_object_notifications_cbk,
                                                           (void *)&callback_arg1,
                                                           (void *)&callback_arg2);
 
   printf("\r\n*********************************************************************\n");
   printf("\r\n NSRM Appliance category  test begins\n");
   printf("\r\n*********************************************************************\n");
  

   printf("\r\n Adding applcategory1.....\n");

   nschain_key_object1.name_p = malloc(10);
   nschain_key_object1.tenant_name_p = malloc(10); 
   strcpy(nschain_key_object1.name_p , "chain1");
   strcpy(nschain_key_object1.tenant_name_p , "t1");
   nschain_config_object1[0].value.nschain_type = 2;
   nschain_config_object1[1].value.load_sharing_algorithm_e = 2;
   nschain_config_object1[2].value.load_indication_type_e = 2;
   nschain_config_object1[3].value.high_threshold = 2;
   nschain_config_object1[4].value.low_threshold = 2;
   nschain_config_object1[5].value.admin_status_e = 3;
   
   
   printf("before add in test");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nschain_object(2,&nschain_key_object1,&nschain_config_object1);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n  added successfully!.\n");
   }
   else
     printf("\r\n failed to add !.\n");
     printvalues_appl();
printf("*********************************************************");
   nschain_key_object2.name_p = malloc(10);
   nschain_key_object2.tenant_name_p = malloc(10); 
   strcpy(nschain_key_object2.name_p , "chain2");
   strcpy(nschain_key_object2.tenant_name_p , "t1");
   nschain_config_object2[0].value.nschain_type = 3;
   nschain_config_object2[1].value.load_sharing_algorithm_e = 0;
   nschain_config_object2[2].value.load_indication_type_e = 1;
   nschain_config_object2[3].value.high_threshold = 2;
   nschain_config_object2[4].value.low_threshold = 1;
   nschain_config_object2[5].value.admin_status_e = 1;
    
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nschain_object(2,&nschain_key_object2,&nschain_config_object2);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");



   printvalues_appl();
   printf("*********************************************************");
  
   nschain_key_object3.name_p = malloc(10);
   nschain_key_object3.tenant_name_p = malloc(10); 
   strcpy(nschain_key_object3.name_p , "chain3");
   strcpy(nschain_key_object3.tenant_name_p , "t3");
   nschain_config_object3[0].value.nschain_type = 3;
   nschain_config_object3[1].value.load_sharing_algorithm_e = 3;
   nschain_config_object3[2].value.load_indication_type_e = 3;
   nschain_config_object3[3].value.high_threshold = 3;
   nschain_config_object3[4].value.low_threshold = 3;
   nschain_config_object3[5].value.admin_status_e = 3;
      
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nschain_object(2,&nschain_key_object3,&nschain_config_object3);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl();

    
  ret_val = nsrm_del_nschain_object(&nschain_key_object2);

   nschain_key_object4.name_p = malloc(10);
   nschain_key_object4.tenant_name_p = malloc(10); 
   strcpy(nschain_key_object4.name_p , "chhaaa4");
   strcpy(nschain_key_object4.tenant_name_p , "t3");
   nschain_config_object4[0].value.nschain_type = 4;
   nschain_config_object4[1].value.load_sharing_algorithm_e = 2;
   nschain_config_object4[2].value.load_indication_type_e = 1;
   nschain_config_object4[3].value.high_threshold = 0;
   nschain_config_object4[4].value.low_threshold = 3;
   nschain_config_object4[5].value.admin_status_e = 3;
      
   
   printf("before add");
  
   printvalues_appl();

   struct nsrm_nschain_object_record *recs_p = (struct nsrm_nschain_object_record*)calloc(2,sizeof(struct nsrm_nschain_object_record));
    int32_t request = 2;
   
 
   ret_val = nsrm_add_nschain_object(2,&nschain_key_object4,&nschain_config_object4);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl();
 
   nschain_key_object5.name_p = malloc(10);
   nschain_key_object5.tenant_name_p = malloc(10);    
   strcpy(nschain_key_object5.name_p , "chain4");
   strcpy(nschain_key_object5.tenant_name_p , "t3");
   nschain_config_object5[0].value.nschain_type = 1;
   nschain_config_object5[1].value.load_sharing_algorithm_e = 4;
   nschain_config_object5[2].value.load_indication_type_e = 3;
   nschain_config_object5[3].value.high_threshold = 7;
   nschain_config_object5[4].value.low_threshold = 7;
   nschain_config_object5[5].value.admin_status_e = 7;
    
   ret_val = nsrm_add_nschain_object(2,&nschain_key_object5,&nschain_config_object5);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl();
  int32_t request1 = 2; 
    ret_val = nsrm_get_first_nschain_objects(request,&number1,&recs_p[2]);
    ret_val = nsrm_attach_nwservice_object_to_nschain_object(&nwservice_key_object5,"chain1",2);
    ret_val = nsrm_attach_nwservice_object_to_nschain_object(&nwservice_key_object3,"chain1",7);
    ret_val = nsrm_attach_nwservice_object_to_nschain_object(&nwservice_key_object6,"chain1",1);
    ret_val = nsrm_attach_nwservice_object_to_nschain_object(&nwservice_key_object1,"chain1",4);
    ret_val = nsrm_attach_nwservice_object_to_nschain_object(&nwservice_key_object2,"chain1",3);
   // ret_val = nsrm_detach_nwservice_object_from_nschain_object(&nwservice_key_object3,"chain4");
    ret_val = nsrm_get_first_nwservice_objects_in_nschain_object(&nschain_key_object1,request1,&number1,&recs[1]);
    ret_val = nsrm_get_next_nwservice_object_in_nschain_object(&nschain_key_object1,&nwservice_key_object5,1,&number1,&recs[1]);
    ret_val = nsrm_get_exact_nwservice_object_in_nschain_object(&nschain_key_object1,&nwservice_key_object3, &recs_p);
   //ret_val = nsrm_get_next_nschain_object(&nschain_key_object1,request,&number,&recs);

 //  ret_val = nsrm_get_exact_nschain_object(&nschain_key_object1,&recs);

}

int32_t nsrm_nschainset_test()
{
   int32_t ret_val;
   uint32_t  callback_arg1 = 2;
   uint32_t  callback_arg2 = 3;
   uint32_t*  number1;
   number1 =  malloc(4);
 
  
   ret_val =  nsrm_register_nschainset_object_notifications(NSRM_NSCHAIN_SET_OBJECT_ALL,
                                                           nsrm_nschainset_object_notifications_cbk,
                                                           (void *)&callback_arg1,
                                                           (void *)&callback_arg2);
 
   printf("\r\n*********************************************************************\n");
   printf("\r\n NSRM Appliance category  test begins\n");
   printf("\r\n*********************************************************************\n");
  

   printf("\r\n Adding applcategory1.....\n");

   nschainset_key_object1.name_p = malloc(10);
   nschainset_key_object1.tenant_name_p = malloc(10); 
   strcpy(nschainset_key_object1.name_p , "chset1");
   strcpy(nschainset_key_object1.tenant_name_p , "t1");
   nschainset_config_object1[0].value.nschainset_type = 2;
   nschainset_config_object1[1].value.admin_status_e = 3;
   
   
   printf("before add in test");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nschainset_object(2,&nschainset_key_object1,&nschainset_config_object1);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n  added successfully!.\n");
   }
   else
     printf("\r\n failed to add !.\n");
     printvalues_appl();
printf("*********************************************************");
   nschainset_key_object2.name_p = malloc(10);
   nschainset_key_object2.tenant_name_p = malloc(10); 
   strcpy(nschainset_key_object2.name_p , "chain2");
   strcpy(nschainset_key_object2.tenant_name_p , "t1");
   nschainset_config_object2[0].value.nschainset_type = 3;
   nschainset_config_object2[1].value.admin_status_e = 1;
    
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nschainset_object(2,&nschainset_key_object2,&nschainset_config_object2);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");



   printvalues_appl();
   printf("*********************************************************");
  
   nschainset_key_object3.name_p = malloc(10);
   nschainset_key_object3.tenant_name_p = malloc(10); 
   strcpy(nschainset_key_object3.name_p , "chain3");
   strcpy(nschainset_key_object3.tenant_name_p , "t3");
   nschainset_config_object3[0].value.nschainset_type = 3;
   nschainset_config_object3[1].value.admin_status_e = 3;
      
   
   printf("before add");
  
   printvalues_appl();

   
   ret_val = nsrm_add_nschainset_object(2,&nschainset_key_object3,&nschainset_config_object3);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl();

    
  ret_val = nsrm_del_nschainset_object(&nschainset_key_object2);

   nschainset_key_object4.name_p = malloc(10);
   nschainset_key_object4.tenant_name_p = malloc(10); 
   strcpy(nschainset_key_object4.name_p , "chhaaa4");
   strcpy(nschainset_key_object4.tenant_name_p , "t3");
   nschainset_config_object4[0].value.nschainset_type = 4;
   nschainset_config_object4[1].value.admin_status_e = 3;
      
   
   printf("before add");
  
   printvalues_appl();

   struct nsrm_nschainset_object_record *recs_p = (struct nsrm_nschainset_object_record*)calloc(2,sizeof(struct nsrm_nschainset_object_record));
    int32_t request = 2;
   
 
   ret_val = nsrm_add_nschainset_object(2,&nschainset_key_object4,&nschainset_config_object4);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl();
 
   nschainset_key_object5.name_p = malloc(10);
   nschainset_key_object5.tenant_name_p = malloc(10);    
   strcpy(nschainset_key_object5.name_p , "chain4");
   strcpy(nschainset_key_object5.tenant_name_p , "t3");
   nschainset_config_object5[0].value.nschainset_type = 1;
   nschainset_config_object5[1].value.admin_status_e = 7;
    
   ret_val = nsrm_add_nschainset_object(2,&nschainset_key_object5,&nschainset_config_object5);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n appl1 added successfully!.\n");
   }
   else
     printf("\r\n failed to add appl1!.\n");

   printvalues_appl();
  int32_t request1 = 2; 
    ret_val = nsrm_get_first_nschainset_objects(request1,&number1,&recs_p[2]);
    ret_val = nsrm_get_next_nschainset_object(&nschainset_key_object1,request1,&number1,&recs_p);

 //  ret_val = nsrm_get_exact_nschainset_object(&nschainset_key_object1,&recs);

}

int32_t nsrm_l2nw_test()
{
   int32_t ret_val;
   uint32_t  callback_arg1 = 2;
   uint32_t  callback_arg2 = 3;
   uint32_t*  number1;
   number1 =  malloc(4);
   ret_val =  nsrm_register_l2nw_service_map_notifications(NSRM_L2NW_SERVICE_MAP_ALL,
                                                           nsrm_l2nw_service_map_notifications_cbk,
                                                           (void *)&callback_arg1,
                                                           (void *)&callback_arg2);
   printf("\r\n*********************************************************************\n");
   printf("\r\n NSRM Appliance category  test begins\n");
   printf("\r\n*********************************************************************\n");
  

   printf("\r\n Adding applcategory1.....\n");

   l2nw_key_object1.map_name_p = (char*)malloc(10);
   l2nw_key_object1.tenant_name_p = (char*)malloc(10); 
   l2nw_config_object1[0].value.nschainset_object_name_p = (char*)malloc(10); 
   l2nw_key_object1.vn_name_p = (char*)malloc(10); 
   strcpy(l2nw_key_object1.map_name_p , "map1");
   strcpy(l2nw_key_object1.tenant_name_p , "t1");
   printf("\nafter tenant copy\n\n");
   strcpy(l2nw_key_object1.vn_name_p , "VN1");
   strcpy(l2nw_config_object1[0].value.nschainset_object_name_p , "chain4");
   l2nw_config_object1[1].value.admin_status_e = 2;
   printf("\nafter vnname copy\n\n");
   printf("\nafter chainset copy\n\n");
   
   
   printf("before add in test");
  
   printvalues_appl();

   
   ret_val = nsrm_add_l2nw_service_map_record(2,&l2nw_key_object1,&l2nw_config_object1);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n  added successfully!.\n");
   }
   else
     printf("\r\n failed to add !.\n");
     printvalues_appl();



   l2nw_key_object2.map_name_p = (char*)malloc(10);
   l2nw_key_object2.tenant_name_p = (char*)malloc(10); 
   l2nw_config_object2[0].value.nschainset_object_name_p = (char*)malloc(10); 
   l2nw_key_object2.vn_name_p = (char*)malloc(10); 
   strcpy(l2nw_key_object2.map_name_p , "map2");
   strcpy(l2nw_key_object2.tenant_name_p , "t1");
   printf("\nafter tenant copy\n\n");
   strcpy(l2nw_key_object2.vn_name_p , "VN1");
   strcpy(l2nw_config_object2[0].value.nschainset_object_name_p , "chain4");
   l2nw_config_object2[1].value.admin_status_e = 3;
   printf("\nafter vnname copy\n\n");
   printf("\nafter chainset copy\n\n");
   
   
   printf("before add in test");
  
   printvalues_appl();

   
   ret_val = nsrm_add_l2nw_service_map_record(2,&l2nw_key_object2,&l2nw_config_object2);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n  added successfully!.\n");
   }
   else
     printf("\r\n failed to add !.\n");
     printvalues_appl();



   l2nw_key_object3.map_name_p = (char*)malloc(10);
   l2nw_key_object3.tenant_name_p = (char*)malloc(10); 
   l2nw_config_object3[0].value.nschainset_object_name_p = (char*)malloc(10); 
   l2nw_key_object3.vn_name_p = (char*)malloc(10); 
   strcpy(l2nw_key_object3.map_name_p , "map3");
   strcpy(l2nw_key_object3.tenant_name_p , "t3");
   printf("\nafter tenant copy\n\n");
   strcpy(l2nw_key_object3.vn_name_p , "VN1");
   strcpy(l2nw_config_object3[0].value.nschainset_object_name_p ,"chain3");
   l2nw_config_object3[1].value.admin_status_e = 1;
   printf("\nafter vnname copy\n\n");
   printf("\nafter chainset copy\n\n");
   
   
   printf("before add in test");
  
   printvalues_appl();

   
   ret_val = nsrm_add_l2nw_service_map_record(2,&l2nw_key_object3,&l2nw_config_object3);
   printf("after addition");

   if(ret_val==NSRM_SUCCESS)
   {
     printf("\r\n  added successfully!.\n");
   }
   else
     printf("\r\n failed to add !.\n");
     printvalues_appl();

   //ret_val = nsrm_del_l2nw_service_map_record(&l2nw_key_object2);
  
   strcpy(l2nw_key_object3.map_name_p , "map3");
   strcpy(l2nw_key_object3.tenant_name_p , "t3");
   printf("\nafter tenant copy\n\n");
   strcpy(l2nw_key_object3.vn_name_p , "VN1");
   strcpy(l2nw_config_object3[0].value.nschainset_object_name_p ,"chain4");
   l2nw_config_object3[1].value.admin_status_e = 0;
   printf("\nafter vnname copy\n\n");
   printf("\nafter chainset copy\n\n");
   
struct nsrm_l2nw_service_map_record *recs_p = (struct nsrm_l2nw_service_map_record*)calloc(2,sizeof(struct nsrm_l2nw_service_map_record));
    int32_t request = 2;
    int32_t* number2;
    number2 = malloc(4);
   


   ret_val = nsrm_modify_l2nw_service_map_record(&l2nw_key_object3,2,&l2nw_config_object3);
   ret_val = nsrm_get_first_l2nw_service_maps(request,&number2,&recs_p[2]);
   ret_val = nsrm_get_next_l2nw_service_maps(&l2nw_key_object1,request,&number2,&recs_p[2]);
   ret_val = nsrm_get_exact_l2nw_service_map(&l2nw_key_object2,&recs_p);
   
   ret_val = nsrm_register_launched_nwservice_object_instance("inst1","t3","chain4","nws1","VM1",12,13);
   ret_val = nsrm_register_launched_nwservice_object_instance("inst2","t1","chain1","nws1","VM1",13,14);
   ret_val = nsrm_register_launched_nwservice_object_instance("inst3","t1","chain1","nws1","VM1",13,14);
   ret_val = nsrm_register_delaunch_nwswervice_object_instance("inst1","t3","chain4","nws1");
}


void nsrm_selection_rule_test()
{
   int32_t ret_val;
 
   uint32_t  callback_arg1 = 2;
   uint32_t  callback_arg2 = 3;
 
    ret_val =  nsrm_register_nschain_selection_rule_notifications(NSRM_NSCHAIN_SELECTION_RULE_ALL,
                                                           nsrm_selection_rule_notifications_cbk,
                                                           (void *)&callback_arg1,
                                                           (void *)&callback_arg2);
 


   key_info1.name_p = (char*)malloc(10);
   key_info1.tenant_name_p = (char*)malloc(10);
   key_info1.nschainset_object_name_p = (char*)malloc(10);
   config_info1[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info1[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info1.name_p,"rule1");
   strcpy(key_info1.tenant_name_p ,"t1");
   strcpy(key_info1.nschainset_object_name_p ,"chain4");
   strcpy(config_info1[8].value.nschain_object_name_p,"chain1");
   strcpy(config_info1[9].value.relative_name_p,"relname");
   config_info1[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_info1[0].value.src_mac.mac_address_type_e); 
   nsrm_add_nschain_selection_rule_to_nschainset(&key_info1,2,&config_info1);
   printvalues_appl();
 

   key_info2.name_p = (char*)malloc(10);
   key_info2.tenant_name_p = (char*)malloc(10);
   key_info2.nschainset_object_name_p = (char*)malloc(10);
   config_info2[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info2[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info2.name_p,"rule1");
   strcpy(key_info2.tenant_name_p ,"t1");
   strcpy(key_info2.nschainset_object_name_p ,"chain4");
   strcpy(config_info2[8].value.nschain_object_name_p,"chain1");
   strcpy(config_info2[9].value.relative_name_p,"rule1");
   key_info2.location_e = 0;
  
   nsrm_add_nschain_selection_rule_to_nschainset(&key_info2,2,&config_info2);
   printvalues_appl();



   key_info3.name_p = (char*)malloc(10);
   key_info3.tenant_name_p = (char*)malloc(10);
   key_info3.nschainset_object_name_p = (char*)malloc(10);
   config_info3[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info3[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info3.name_p,"rule3");
   strcpy(key_info3.tenant_name_p ,"t1");
   strcpy(key_info3.nschainset_object_name_p ,"chain4");
   strcpy(config_info3[8].value.nschain_object_name_p,"chain1");
   strcpy(config_info3[9].value.relative_name_p,"rule1");
   key_info3.location_e = 2;
  
   nsrm_add_nschain_selection_rule_to_nschainset(&key_info3,2,&config_info3);
   printvalues_appl();

  
   key_info4.name_p = (char*)malloc(10);
   key_info4.tenant_name_p = (char*)malloc(10);
   key_info4.nschainset_object_name_p = (char*)malloc(10);
   config_info4[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info4[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info4.name_p,"rule4");
   strcpy(key_info4.tenant_name_p ,"t1");
   strcpy(key_info4.nschainset_object_name_p ,"chain4");
   strcpy(config_info4[8].value.nschain_object_name_p,"chain1");
   strcpy(config_info4[9].value.relative_name_p,"rule3");
   key_info4.location_e = 3;
  
   nsrm_add_nschain_selection_rule_to_nschainset(&key_info4,2,&config_info4);
   printvalues_appl();

   key_info5.name_p = (char*)malloc(10);
   key_info5.tenant_name_p = (char*)malloc(10);
   key_info5.nschainset_object_name_p = (char*)malloc(10);
   config_info5[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info5[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info5.name_p,"rule5");
   strcpy(key_info5.tenant_name_p ,"t1");
   strcpy(key_info5.nschainset_object_name_p ,"chain4");
   strcpy(config_info5[8].value.nschain_object_name_p,"chain1");
   strcpy(config_info5[9].value.relative_name_p,"relname");
   config_info5[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_info5[0].value.src_mac.mac_address_type_e); 
   nsrm_add_nschain_selection_rule_to_nschainset(&key_info5,2,&config_info5);
   printvalues_appl();
 
   key_info6.name_p = (char*)malloc(10);
   key_info6.tenant_name_p = (char*)malloc(10);
   key_info6.nschainset_object_name_p = (char*)malloc(10);
   config_info6[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info6[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info6.name_p,"rule1");
   strcpy(key_info6.tenant_name_p ,"t1");
   strcpy(key_info6.nschainset_object_name_p ,"chain4");
   strcpy(config_info6[8].value.nschain_object_name_p,"chain1");
   strcpy(config_info6[9].value.relative_name_p,"relname");
   config_info6[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_info6[0].value.src_mac.mac_address_type_e); 
   nsrm_add_nschain_selection_rule_to_nschainset(&key_info6,2,&config_info6);
   printvalues_appl();


    key_info5.name_p = (char*)malloc(10);
   key_info5.tenant_name_p = (char*)malloc(10);
   key_info5.nschainset_object_name_p = (char*)malloc(10);
   config_info5[8].value.nschain_object_name_p = (char*)malloc(10);
   config_info5[9].value.relative_name_p = (char*)malloc(10);

   strcpy(key_info5.name_p,"rule1");
   strcpy(key_info5.tenant_name_p ,"t1");
   strcpy(key_info5.nschainset_object_name_p ,"chain4");
   strcpy(config_info5[8].value.nschain_object_name_p,"chain3");
   strcpy(config_info5[9].value.relative_name_p,"rule1");
   config_info5[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_info5[0].value.src_mac.mac_address_type_e); 
   nsrm_modify_nschain_selection_rule(&key_info5,2,&config_info5);
   printvalues_appl();
 
 

   nsrm_del_nschain_selection_rule_from_nschainset(&key_info2);
 
   int32_t *number;
   number=calloc(1,4);
   struct nsrm_nschain_selection_rule_record *recs_p = (struct nsrm_nschain_selection_rule_record*)calloc(11,sizeof(struct nsrm_nschain_selection_rule_record));
 
   nsrm_get_first_selection_rules_from_nschainset_object(&nschainset_key_object5,1,&number,&recs_p[11]);
   nsrm_get_next_selection_rules_from_nschainset_object(&nschainset_key_object5,&key_info1,2,&number,&recs_p[11]);
   nsrm_get_exact_selection_rule_from_nschainset_object(&nschainset_key_object5,&key_info1,&recs_p);
}


int32_t nsrm_bypass_rule_test()
{
  
  int32_t ret_val;

   uint32_t  callback_arg1 = 2;
   uint32_t  callback_arg2 = 3;
  
    ret_val =  nsrm_register_nwservice_bypass_rule_notifications(NSRM_NWSERVICE_BYPASS_RULE_ALL,
                                                           nsrm_bypass_rule_notifications_cbk,
                                                           (void *)&callback_arg1,
                                                           (void *)&callback_arg2);


   key_infob1.name_p = (char*)malloc(10);
   key_infob1.tenant_name_p = (char*)malloc(10);
   key_infob1.nschain_object_name_p = (char*)malloc(10);
   config_infob1[9].value.nwservice_object_names_p_p = (char**)calloc(1,100);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test1");
   config_infob1[10].value.relative_name_p = (char*)malloc(10);

   strcpy(key_infob1.name_p,"ruleb1");
   strcpy(key_infob1.tenant_name_p ,"t1");
   strcpy(key_infob1.nschain_object_name_p ,"chain1");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test2");
   config_infob1[8].value.no_of_nwservice_objects = 3;
   config_infob1[9].value.nwservice_object_names_p_p[0] = (char*)calloc(1,10);
   strcpy(config_infob1[9].value.nwservice_object_names_p_p[0],"nws1");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test3");
   config_infob1[9].value.nwservice_object_names_p_p[1] = (char*)calloc(1,10);
   strcpy(config_infob1[9].value.nwservice_object_names_p_p[1],"nws3");
   config_infob1[9].value.nwservice_object_names_p_p[2] = (char*)calloc(1,10);
   strcpy(config_infob1[10].value.relative_name_p,"relname");
   strcpy(config_infob1[9].value.nwservice_object_names_p_p[2],"nws1");
   config_infob1[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_infob1[0].value.src_mac.mac_address_type_e);
   key_infob1.location_e = 0;
   nsrm_add_nwservice_bypass_rule_to_nschain_object(&key_infob1,2,&config_infob1);
   printvalues_appl();



   key_infob2.name_p = (char*)malloc(10);
   key_infob2.tenant_name_p = (char*)malloc(10);
   key_infob2.nschain_object_name_p = (char*)malloc(10);
   config_infob2[9].value.nwservice_object_names_p_p = (char**)calloc(1,100);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test1");
   config_infob2[10].value.relative_name_p = (char*)malloc(10);

   strcpy(key_infob2.name_p,"ruleb2");
   strcpy(key_infob2.tenant_name_p ,"t1");
   strcpy(key_infob2.nschain_object_name_p ,"chain1");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test2");
   config_infob2[8].value.no_of_nwservice_objects = 3;
   config_infob2[9].value.nwservice_object_names_p_p[0] = (char*)calloc(1,10);
   strcpy(config_infob2[9].value.nwservice_object_names_p_p[0],"nws3");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test3");
   config_infob2[9].value.nwservice_object_names_p_p[1] = (char*)calloc(1,10);
   strcpy(config_infob2[9].value.nwservice_object_names_p_p[1],"nws1");
   config_infob2[9].value.nwservice_object_names_p_p[2] = (char*)calloc(1,10);
   strcpy(config_infob2[10].value.relative_name_p,"ruleb1");
   strcpy(config_infob2[9].value.nwservice_object_names_p_p[2],"nws4");
   config_infob2[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_infob2[0].value.src_mac.mac_address_type_e);
   key_infob1.location_e = 1;
   nsrm_add_nwservice_bypass_rule_to_nschain_object(&key_infob2,2,&config_infob2);
   printvalues_appl();


   key_infob3.name_p = (char*)malloc(10);
   key_infob3.tenant_name_p = (char*)malloc(10);
   key_infob3.nschain_object_name_p = (char*)malloc(10);
   config_infob3[9].value.nwservice_object_names_p_p = (char**)calloc(1,100);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test1");
   config_infob3[10].value.relative_name_p = (char*)malloc(10);

   strcpy(key_infob3.name_p,"ruleb3");
   strcpy(key_infob3.tenant_name_p ,"t1");
   strcpy(key_infob3.nschain_object_name_p ,"chain1");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test2");
   config_infob3[8].value.no_of_nwservice_objects = 3;
   config_infob3[9].value.nwservice_object_names_p_p[0] = (char*)calloc(1,10);
   strcpy(config_infob3[9].value.nwservice_object_names_p_p[0],"nws3");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test3");
   config_infob3[9].value.nwservice_object_names_p_p[1] = (char*)calloc(1,10);
   strcpy(config_infob3[9].value.nwservice_object_names_p_p[1],"nws1");
   config_infob3[9].value.nwservice_object_names_p_p[2] = (char*)calloc(1,10);
   strcpy(config_infob3[10].value.relative_name_p,"relname");
   strcpy(config_infob3[9].value.nwservice_object_names_p_p[2],"nws4");
   config_infob3[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_infob3[0].value.src_mac.mac_address_type_e);
   nsrm_add_nwservice_bypass_rule_to_nschain_object(&key_infob3,2,&config_infob3);
   printvalues_appl();

   key_infob4.name_p = (char*)malloc(10);
   key_infob4.tenant_name_p = (char*)malloc(10);
   key_infob4.nschain_object_name_p = (char*)malloc(10);
   config_infob4[9].value.nwservice_object_names_p_p = (char**)calloc(1,100);
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test1");
   config_infob4[10].value.relative_name_p = (char*)malloc(10);

   strcpy(key_infob4.name_p,"ruleb4");
   strcpy(key_infob4.tenant_name_p ,"t1");
   strcpy(key_infob4.nschain_object_name_p ,"chain1");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test2");
   config_infob4[8].value.no_of_nwservice_objects = 3;
   config_infob4[9].value.nwservice_object_names_p_p[0] = (char*)calloc(1,10);
   strcpy(config_infob4[9].value.nwservice_object_names_p_p[0],"nws3");
   OF_LOG_MSG(OF_LOG_NSRM, OF_LOG_DEBUG,"debug in test3");
   config_infob4[9].value.nwservice_object_names_p_p[1] = (char*)calloc(1,10);
   strcpy(config_infob4[9].value.nwservice_object_names_p_p[1],"nws1");
   config_infob4[9].value.nwservice_object_names_p_p[2] = (char*)calloc(1,10);
   strcpy(config_infob4[10].value.relative_name_p,"rule1");
   strcpy(config_infob4[9].value.nwservice_object_names_p_p[2],"nws4");
   config_infob4[0].value.src_mac.mac_address_type_e = 8;
   printf("test :%d",config_infob4[0].value.src_mac.mac_address_type_e);
   nsrm_add_nwservice_bypass_rule_to_nschain_object(&key_infob4,2,&config_infob4);
   printvalues_appl();

   int32_t *number;
   number=calloc(1,4);
   struct nsrm_nwservice_bypass_rule_record *recs_p = (struct nsrm_nwservice_bypass_rule_record*)calloc(11,sizeof(struct nsrm_nwservice_bypass_rule_record));
 


   nsrm_get_first_bypass_rules_from_nschain_object(&nschain_key_object1 , 1 ,&number ,&recs_p[3]);

   nsrm_del_nwservice_bypass_rule_from_nschain_object(&key_infob1);

} 
