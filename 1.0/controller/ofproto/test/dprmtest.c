#include "controller.h"
#include "dprm.h"

#define dprm_debug printf

void get_switch(void);
void get_domain(void);
void get_table(uint64_t domain_handle );
void get_datapath(void);
void get_dp_port(uint64_t dp_handle);

void  printvalues(void);
void  printvalues_domain(void);
void  printvalues_datapath(void);
void  printvalues_oftable(void);
void  printvalues_port(void); 

void  printhandle(uint64_t handle);
void  printretvalue(int32_t retval);

void  getall_switches(void);
void  getall_domains(void);
void  getall_datapaths(void);
//void  getall_tables(uint64_t dm_handle);
void  getall_ports(uint64_t dp_handle);

extern int32_t perform_tests_on_idbased_table();

void      test_mempool_module(void);
void      switch_test(void);
int32_t      domain_test(void);
int32_t       datapath_test(void);

uint64_t  switch_handle,domain_handle,datapath_handle,port_handle;

uint64_t  sw_handle1,sw_handle2,sw_handle3,sw_handle4,sw_handle5;
uint64_t  dm_handle1,dm_handle2,dm_handle3,dm_handle4,dm_handle5;
uint64_t  dp_handle1,dp_handle2,dp_handle3,dp_handle4,dp_handle5;
uint64_t  oftable_handle,oftable_handle1,oftable_handle2;
uint64_t  port_handle1,port_handle2;

struct    dprm_switch_info switch_info1,switch_info2,switch_info3,switch_info4,switch_info5;
struct    dprm_distributed_forwarding_domain_info domain_info1,domain_info2,domain_info3,domain_info4,domain_info5;
struct    dprm_datapath_general_info datapath_info1,datapath_info2,datapath_info3,datapath_info4,datapath_info5;
struct    dprm_table_info *dprm_table_p,*dprm_table1_p,*dprm_table2_p;
struct    dprm_port_info port_info1,port_info2;

struct    dprm_switch_info config_info;
struct    dprm_switch_runtime_info runtime_info;
struct    dprm_distributed_forwarding_domain_info* config_info_domain_p;
struct    dprm_distributed_forwarding_domain_runtime_info config_runtime_info_domain;
struct    dprm_datapath_general_info config_info_datapath;
struct    dprm_datapath_runtime_info runtime_info_datapath;
struct    dprm_port_info config_info_port;
struct    dprm_port_runtime_info config_runtime_info_port;

struct    dprm_distributed_forwarding_domain_info* domain_info1_p;

struct    dprm_datapath_entry* datapath_info_p;

int32_t   lRet_value0,lRet_value1,lRet_value2,lRet_value3,lRet_value4,lRet_value5;
int32_t   ii;

/* mempool test */
void *dprm_test_mempool_g = NULL;
struct mempool_stats stats;

void print_mempool_stats(void);
struct  test_entry{
  int32_t  val1;
  int32_t  val2;
  int32_t  val3;
  int32_t  val4;
  int32_t  val5;
  uint8_t  heap_b;
};

struct  test_entry* test_mem_block[40];
uint8_t heap_b;

void sw_notifications_cbk(uint32_t notification_type,
                          uint64_t switch_handle,
                          struct   dprm_switch_notification_data notification_data,
                          void     *callback_arg1,
                          void     *callback_arg2)
{
  printf("\r\n ***********************switch callback function called*****************************"); 
  printf("\r\n switch name = %s",notification_data.switch_name);
  printf("\r\n switch notification type = %d",notification_type);
  
  if(notification_type == 0)
    printf("\r\n SWITCH_ALL_NOTIFICATIONS");
  if(notification_type == 1)
    printf("\r\n DPRM_SWITCH_ADDED");
  if(notification_type == 2)
    printf("\r\n DPRM_SWITCH_DELETED");
  if(notification_type == 3)
    printf("\r\n DPRM_SWITCH_MODIFIED");
  if(notification_type == 6)
    printf("\r\n DPRM_SWITCH_DATAPATH_ATTACHED");
  if(notification_type == 7)
    printf("\r\n DPRM_SWITCH_DATAPATH_DETACHED");
  
  printf("\r\n switch name = %s",notification_data.switch_name); 
  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1); 
  printf(" callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n*********************************************************************\r\n");
}

void dm_notifications_cbk(uint32_t notification_type,
                          uint64_t domain_handle,
                          struct   dprm_domain_notification_data notification_data,
                          void     *callback_arg1,
                          void     *callback_arg2)
{
  printf("\r\n ******************** domain callback function called*****************************");
  printf("\r\n domain name = %s",notification_data.domain_name);
  printf("\r\n domain notification type = %d",notification_type);
 
  if(notification_type == 0)
    printf("\r\n DOMAIN_ALL_NOTIFICATIONS");
  if(notification_type == 1)
    printf("\r\n DPRM_DOMAIN_ADDED");
  if(notification_type == 2)
    printf("\r\n DPRM_DOMAIN_DELETE");
  if(notification_type == 4)
    printf("\r\n DPRM_DOMAIN_OFTABLE_ADDED");
  if(notification_type == 5)         
    printf("\r\n DPRM_DOMAIN_OFTABLE_DELETE");
  if(notification_type == 8)
    printf("\r\n DPRM_DOMAIN_DATAPATH_ATTACHED");   
  if(notification_type == 9)
    printf("\r\n DPRM_DOMAIN_DATAPATH_DETACH");   

  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf(" callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n ********************************************************************************\r\n");
}

