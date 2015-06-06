



/** \ingroup Flow_Description
 *  \struct of_flow_mod_info
 *  \brief flow description details\n\n
 *  <b>Description </b>\n 
 *  Data structure to hold flow mod all fields are converted into host byte order\n
 */

#define OFU_FLOW_MATCH_FIELD_LEN  46
#define OFU_FLOW_INSTRUCTION_LEN  16
#define OFU_FLOW_ACTION_LEN  10


#define OFU_FLOW_COOKIE_START  0x12345678
#define OFU_FLOW_COOKIE_MASK   0xFFFFFFFF 

/*ERROR MACROS */

#define OF_FLOWMOD_ERROR_INVALID_MISSENTRY 11
#define OF_FLOWMOD_GOTOTABLE_INVALID 22


struct ofi_flow_match_fields
{
  of_list_node_t   next_match_field;
  uint16_t      field_id;
  uint8_t       field_type;
  uint8_t       field_len;

  uint8_t       ui8_data;
  uint16_t       ui16_data;
  uint32_t      ui32_data;
  uint64_t      ui64_data;
  uint8_t       ui8_data_array[6];
  ipv6_addr_t   ipv6_addr;

  //void          match_field_val[OFU_FLOW_MATCH_FIELD_LEN];
  uint8_t       is_mask_set;
  uint8_t       mask[OFU_FLOW_MATCH_FIELD_LEN];
  uint8_t       mask_len;
};
#define OF_FLOW_MATCH_FILED_LISTNODE_OFFSET offsetof(struct ofi_flow_match_fields, next_match_field)

struct ofi_flow_action
{
  of_list_node_t  next_action;
  uint16_t      action_id;
  uint8_t       action_type;
  uint8_t       action_len;
  
  uint32_t	port_no;
  uint16_t	max_len;
  uint8_t       ttl;
  uint16_t      ether_type;
  uint32_t	queue_id;
  uint32_t	group_id;
  int8_t       setfield_type;
  uint8_t       ui8_data;
  uint16_t       ui16_data;
  uint32_t      ui32_data;
  uint64_t      ui64_data;
  uint8_t       ui8_data_array[6];
  ipv6_addr_t   ipv6_addr;

};
#define OF_FLOW_ACTION_LISTNODE_OFFSET offsetof(struct ofi_flow_action, next_action)

struct ofi_flow_instruction
{ 
 of_list_node_t   next_instruction;
 uint16_t      inst_id;
 uint8_t       instruction_type;
 uint8_t       instruction_len;

 uint8_t       ui8_data;
 uint16_t      ui16_data;
 uint32_t      ui32_data;
 uint64_t      ui64_data;
 uint8_t       ui8_data_array[6];

 uint8_t       is_mask_set;
 uint8_t       mask[OFU_FLOW_INSTRUCTION_LEN];
 uint8_t       mask_len;

 of_list_t        action_list;
};

#define OF_FLOW_INSTRUCTION_LISTNODE_OFFSET offsetof(struct ofi_flow_instruction, next_instruction)
struct ofi_flow_mod_info
{
   /** Flow Id*/
   of_list_node_t   next_flow;
   uint32_t  flow_id;
   uint8_t   table_id;
   uint16_t  priority;
   uint8_t   command;
   uint16_t  idle_timeout;
   uint16_t  hard_timeout;
   uint64_t  cookie;
   uint64_t  cookie_mask;
   uint8_t   flow_flags;
   uint32_t  out_port;
   uint32_t  out_group;
   of_list_t    match_field_list; 
   of_list_t    instruction_list; 
   of_list_t    action_list; 
};
#define OF_FLOW_MOD_LISTNODE_OFFSET offsetof(struct ofi_flow_mod_info, next_flow)

int32_t of_flow_set_instruction_data(struct ofi_flow_instruction *inst_info,
                                    char *data, uint32_t data_len);

