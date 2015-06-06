
#define RRM_SUCCESS  0
#define RRM_FAILURE -1

#define RRM_ERROR_INCOMPLETE_RESOURCE_RELATIONSHIPS            -2
#define RRM_ERROR_NWPORT_NAME_NULL                             -3
#define RRM_ERROR_INVALID_VN_NAME                              -3
#define RRM_ERROR_INVALID_VM_NAME                              -4
#define RRM_ERROR_INVALID_VN_HANDLE                            -5
#define RRM_ERROR_INVALID_VM_HANDLE                            -6 
#define RRM_ERROR_INVALID_SWITCH_NAME                          -7
#define RRM_ERROR_INVALID_SWITCH_INFO                          -8
#define RRM_ERROR_FAILED_TO_ADD_CRM_COMPUTE_NODE               -9 
#define RRM_ERROR_CRM_CN_NODE_OR_CRM_LOGICAL_SWITCH_NOT_FOUND  -10
#define RRM_ERROR_INVALID_CRM_PORT_HANDLE                      -11

int32_t rrm_check_crm_nwport_resource_relationships(char* switch_name_p,
                                                    char* br_name_p,
                                                    uint64_t* switch_handle_p,
                                                    uint64_t* logical_switch_handle_p,
                                                    uint32_t* switch_ip);


int32_t rrm_check_crm_vmport_resource_relationships(char* switch_name_p,
                                                    char* br_name_p,
                                                    char* vn_name_p, 
                                                    char* vm_name_p,
                                                    uint64_t* switch_handle_p,
                                                    uint64_t* logical_switch_handle_p,
                                                    uint64_t* vn_handle_p,
                                                    uint64_t* vm_handle_p,
                                                    uint32_t* switch_ip_p,
                                                    uint8_t*  nw_type_p);


int32_t rrm_setup_crm_vmport_resource_relationships(char* port_name_p,
                                                    uint64_t crm_port_handle,
                                                    uint8_t  port_type,  
                                                    char* vm_name_p,
                                                    char* vn_name_p,
                                                    char* switch_name_p,
                                                    char* br_name_p);


int32_t rrm_delete_crm_vmport_resource_relationships(char*     port_name,
                                                     uint64_t  port_handle,
                                                     uint8_t   port_type,
                                                     char*     switch_name,
                                                     char*     vm_name,
                                                     char*     vn_name,
                                                     uint64_t  vn_handle, 
                                                     uint64_t  logical_switch_handle);

int32_t rrm_get_vm_logicalswitch_side_ports(char*      vm_name,
                                            uint64_t   vm_handle,
                                            struct     crm_virtual_machine* crm_vm_node_p,
                                            uint64_t*  port_saferef_p);



int32_t rrm_delete_vmport_resource_relationships(char* port_name, uint64_t port_handle,uint8_t port_type,
                                                 char* switch_name, char* vm_name, char* vn_name,
                                                 uint64_t  vn_handle,
                                                 uint64_t  logical_switch_handle);
int32_t rrm_check_crmport_resource_relationships(char* switch_name_p,char* br_name_p,
                                                 char* vn_name_p, char* vm_name_p,
                                                 uint64_t* switch_handle_p,uint64_t* logical_switch_handle_p,
                                                 uint64_t* vn_handle_p, uint64_t* vm_handle_p,
                                                 uint32_t* switch_ip_p,uint8_t* nw_type_p);