void dp_notifications_cbk(uint32_t notification_type,
                          uint64_t datapath_handle,
                          struct   dprm_datapath_notification_data notification_data,
                          void     *callback_arg1,
                          void     *callback_arg2)
{
  printf("\r\n ******************datapath callback function called****************************");
  printf("\r\n datapath id  = %lld",notification_data.dpid);
  printf("\r\n datapath notification type = %d",notification_type);

  if(notification_type == 0)
    printf("\r\n DPRM_DATAPATH_ALL_NOTIFICATIONS");
  if(notification_type == 1) 
    printf("\r\n DPRM_DATAPATH_ADDED");
  if(notification_type == 2)
    printf("\r\n DPRM_DATAPATH_DELETE");
  if(notification_type == 5)
    printf("\r\n DPRM_DATAPATH_PORT_ADDED");
  if(notification_type == 6)
    printf("\r\n DPRM_DATAPATH_PORT_DELETE");

  printf("\r\n callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf(" callback arg1 = %d",*(uint32_t *)callback_arg1);
  printf("\r\n ******************************************************************************\r\n");
}

void dprm_register_switch_test()
{
  uint32_t jj,kk; 
  //test_mempool_module();
    
  switch_test(); 
  domain_test();
  datapath_test();
  for(jj=10000;(jj != 0);--jj)
  {
    for(kk=10000;(kk != 0);--kk)
    {
      int x,y,z;
      x = 8;
      y = 8;
      z = 8;
      z = x*y/z;
      y = z*x;
    }  
  }
  //scanf("%d",jj);  
  getall_switches();
  getall_domains();
  getall_datapaths();
}

int32_t  datapath_test()
{
  uint32_t  callback_arg1 = 2;
  uint32_t  callback_arg2 = 3;
#if 1  
  lRet_value0 = dprm_register_datapath_notifications(DPRM_DATAPATH_ALL_NOTIFICATIONS,
                                                     dp_notifications_cbk,
                                                     (void *)&callback_arg1,
                                                     (void *)&callback_arg2);
#endif
  printf("\r\n*********************************************************************\n");
  printf("\r\n Datapath test begins\n");
  printf("\r\n*********************************************************************\n");

  printf("\r\n Registering datapath dpid = 100 rajeshmsw,rajeshmdom\n");
  datapath_info1.dpid = 100;
  strcpy(datapath_info1.expected_subject_name_for_authentication,"nsmurthy@fsl.com");
  printf("\r\n subject name = %s\n",datapath_info1.expected_subject_name_for_authentication);
  lRet_value1 = dprm_register_datapath(&datapath_info1,sw_handle2,dm_handle1,&dp_handle1);  

  printretvalue(lRet_value1);
  if(lRet_value1 == DPRM_SUCCESS)
  {
    printhandle(dp_handle1);
    printf("\r\n ****Registration of datapath dpid = 100 successful****\n");
  }
  else
   printf("\r\n Registration of datapath dpid = 100 failed\n"); 
  printf("\r\n Registering datapath dpid = 200 nsmurthysw (deleted),mspmurthydom\n");
  datapath_info2.dpid = 200;
  strcpy(datapath_info2.expected_subject_name_for_authentication,"rajeshm@fsl.com");
  lRet_value2 = dprm_register_datapath(&datapath_info2,sw_handle2,dm_handle2,&dp_handle2);

  printretvalue(lRet_value2);
  if(lRet_value2 == DPRM_SUCCESS)
  {
    printhandle(dp_handle2);
    printf("\r\n Registration of datapath dpid = 200 successful\n");
  } 
  else 
    printf("\r\n Registration of datapath dpid = 200 failed\n"); 
  printf("\r\n***************************************************************\n");  
  printf("\r\n get exact dpid 100 sw name rajeshmsw domain name rajeshmdom\n"); 
  lRet_value0 = dprm_get_exact_datapath(dp_handle1,&switch_handle,&domain_handle,&config_info_datapath,&runtime_info_datapath);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(dp_handle1);
    printhandle(switch_handle);
    printhandle(domain_handle);

    printvalues_datapath();
  
    lRet_value0 = dprm_get_exact_switch(switch_handle,&config_info,&runtime_info);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n switch handle = ");
      printhandle(switch_handle);
      printvalues();
    } 
    else
      printf("\r\n Unable to fetch the switch parameters\n");

    lRet_value0 = dprm_get_exact_forwarding_domain(domain_handle,config_info_domain_p,&config_runtime_info_domain);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n\r\n domain handle = ");
      printhandle(domain_handle);
      printvalues_domain();
    }
    else
      printf("\r\n Unable to fetch the domain parameters\n");
  }
  printf("\r\n***************************************************************\n");
  printf("\r\n get exact dpid 200 sw name rajeshmsw domain name rajeshmdom\n");
  lRet_value0 = dprm_get_exact_datapath(dp_handle2,&switch_handle,&domain_handle,&config_info_datapath,&runtime_info_datapath);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(dp_handle2);
    printhandle(switch_handle);
    printhandle(domain_handle);

    printvalues_datapath();
   
    //lRet_value0 = dprm_get_exact_switch(switch_handle,&config_info,&runtime_info);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n switch handle = \n");
      printhandle(switch_handle);
      printvalues();
    } 
    else
      printf("\r\n Unable to fetch the switch parameters\n");

    lRet_value0 = dprm_get_exact_forwarding_domain(domain_handle,config_info_domain_p,&config_runtime_info_domain);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printf("\r\n\r\n domain handle = ");
      printhandle(domain_handle);
      printvalues_domain();
    }
    else
      printf("\r\n Unable to fetch the domain parameters\n");
  }
  #if 1
  printf("\r\n *************************************************************************\n");
  printf("\r\n Delete datapath id 100 and try getting exact datapath info using the same handle\n");
  printf("\r\n");

  lRet_value0 = dprm_unregister_datapath(dp_handle1);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    lRet_value0 = dprm_get_exact_datapath(dp_handle1,&switch_handle,&domain_handle,&config_info_datapath,&runtime_info_datapath);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_ERROR_INVALID_DATAPATH_HANDLE)
      printf("\r\n Datapath not found as it is deleted.\n");
    else
      printf("\r\n Datapath is found even after it is deleted.\n");
  }
  else
    printf("\r\n Datapath with dpid = 100 could not be un registered\n"); 

  #endif
  /***********************Get handle for non existing datapath id 300*********/
  printf("\r\n ******************************************************************\n");
  printf("\r\n Get handle for non existing datapath id 300\n");
  uint64_t dpid1;
  dpid1 = 300;
  lRet_value0 =  dprm_get_datapath_handle(dpid1,&datapath_handle);
  if(lRet_value0 != DPRM_SUCCESS)
    printf("\r\n Failed to get handle for nsmurthy datapath which is not existing\n");
  else
    printf("\r\n Found handle for nsmurthy datapath which is not added\n");
  /***********************Get handle for existing datapath id 100*********/
  printf("\r\n ******************************************************************\n");
  printf("\r\n Get handle for existing datapath id 100\n");
  uint64_t dpid2;
  dpid2 = 100;
  lRet_value0 =  dprm_get_datapath_handle(dpid2,&datapath_handle);

  printretvalue(lRet_value0);

  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(datapath_handle);
    lRet_value0 = dprm_get_exact_datapath(datapath_handle,&sw_handle4,&dm_handle4,&config_info_datapath,&runtime_info_datapath);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues_datapath();
      printhandle(datapath_handle);
    }
  }
  printf("\r\n ***********Test the ports attached to a datapath - Begin*****************\n");
  printf("\r\n Adding port nsmfprt1 to datapth with dpid = 200\n");
  strcpy(port_info1.port_name,"nsmfprt1");
  port_info1.port_id   = 12345678;
  lRet_value2 = dprm_add_port_to_datapath(dp_handle2,&port_info1,&port_handle1);
  printretvalue(lRet_value2);
 
  if(lRet_value2 == DPRM_SUCCESS)
  {
    printhandle(dp_handle2);
    printhandle(port_handle1);

    lRet_value0 = dprm_get_exact_datapath_port(dp_handle2,&config_info_port,&config_runtime_info_port,port_handle1);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printhandle(dp_handle2);
      printhandle(port_handle1);
      printvalues_port();
    }  
  }
  printf("\r\n Adding port abcrfslsprt2 to datapth with dpid = 200\n");
  strcpy(port_info2.port_name,"abcrfslsprt2");
  port_info2.port_id   = 87654321;
  lRet_value2 = dprm_add_port_to_datapath(dp_handle2,&port_info2,&port_handle2);
  printretvalue(lRet_value2);

  if(lRet_value2 == DPRM_SUCCESS)
  {
    printhandle(dp_handle2);
    printhandle(port_handle2);

    lRet_value0 = dprm_get_exact_datapath_port(dp_handle2,&config_info_port,&config_runtime_info_port,port_handle2);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printhandle(dp_handle2);
      printhandle(port_handle2);
      printvalues_port();
    }  
  }
  getall_ports(dp_handle2);
  
  lRet_value0 = dprm_delete_port_from_datapath(dp_handle2,port_handle2);
 
  printf("\r\n *********************************************************************************\n");
  printf("Get internal port structure using with port_name = nsmfprt1 and datapath handle dp_handle2 and show its fields.\r\n");  
 
  get_dp_port(dp_handle2);
  
  printf("\r\n ***********Test the ports attached to a datapath - End  *******************\n");

  printf("\r\n\r\n Get internal datapath structure using with dpid = 100 and show its fields.\r\n");
  
  get_datapath(); /* code in dprmapi.c */
  printf("\r\n Try unregistering datapath with dpid = 200 with one port attached\n");
  dprm_unregister_datapath(dp_handle2);
  getall_datapaths();
  return 0;
}  

