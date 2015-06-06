
#ifndef __OF_FLOW_MOD_H
#define __OF_FLOW_MOD_H


enum flow_commands
{
     ADD_FLOW=1,
     MODIFY_FLOW=2,
     DEL_FLOW=3
};

struct flow_trans{
    of_list_node_t     *next_flow;
    uint64_t datapath_handle;
    uint32_t trans_id;
    uint8_t  table_id;
    uint16_t  priority;
    uint32_t flow_id;
    uint32_t command_id;
    uint32_t sub_command_id;
};
#define OF_FLOW_TRANS_LISTNODE_OFFSET  offsetof(struct flow_trans, next_flow)

#define OF_FLOW_GENERATE_TRANS_ID(id)\
{\
    id++;\
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"trans Id %d",id);\
}

#endif


