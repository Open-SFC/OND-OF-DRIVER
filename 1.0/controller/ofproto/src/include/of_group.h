
#ifndef __OF_GROUP_H
#define __OF_GROUP_H


enum group_commands
{
        ADD_GROUP=1,
        ADD_GROUP_BUCKET=2,
        ADD_GROUP_BUCKET_ACTION=3,
        MOD_GROUP=4,
        DEL_GROUP=5
};

struct group_trans{
	of_list_node_t     *next_node;
        uint64_t datapath_handle;
        uint32_t trans_id;
        uint32_t group_id;
        uint32_t command_id;
        uint32_t sub_command_id;
};
#define OF_GROUP_TRANS_LISTNODE_OFFSET offsetof(struct group_trans, next_node)

#define OF_GROUP_WEIGHT_PARAM_REQUIRED 10
#define OF_GROUP_WATCH_PORT_OR_GROUP_REQUIRED 20
#define OF_GROUP_BUCKET_LIMIT_EXCEEDED 30

#define OF_GROUP_GENERATE_TRANS_ID(id)\
{\
        id++;\
          OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"trans Id %d",id);\
}

int32_t of_register_group(uint64_t datapath_handle, struct ofi_group_desc_info *group_desc_p);
int32_t of_update_group(uint64_t datapath_handle, struct ofi_group_desc_info *group_info_p);
int32_t of_register_bucket_to_group(uint64_t datapath_handle,struct ofi_bucket *bucket_p, uint32_t group_id);
int32_t of_update_bucket_in_group(uint64_t datapath_handle, struct ofi_bucket *bucket_info_p, uint32_t group_id);
int32_t of_register_action_to_bucket(uint64_t datapath_handle, struct ofi_action *action_p, uint32_t group_id, uint32_t bucket_id);
int32_t of_unregister_group(uint64_t datapath_handle, uint32_t group_id);
int32_t of_get_group(uint64_t datapath_handle,  uint32_t group_id,  struct ofi_group_desc_info **group_entry_p);
int32_t of_group_unregister_bucket(uint64_t datapath_handle,uint32_t group_id, uint32_t bucket_id); /* single bucket*/
int32_t of_group_unregister_buckets(uint64_t datapath_handle,uint32_t group_id);/* all buckets in group*/
int32_t of_group_frame_action_request(struct of_msg *msg, struct ofi_action  *action_p);
int32_t of_group_frame_message_request(struct of_msg *msg, struct ofi_group_desc_info *group_desc_p);
int32_t of_group_frame_and_send_message_request(uint64_t dapath_handle, uint32_t group_id, uint8_t command);
int32_t of_group_frame_response(char *msg, struct ofi_group_desc_info *group_entry_p, uint32_t msg_length);
int32_t of_group_frame_bucket_response(char *msg, uint8_t   group_type,  struct ofi_bucket **bucket_entry_pp, uint32_t *msg_length);
int32_t of_group_frame_action_response(char *msg, struct ofi_action **action_entry_pp, uint32_t *length);
#endif