int32_t domain_test()
{
  uint32_t  callback_arg1 = 2;
  uint32_t  callback_arg2 = 3;

#if 1 
  lRet_value0 = dprm_register_forwarding_domain_notifications(DPRM_DOMAIN_ALL_NOTIFICATIONS,
                                                              dm_notifications_cbk,
                                                              (void *)&callback_arg1,
                                                              (void *)&callback_arg2);
#endif
  printf("\r\n*********************************************************************\n");
  printf("\r\n Domain test begins\n");
  printf("\r\n*********************************************************************\n");

  ii = (sizeof (struct dprm_distributed_forwarding_domain_info)) /* + (8 * sizeof(struct dprm_table_info)*/;
  config_info_domain_p = (struct dprm_distributed_forwarding_domain_info *)malloc(ii);
  if(config_info_domain_p == NULL)
  {
    printf("\r\n dprmtest - domain test failed as malloc failed for domain table creation\n");
    return 0;
  }

  ii = (sizeof (struct dprm_distributed_forwarding_domain_info)) /* + (2 * sizeof(struct dprm_table_info))*/; 
  domain_info1_p = (struct dprm_distributed_forwarding_domain_info *)malloc(ii);
  if(domain_info1_p == NULL)
  {
    printf("\r\n dprmtest - domain test failed as malloc failed for domain table creation\n");
    return 0;
  }  
  strcpy(domain_info1_p->name,"mspmurthydom");
  strcpy(domain_info1_p->expected_subject_name_for_authentication,"nsmurthy@fsl.com");
  
  lRet_value1 = dprm_create_distributed_forwarding_domain(domain_info1_p,&dm_handle1);

  printretvalue(lRet_value1);
  if(lRet_value1 == DPRM_SUCCESS)
  {
    printf("\r\n --------------domain mspmurthydom successfully added\n");
    printhandle(dm_handle1);
  }
  strcpy(domain_info2.name,"rajeshmdom");
  strcpy(domain_info2.expected_subject_name_for_authentication,"rajeshm@fsl.com");
  lRet_value2 = dprm_create_distributed_forwarding_domain(&domain_info2,&dm_handle2);

  printretvalue(lRet_value2);
  if(lRet_value2 == DPRM_SUCCESS)
  {
    printf("\r\n ------------domain rajeshmdom successfully added\n");
    printhandle(dm_handle2);
  }
  //config_info_domain_p->number_of_tables;
  lRet_value0 = dprm_get_exact_forwarding_domain(dm_handle1,config_info_domain_p,&config_runtime_info_domain);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(dm_handle1);
    printvalues_domain();
    printf("\r\n ----------domain mspmurthydom successfully retrieved\n");
  }
  printf("\r\n next domain\n");
  lRet_value0 = dprm_get_exact_forwarding_domain(dm_handle2,config_info_domain_p,&config_runtime_info_domain);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(dm_handle2);
    printvalues_domain();
    printf("\r\n ----------domain rajeshmdom successfully retrieved\n");
  }
  printf("\r\n");
