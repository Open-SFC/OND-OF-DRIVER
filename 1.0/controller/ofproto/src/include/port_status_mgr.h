

#define PSM_SUCCESS 0
#define PSM_FAILURE -1

#define SYSTEM_PORT_NOT_READY  0
#define SYSTEM_PORT_READY      1

#define PORT_STATUS_UP         2
#define PORT_STATUS_DOWN       3
#define PORT_STATUS_UPDATE     4
#define PORT_STATUS_CREATED    5

int32_t psm_module_init();
int32_t psm_module_uninit();

void  psm_get_port_status(uint64_t logical_switch_handle,
                          char* port_name_p,uint64_t* dp_port_handle_p,
                          uint8_t* port_status_p, uint32_t* port_id_p);




