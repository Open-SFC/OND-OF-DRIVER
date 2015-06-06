
#ifndef __OF_METER_H
#define __OF_METER_H


enum meter_commands
{
        ADD_METER=1,
        ADD_METER_BAND=2,
        MOD_METER=3,
        DEL_METER=4
};

struct meter_trans{
	of_list_node_t     *next_node;
        uint64_t datapath_handle;
        uint32_t trans_id;
        uint32_t meter_id;
        uint32_t command_id;
        uint32_t sub_command_id;
};

#define OF_METER_TRANS_LISTNODE_OFFSET  offsetof(struct meter_trans, next_node)

#define OF_METER_RATE_REQUIRED 10
#define OF_METER_PREC_LEVEL_REQUIRED 20

#define OF_METER_GENERATE_TRANS_ID(id)\
{\
        id++;\
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"trans Id %d",id);\
}

int32_t of_register_meter(uint64_t datapath_handle, struct ofi_meter_rec_info *meter_desc_p);
int32_t of_update_meter(uint64_t datapath_handle, struct ofi_meter_rec_info *meter_info_p);
int32_t of_register_band_to_meter(uint64_t datapath_handle,struct ofi_meter_band *band_p, uint32_t meter_id);
int32_t of_update_band_in_meter(uint64_t datapath_handle, struct ofi_meter_band *band_info_p, uint32_t meter_id);
int32_t of_unregister_meter(uint64_t datapath_handle, uint32_t meter_id);
int32_t of_get_meter(uint64_t datapath_handle,  uint32_t meter_id,  struct ofi_meter_rec_info **meter_entry_p);
int32_t of_meter_unregister_band(uint64_t datapath_handle,uint32_t meter_id, uint32_t band_id); /* single band*/
int32_t of_meter_unregister_bands(uint64_t datapath_handle,uint32_t meter_id);/* all bands in meter*/
int32_t of_meter_frame_message_request(struct of_msg *msg, struct ofi_meter_rec_info *meter_desc_p);
int32_t of_meter_frame_and_send_message_request(uint64_t dapath_handle, uint32_t meter_id, uint8_t command);
int32_t of_meter_frame_response(char *msg, struct ofi_meter_rec_info *meter_entry_p, uint32_t msg_length);
int32_t of_meter_frame_band_response(char *msg, struct ofi_meter_band **band_entry_pp, uint32_t *msg_length);
int32_t of_add_meter(struct of_msg *msg, uint64_t  datapath_handle,uint32_t  meter_id, uint16_t meter_type,                      void **conn_info_pp);
int32_t of_modify_meter(struct of_msg *msg,uint64_t datapath_handle,uint32_t  meter_id,uint16_t meter_type,                     void **conn_info_pp);
int32_t of_delete_meter(struct of_msg *msg, uint64_t datapath_handle,uint32_t  meter_id,void **conn_info_pp);

#endif