#if 0
  /* Add two oftables to domain1 */
  ii = (sizeof (struct dprm_table_info)) + (4 * sizeof(struct dprm_table_info));
  dprm_table1_p = (struct dprm_table_info *)(malloc(ii));
  if(dprm_table1_p == NULL)
  {
    printf("\r\n dprmtest - domain test failed as malloc failed for domain table creation\n");
    return 0;
  }

  ii = (sizeof (struct dprm_table_info)) + (2 * sizeof(struct dprm_table_info));
  dprm_table2_p = (struct dprm_table_info *)malloc(ii);
  if(dprm_table2_p == NULL)
  {
    printf("\r\n dprmtest - domain test failed as malloc failed for domain table creation\n");
    return 0;
  }
 
  ii = (sizeof (struct dprm_table_info)) + (8 * sizeof(struct dprm_table_info));
  dprm_table_p = (struct dprm_table_info *)malloc(ii);
  if(dprm_table_p == NULL)
  {
   printf("\r\n dprmtest - domain test failed as malloc failed for domain table creation\n");
   return 0;
  }

  strcpy(dprm_table1_p->table_name,"dprmtabl1");
  dprm_table1_p->table_id   = 1;
  dprm_table1_p->no_of_match_fields = 4;
  
  dprm_table1_p->match_field_p[0].type = 1;
  dprm_table1_p->match_field_p[0].mask = 1;
  
  dprm_table1_p->match_field_p[1].type = 2;
  dprm_table1_p->match_field_p[1].mask = 0;
  
  dprm_table1_p->match_field_p[2].type = 3;
  dprm_table1_p->match_field_p[2].mask = 1;
  
  dprm_table1_p->match_field_p[3].type = 4;
  dprm_table1_p->match_field_p[3].mask = 0;

  strcpy(dprm_table2_p->table_name,"dprmtabl2");
  dprm_table2_p->table_id   = 2;
  dprm_table2_p->no_of_match_fields = 2;
        
  dprm_table2_p->match_field_p[0].type = 5;
  dprm_table2_p->match_field_p[0].mask = 1;
             
  dprm_table2_p->match_field_p[1].type = 6;
  dprm_table2_p->match_field_p[1].mask = 0;

  lRet_value2 = dprm_add_oftable_to_domain(dm_handle1,dprm_table1_p,&oftable_handle1);
  lRet_value3 = dprm_add_oftable_to_domain(dm_handle1,dprm_table2_p,&oftable_handle2);

  printf("\r\n tables added");
  printf("\r\n");
 
  printretvalue(lRet_value2);
  if(lRet_value2 == DPRM_SUCCESS)
  {
    //printhandle(dm_handle1);
    //printhandle(oftable_handle1);
    dprm_table_p->no_of_match_fields = 4;
    printf("\r\n ***get table dprmtabl1 using table handle***");
    lRet_value0 =  dprm_get_exact_domain_oftable(dm_handle1,
                                                 dprm_table_p,
                                                 oftable_handle1);
    if(lRet_value0 == DPRM_SUCCESS)
      printvalues_oftable();
    else
      printf("\r\n Failed to get table1");
   }

 printretvalue(lRet_value3); 
 if(lRet_value3 == DPRM_SUCCESS)
 { 
   //printhandle(dm_handle1);
   //printhandle(oftable_handle2);
   dprm_table_p->no_of_match_fields = 4;
   printf("\r\n ***get table dprmtabl2 using table handle***");
   lRet_value0 = dprm_get_exact_domain_oftable(dm_handle1,
                                               dprm_table_p,
                                               oftable_handle2);
   if(lRet_value0 == DPRM_SUCCESS)
     printvalues_oftable();
   else   
     printf("\r\n Failed to get table2");
 }

 /* get table 1: get handle using table name and get table using table handle*/
 printf("\r\n ***get table using name dprmtabl1***");
 oftable_handle = 0;
 lRet_value1 = dprm_get_oftable_handle(dm_handle1,"dprmtabl1",&oftable_handle);
 if(lRet_value1 == DPRM_SUCCESS)
 {
   //printhandle(oftable_handle);
   dprm_table_p->no_of_match_fields = 4;
   lRet_value0 = dprm_get_exact_domain_oftable(dm_handle1,dprm_table_p,oftable_handle);
   if(lRet_value0 == DPRM_SUCCESS)
     printvalues_oftable();
   else
     printf("\r\n Failed to get table1");
 } 
 else
  printretvalue(lRet_value1);
  /* get table 2: get handle using table name and get table using table handle*/
  printf("\r\n ***get table using name dprmtabl2***");
  oftable_handle = 0;
  lRet_value1 = dprm_get_oftable_handle(dm_handle1,"dprmtabl2",&oftable_handle);
  if(lRet_value1 == DPRM_SUCCESS)
  {
    //printhandle(oftable_handle);
    dprm_table_p->no_of_match_fields = 4;
    lRet_value0 = dprm_get_exact_domain_oftable(dm_handle1,dprm_table_p,oftable_handle);
    if(lRet_value0 == DPRM_SUCCESS)
      printvalues_oftable();
    else
     printf("\r\n Failed to get table2");
  }
  else
    printretvalue(lRet_value1);

  //getall_tables(dm_handle1);
 // getall_tables(dm_handle2);
 
  lRet_value0 = dprm_delete_oftable_from_domain(dm_handle1,oftable_handle1);
  if(lRet_value0 == DPRM_SUCCESS)
    printf("\r\n Successfully deleted dprmtabl1 dm_handle1 oftable_handle2"); 
  else
    printf("\r\n Failureto delete dprmtabl1 dm_handle1 oftable_handle2");

  //getall_tables(dm_handle1);
  
  printf("\r\n idbased table list - domain name = mspmurthydom,table name = dprmtabl2");
  
  perform_tests_on_idbased_table();
  
  printf("\r\n table tests -ve");
#endif

  printf("\r\n *******Add rajeshmdom domain again*****************************************");
  
  strcpy(domain_info3.name,"rajeshmdom");
  strcpy(domain_info3.expected_subject_name_for_authentication,"rajeshm@fsl.com");

  lRet_value3 = dprm_create_distributed_forwarding_domain(&domain_info3,&dm_handle3);
 
  printretvalue(lRet_value3);
  if(lRet_value3 == DPRM_SUCCESS)
  {
    printhandle(dm_handle3);
    lRet_value0 = dprm_get_exact_forwarding_domain(dm_handle3,config_info_domain_p,&config_runtime_info_domain);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printhandle(dm_handle3);
      printvalues_domain();
    }
  }
  #if 1 
  printf("\r\n *************************************************************************\n");
  printf("\r\n Delete mspmurthydom domain and try getting exact domain info using the same handle\n");
  printf("\r\n");
  lRet_value0 = dprm_unregister_distributed_forwarding_domain(dm_handle1);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    lRet_value0 = dprm_get_exact_forwarding_domain(dm_handle1,config_info_domain_p,&config_runtime_info_domain);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_ERROR_INVALID_DOMAIN_HANDLE)
      printf("\r\n Domain not found as it is deleted.\n");
    else
      printf("\r\n Domain is found even after it is deleted.\n");
  } 
  #endif
  /***********************Get handle for non existing domain name nsmurthy*********/
  printf("\r\n ******************************************************************\n");
  printf("\r\n Get handle for non existing domain name nsmurthy\n");
  char* name_domain_p = (char*)malloc(12);
  name_domain_p = "nsmurthy";
  lRet_value0 =  dprm_get_forwarding_domain_handle(name_domain_p,&dm_handle4);
  if(lRet_value0 != DPRM_SUCCESS)
    printf("\r\n Failed to get handle for nsmurthy domain which is not existing\n");
  else
    printf("\r\n Found handle for nsmurthy domain which is not added\n");
  /***********************Get handle for existing domain name rajeshmdom*********/
  printf("\r\n ******************************************************************\n");
  printf("\r\n Get handle for existing domain name rajeshmdom\n");
  char* name_1234_domain_p = (char*)malloc(12);
  name_1234_domain_p = "rajeshmdom";
  lRet_value0 =  dprm_get_forwarding_domain_handle(name_1234_domain_p,&dm_handle5);

  printretvalue(lRet_value0);

  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(dm_handle5);
    lRet_value0 = dprm_get_exact_forwarding_domain(dm_handle5,config_info_domain_p,&config_runtime_info_domain);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues_domain();
      printhandle(dm_handle5);
    }
  }
  printf("\r\n\r\n Get internal domain structure using with name = mspmurthydom and show its fields.\r\n");
  get_domain();
  getall_domains();
  return 0;
}  
void switch_test(void)
{
  uint32_t  callback_arg1 = 1;
  uint32_t  callback_arg2 = 2;
#if 1
  lRet_value0 = dprm_register_switch_notifications(DPRM_SWITCH_ALL_NOTIFICATIONS,
                                                   sw_notifications_cbk,
                                                   (void *)&callback_arg1,
                                                   (void *)&callback_arg2);
#endif
  printf("\r\n*********************************************************************\n");
  printf("\r\n Switch test begins\n");
  printf("\r\n*********************************************************************\n");
  printf("\r\n nsmurthysw addition\n");
  strcpy(switch_info1.name,"nsmurthysw");
  switch_info1.switch_type = PHYSICAL_SWITCH;
  switch_info1.ipv4addr.baddr_set = TRUE;
  switch_info1.ipv4addr.addr = 0xc0a80c0c;
  strcpy(switch_info1.switch_fqdn,"http:/nsm/fsl.com.");
  lRet_value1 = dprm_register_switch(&switch_info1,&sw_handle1);
  
  printretvalue(lRet_value1);
  if(lRet_value1 == DPRM_SUCCESS)
    printhandle(sw_handle1);
 
  printf("\r\n rajeshmsw addition\n"); 
  strcpy(switch_info2.name,"rajeshmsw");
  switch_info2.switch_type = VIRTUAL_SWITCH;
  switch_info2.ipv4addr.baddr_set = TRUE;
  switch_info2.ipv4addr.addr = 0xc0a8c6c6;
  strcpy(switch_info2.switch_fqdn,"http:/rajesh/freescale.com.");
  lRet_value2 = dprm_register_switch(&switch_info2,&sw_handle2);

  printretvalue(lRet_value2);
  if(lRet_value2 == DPRM_SUCCESS)
    printhandle(sw_handle2);

  printf("\r\n apallisrisw addition\n");
  strcpy(switch_info4.name,"apaisnri9sw");
  switch_info4.switch_type = PHYSICAL_SWITCH;
  switch_info4.ipv4addr.baddr_set = TRUE;
  switch_info4.ipv4addr.addr = 0xc0a80c04;
  strcpy(switch_info4.switch_fqdn,"http:/srini/freescale.com.");
  lRet_value4 = dprm_register_switch(&switch_info4,&sw_handle4);

  printretvalue(lRet_value4);
  if(lRet_value4 == DPRM_SUCCESS)
    printhandle(sw_handle4);

  printf("\r\n srteja1234sw addition\n");
  strcpy(switch_info5.name,"abtejaamtbsw");
  switch_info5.switch_type = PHYSICAL_SWITCH;
  switch_info5.ipv4addr.baddr_set = TRUE;
  switch_info5.ipv4addr.addr = 0xc0a80c0a;
  strcpy(switch_info5.switch_fqdn,"http:/steja/freescale.com.");
  lRet_value5 = dprm_register_switch(&switch_info5,&sw_handle5);

  printretvalue(lRet_value5);
  if(lRet_value5 == DPRM_SUCCESS)
    printhandle(sw_handle5);
  
#if 1
  printf("\r\n *******Add rajeshmsw switch again*****************************************");
  strcpy(switch_info3.name,"rajeshmsw");
  switch_info3.switch_type = VIRTUAL_SWITCH;
  switch_info3.ipv4addr.baddr_set = TRUE;
  switch_info3.ipv4addr.addr = 0xc0a8c6c6;
  strcpy(switch_info3.switch_fqdn,"http:/nsm/fsl.com.");
  lRet_value3 = dprm_register_switch(&switch_info3,&sw_handle3);

  printretvalue(lRet_value3);
  if(lRet_value3 == DPRM_SUCCESS)
  {
    printhandle(sw_handle3);
    lRet_value0 = dprm_get_exact_switch(sw_handle3,&config_info,&runtime_info);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_SUCCESS)
    {
      printhandle(sw_handle3);
      printvalues();
    } 
  }
#endif  
  printf("\r\n *****************get rajeshmsw sw info using valid handle****************");
  lRet_value0 = dprm_get_exact_switch(sw_handle2,&config_info,&runtime_info);
  printretvalue(lRet_value0);

  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues();
    printhandle(sw_handle2);
  }

  printf("\r\n *****************get nsmurthysw sw info using valid handle****************"); 
  lRet_value0 = dprm_get_exact_switch(sw_handle1,&config_info,&runtime_info);
  printretvalue(lRet_value0);

  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues();
    printhandle(sw_handle1);
  }
  printf("\r\n *************************************************************************\n");  
  #if 1 
  printf("\r\n Delete nsmurthysw sw and try getting exact switch info using the same handle\n");

  lRet_value0 = dprm_unregister_switch(sw_handle1);
  printretvalue(lRet_value0);
#endif
#if 1
  if(lRet_value0 == DPRM_SUCCESS)
  {
    lRet_value0 = dprm_get_exact_switch(sw_handle1,&config_info,&runtime_info);
    printretvalue(lRet_value0);

    if(lRet_value0 == DPRM_ERROR_INVALID_SWITCH_HANDLE)
      printf("\r\n Switch not found as it is deleted.\n");
    else
      printf("\r\n Switch is found even after it is deleted.\n");
  }
  //lRet_value0 = dprm_unregister_switch(sw_handle2); 
  lRet_value0 = dprm_unregister_switch(sw_handle4);
  lRet_value0 = dprm_unregister_switch(sw_handle5);
#endif
#if 1
/*******************Get handle for non existing switch name nsmurthysw1*********/  
   printf("\r\n ******************************************************************\n");
   printf("\r\n Get handle for non existing switch name nsmurthysw1\n");
   char* name_p = (char*)malloc(12);
   name_p = "nsmurthysw1";
   lRet_value0 =  dprm_get_switch_handle(name_p,&sw_handle3);
   if(lRet_value0 != DPRM_SUCCESS)
     printf("\r\n Failed to get handle for nsmurthysw1 switch which is not existing\n");  
   else
     printf("\r\n Found handle for nsmurthysw1 sw which is not added\n");
#endif
/******************Get handle for nsmurthysw switch which is added earlier ****/
  printf("\r\n ******************************************************************\n");
  printf("\r\n Get handle for existing switch name nsmurthysw\n");

  char* name12_p = (char*)malloc(12); 
  name12_p = "nsmurthysw";
  lRet_value0 =  dprm_get_switch_handle(name12_p,&sw_handle3);
  printretvalue(lRet_value0);

  if(lRet_value0 == DPRM_SUCCESS)
  {
    printhandle(sw_handle3);
    printf("\r\n");
    #if 1   
    lRet_value0 = dprm_get_exact_switch(sw_handle3,&config_info,&runtime_info);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues();
      printhandle(sw_handle3);
    }
    #endif
  }
#if 1  
/*******************Get handle for rajeshmsw switch which is added earlier****/
  printf("\r\n ******************************************************************\n");
  printf("\r\n Get handle for existing switch name rajeshmsw\n");
  
  char * name1_p = "rajeshmsw";
  lRet_value0 =  dprm_get_switch_handle(name1_p,&sw_handle3);
  printretvalue(lRet_value0);

  if(lRet_value0 ==  DPRM_SUCCESS)
  {
    printhandle(sw_handle3);
    
    lRet_value0 = dprm_get_exact_switch(sw_handle3,&config_info,&runtime_info);
    printretvalue(lRet_value0); 
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues();
      printhandle(sw_handle3);
    }  
  }
  /*************************************************************************************/
  printf("\r\n***************************************************************\n");
  lRet_value0 = dprm_get_exact_switch(sw_handle4,&config_info,&runtime_info);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues();
    printhandle(sw_handle4);
  }
  printf("\r\n"); 
  lRet_value0 = dprm_get_exact_switch(sw_handle5,&config_info,&runtime_info);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues();
    printhandle(sw_handle5);
  }
#endif
  //getall_switches();
  //printf("\r\n\r\n Get internal switch structure using with name = nsmurthysw and show its fields.\r\n");
  //get_switch();
}  
/********************************************************************************/
void getall_switches(void)
{  
  int32_t lRet_value0;
  printf("\r\n****************************************************************"); 
  printf("\r\n Get all the switches\n");
  lRet_value0 =  dprm_get_first_switch(&config_info,&runtime_info,&switch_handle);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues(); 
    printhandle(switch_handle);
  } 
  else
   return;
  for(lRet_value0 = DPRM_SUCCESS;lRet_value0 == DPRM_SUCCESS;)
  {
    printf("\r\n dprmtest - Get the next Switch\n");
    lRet_value0 = dprm_get_next_switch(&config_info,&runtime_info,&switch_handle);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues();
      printhandle(switch_handle);
    } 
    else
    {
      if(lRet_value0 == DPRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n dprm test -  No more switches found\n");
      }    
      else if(lRet_value0 == DPRM_ERROR_INVALID_SWITCH_HANDLE)
      {
        printf("\r\n dprm test - Invalid handle\n");
      }    
      break; 
    }
  }
} 
/*************************************switch**********************************************************/
void printvalues(void) 
{
   printf("\r\n switch name = %s",config_info.name);
   printf("\r\n switch type = %d",config_info.switch_type);
   printf("\r\n switch ipv4 address = %x",config_info.ipv4addr.addr);
   printf("\r\n Switch fqdn = %s",config_info.switch_fqdn);

   printf("\r\n number of datapaths = %d",runtime_info.number_of_datapaths);
   printf("\r\n number of attributes = %d",runtime_info.number_of_attributes);
}
/*****************************************************************************************************/
void printhandle(uint64_t handle)
{
  int ii;
  uchar8_t *swhandle;

  printf(" handle = ");
  swhandle =(uchar8_t *)(&handle);
  for(ii=0;ii<8;ii++)
    printf(" %x",swhandle[ii]);
}
/******************************************************************************************************/
void printretvalue(int32_t ret_value)
{
  if(ret_value == DPRM_ERROR_NAME_LENGTH_NOT_ENOUGH)
    dprm_debug("\r\n%s:%d Name shall contain atleast 8 chars \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_INVALID_SWITCH_TYPE)
    dprm_debug("\r\n%s:%d Invalid switch type \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_INVALID_IPADDR)
    dprm_debug("\r\n%s:%d Invalid FQDN or IP Address \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_NO_MEM)
    dprm_debug("\r\n%s:%d Unable to allocate memory for the resource \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_DUPLICATE_RESOURCE)
    dprm_debug("\r\n%s:%d Duplicate resource found in the database \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_FAILURE)
   dprm_debug("\r\n%s:%d Failed to register the resource\r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_INVALID_SWITCH_HANDLE)
    dprm_debug("\r\n%s:%d Switch not present in the database \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_INVALID_DOMAIN_HANDLE)
    dprm_debug("\r\n%s:%d Domain not present in the database \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_INVALID_DATAPATH_HANDLE)
     dprm_debug("\r\n%s:%d Datapath not present in the database \r\n",__FUNCTION__,__LINE__);
#if 0
  if(ret_value == DPRM_ERROR_INVALID_OFTABLE_HANDLE)
     dprm_debug("\r\n%s:%d OFTABLE not present in the database \r\n",__FUNCTION__,__LINE__);
#endif
  if(ret_value == DPRM_ERROR_INVALID_PORT_HANDLE)
     dprm_debug("\r\n%s:%d Port not present in the database \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_NO_MORE_ENTRIES)
    dprm_debug("\r\n%s:%d No more Resources present in the database \r\n",__FUNCTION__,__LINE__);
  if(ret_value == DPRM_ERROR_RESOURCE_NOTEMPTY)
     dprm_debug("\r\n%s:%d Resource is not empty and is not deleted. \r\n",__FUNCTION__,__LINE__);
}
/********************************************************************************/
void getall_domains(void)
{
  int32_t lRet_value0;
  
  printf("\r\n****************************************************************");
  printf("\r\n Get all the domain\n");

  lRet_value0 =  dprm_get_first_forwarding_domain(config_info_domain_p,&config_runtime_info_domain,&domain_handle);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues_domain();
    printhandle(domain_handle);
  }
  for(lRet_value0 = DPRM_SUCCESS;lRet_value0 == DPRM_SUCCESS;)
  {
    printf("\r\n dprmtest - Get the next Domain\n");
    lRet_value0 = dprm_get_next_forwarding_domain(config_info_domain_p,&config_runtime_info_domain,&domain_handle);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues_domain();
      printhandle(domain_handle);
    }
    else
    {
      if(lRet_value0 == DPRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n dprm test -  No more domains found\n");
      }
      else if(lRet_value0 == DPRM_ERROR_INVALID_DOMAIN_HANDLE)
      {
        printf("\r\n dprm test - Invalid handle\n");
      }
      break;
   }
 }
}
/************************************************************************************/
void getall_datapaths(void)
{
  int32_t lRet_value0;

  printf("\r\n****************************************************************");
  printf("\r\n Get all the datapaths\n");

  lRet_value0 =  dprm_get_first_datapath(&config_info_datapath,&runtime_info_datapath,&datapath_handle);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues_datapath();
    printhandle(datapath_handle);
  }
  for(lRet_value0 = DPRM_SUCCESS;lRet_value0 == DPRM_SUCCESS;)
  {
    printf("\r\n dprmtest - Get the next Datapath\n");
    lRet_value0 = dprm_get_next_datapath(&config_info_datapath,&runtime_info_datapath,&datapath_handle);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues_datapath();
      printhandle(datapath_handle);
    }
    else
    {
      if(lRet_value0 == DPRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n dprm test -  No more datapaths found\n");
      }
      else if(lRet_value0 == DPRM_ERROR_INVALID_DATAPATH_HANDLE)
      {
        printf("\r\n dprm test - Invalid handle\n");
      }
      break;
    }
  }
}
/*************************************************************************************/
void getall_ports(uint64_t dp_handle)
{
  int32_t lRet_value0;

  printf("\r\n****************************************************************");
  printf("\r\n Get all the ports in the specified datapath \n");

  lRet_value0 =  dprm_get_first_datapath_port(dp_handle,&config_info_port,&config_runtime_info_port,&port_handle);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues_port();
    printhandle(dp_handle);
    printhandle(port_handle);
  }
  for(lRet_value0 = DPRM_SUCCESS;lRet_value0 == DPRM_SUCCESS;)
  {
    printf("\r\n dprmtest - Get the next port in the Datapath\n");
    lRet_value0 = dprm_get_next_datapath_port(dp_handle,&config_info_port,&config_runtime_info_port,&port_handle);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues_port();
      printhandle(dp_handle);
      printhandle(port_handle);
    }
    else
    {
      if(lRet_value0 == DPRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n dprm test - No more ports in the datapath found\n");
      }
      else if(lRet_value0 == DPRM_ERROR_INVALID_PORT_HANDLE)
      {
        printf("\r\n dprm test - Invalid handle\n");
      }
      break;
    }
  }
}
#if 0
/*************************************************************************************/
void getall_tables(uint64_t dm_handle)
{
  int32_t lRet_value0;
  
  printf("\r\n****************************************************************");
  printf("\r\n Get all the tables in the specified domain \n");

  dprm_table_p->no_of_match_fields = 8;
  lRet_value0 = dprm_get_first_domain_oftable(dm_handle,dprm_table_p,&oftable_handle);
  printretvalue(lRet_value0);
  if(lRet_value0 == DPRM_SUCCESS)
  {
    printvalues_oftable();
    printhandle(dm_handle);
    printhandle(oftable_handle);
  }
  for(lRet_value0 = DPRM_SUCCESS;lRet_value0 == DPRM_SUCCESS;)
  {
    printf("\r\n dprmtest - Get the next table in the domain\n");
   
    dprm_table_p->no_of_match_fields = 8;
    lRet_value0 = dprm_get_next_domain_oftable(dm_handle,dprm_table_p,&oftable_handle);
    printretvalue(lRet_value0);
    if(lRet_value0 == DPRM_SUCCESS)
    {
      printvalues_oftable();
      printhandle(dm_handle);
      printhandle(oftable_handle);
    }
    else
    {
      if(lRet_value0 == DPRM_ERROR_NO_MORE_ENTRIES)
      {
        printf("\r\n dprm test - No more tables in the domain found\n");
      }
      else if(lRet_value0 == DPRM_ERROR_INVALID_OFTABLE_HANDLE)
      {
        printf("\r\n dprm test - Invalid handle\n");
      }
      break;
    }
  }
}
#endif
/************************************************************************************/
void printvalues_domain(void)
{
  printf("\r\n domain  name = %s",config_info_domain_p->name);
  printf("\r\n subject name = %s",config_info_domain_p->expected_subject_name_for_authentication);
  printf("\r\n number of name based tables created sofar = %d",config_info_domain_p->number_of_tables);

  printf("\r\n number of id based tables created so far = %d",config_runtime_info_domain.number_of_tables);
  printf("\r\n number of datapaths = %d",config_runtime_info_domain.number_of_datapaths);
  printf("\r\n number of attributes = %d",config_runtime_info_domain.number_of_attributes);
}  
/*************************************************************************************/
void printvalues_datapath(void)
{
  printf("\r\n dpid = %lld",config_info_datapath.dpid);
  printf("\r\n subject name = %s",config_info_datapath.expected_subject_name_for_authentication);
  
  printf("\r\n number of ports      = %d",runtime_info_datapath.number_of_ports);
  printf("\r\n number of attributes = %d",runtime_info_datapath.number_of_attributes);
}
/*************************************************************************************/
void printvalues_oftable()
{
  uint32_t ii;

  printf("\r\n table_name = %s\n",dprm_table_p->table_name);
  printf(" table_id   = %d\n",dprm_table_p->table_id);
  printf(" no_of_match_fields = %d\n",dprm_table_p->no_of_match_fields);

  for(ii=0;ii < dprm_table_p->no_of_match_fields;ii++)
  {
    printf(" mf type: %d",dprm_table_p->match_field_p[ii].type);
    printf("   mf mask: %d\n",dprm_table_p->match_field_p[ii].mask);
  }  
}  
void printvalues_port(void)
{
  printf("\r\n port id   = %d",config_info_port.port_id);
  printf("\r\n port name = %s",config_info_port.port_name);

  printf("\r\n dummy = %d\n",config_runtime_info_port.dummy);
}

int32_t ret_value;

void test_mempool_module()
{
  uint32_t switch_entries_max    = 22;//32;
  uint32_t switch_static_entries = 18;//28; 
  struct mempool_params mempool_info={};

  #if 1 
  mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
  mempool_info.block_size = sizeof(struct test_entry);
  mempool_info.max_blocks = switch_entries_max;
  mempool_info.static_blocks = switch_static_entries;
  mempool_info.threshold_min = 6;
  mempool_info.threshold_max = 12;
  mempool_info.replenish_count = 2;
  mempool_info.b_memset_not_required =  FALSE;
  mempool_info.b_list_per_thread_required =  FALSE;
  mempool_info.noof_blocks_to_move_to_thread_list = 0;

  ret_value = mempool_create_pool("dprm_test_pool", &mempool_info,
		  &dprm_test_mempool_g);
  ret_value =  mempool_get_stats(dprm_test_mempool_g,&stats);
  print_mempool_stats();
  printf("\r\n");  
  for(ii =0;ii<26;ii++)
  {
    heap_b = 1;
    ret_value = mempool_get_mem_block(dprm_test_mempool_g,(uchar8_t**)(&(test_mem_block[ii])),&heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      printf(" mem block not allocated.");
    else
    {
      printf(" mem block allocated,heap status.%d,%d",ii,heap_b);
      test_mem_block[ii]->heap_b = heap_b;
    }
    ret_value =  mempool_get_stats(dprm_test_mempool_g,&stats);
    print_mempool_stats();
  } 
  printf("\r\n"); 
  
  for(ii = 0;ii<22;ii++)
  {
    ret_value = mempool_release_mem_block(dprm_test_mempool_g,(uchar8_t *)test_mem_block[ii],test_mem_block[ii]->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      printf(" mem block not released.");
    else
    {
      printf(" mem block released,heap_status.%d,%d",ii,test_mem_block[ii]->heap_b);
      ret_value =  mempool_get_stats(dprm_test_mempool_g,&stats);
      print_mempool_stats();
    } 
  }
  ret_value = mempool_delete_pool(dprm_test_mempool_g);
  if(ret_value == DPRM_SUCCESS)
    printf("\r\n Mempool is deleted successfully\n");
#endif
#if 0  
  switch_entries_max    = 22;//32;
  switch_static_entries = 18;//28; 
  
  ret_value = mempool_create_pool("dprm_test_pool",
                                   sizeof(struct test_entry),
                                   switch_entries_max,switch_static_entries,
                                   6/*16*/,12/*20*/,
                                   2, FALSE,TRUE,3,
                                   &dprm_test_mempool_g);

  ret_value =  mempool_get_stats(dprm_test_mempool_g,&stats);
  print_mempool_stats();
  printf("\r\n");  

  for(ii =0;ii<26;ii++)
  {
    heap_b = 1;
    ret_value = mempool_get_mem_block(dprm_test_mempool_g,(uchar8_t**)(&(test_mem_block[ii])),&heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      printf(" mem block not allocated.");
    else
    {
      printf(" mem block allocated,heap status.%d,%d",ii,heap_b);
      test_mem_block[ii]->heap_b = heap_b;
    }
    ret_value =  mempool_get_stats(dprm_test_mempool_g,&stats);
    print_mempool_stats();
  } 
  printf("\r\n"); 

  for(ii = 0;ii<22;ii++)
  {
    ret_value = mempool_release_mem_block(dprm_test_mempool_g,(uchar8_t *)test_mem_block[ii],test_mem_block[ii]->heap_b);
    if(ret_value != MEMPOOL_SUCCESS)
      printf(" mem block not released.");
    else
    {
      printf(" mem block released.%d",ii);
      ret_value =  mempool_get_stats(dprm_test_mempool_g,&stats);
      print_mempool_stats();
    }
 }
#endif  
  printf("\r\n");
  printf("\r\n");
}
void print_mempool_stats(void)
{
  printf("\r\n");
  printf("\r\n free_blocks_count                 = %d",stats.free_blocks_count);
  printf(" allocated_blocks_count            = %d",stats.allocated_blocks_count);
  printf(" released_blocks_count             = %d",stats.released_blocks_count);

  printf("\r\n heap_free_blocks_count            = %d",stats.heap_free_blocks_count);
  printf("  heap_allocated_blocks_count       = %d",stats.heap_allocated_blocks_count);
  printf("  heap_released_blocks_count        = %d",stats.heap_released_blocks_count);


  printf("\r\n allocation_fail_blocks_count      = %d",stats.allocation_fail_blocks_count);
  printf(" released_fail_blocks_count        = %d",stats.released_fail_blocks_count);
  printf(" heap_allocation_fail_blocks_count = %d",stats.heap_allocation_fail_blocks_count);

  
  printf("\r\n static_allocated_blocks_count     = %d",stats.static_allocated_blocks_count);
  printf(" heap_allowed_blocks_count         = %d",stats.heap_allowed_blocks_count);
  printf(" total_blocks_count                = %d",stats.total_blocks_count);
/*
  printf("\r\n block_size                        = %d",stats.block_size);
                                           
  printf("\r\n threshold_max                     = %d",stats.threshold_max);
  printf("\r\n threshold_min                     = %d",stats.threshold_min);
  printf("\r\n replenish_count                   = %d",stats.replenish_count);
*/  
}
