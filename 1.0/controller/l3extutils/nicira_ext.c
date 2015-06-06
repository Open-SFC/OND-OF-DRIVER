/*Nicira Extension source file */
#ifdef NICIRA_EXT_SUPPORT
#include <stdint.h>
#include "controller.h"
#include "dprm.h"
#include "nicira_ext.h"


#define OF_OXM_HDR_LEN   (4)
int32_t
of_get_nicira_register_match_field(struct of_msg *msg,
                    void *match_fields,
                    uint16_t match_fields_len,
		    uint32_t reg_index,
                    uint64_t *reg_value
		    )
{
	uint32_t          oxm_header;
	uint8_t          *ptr;
	uint16_t          parsed_data_len;

        OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_DEBUG, "Entered");
	cntlr_assert(match_fields != NULL);

	if (( NXM_FIELD(reg_index) < 0 )|| (NXM_FIELD(reg_index) > 3))
	{
		OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_ERROR, "invalid reg_index");
		return OF_FAILURE;
	}
        if(unlikely(NULL == match_fields) && (0 != match_fields_len))
        {
		OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_ERROR, " match_fields is NULL. something is badly wrong \r\n");
		return OF_FAILURE;
        }
        
	ptr =  (uint8_t*)match_fields;
	while(match_fields_len)
	{
		oxm_header = ntohl(*(uint32_t*)ptr);
		//		if(OXM_CLASS(oxm_header) ==	NX_VENDOR_ID)
		//		if(OXM_CLASS(oxm_header) != OFPXMC_OPENFLOW_BASIC)
		if( (NXM_IS_NX_REG(oxm_header)) || NXM_IS_NX_REG_W(oxm_header))
		{
			OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_DEBUG, "nicira matchfiled");
			if ( oxm_header == reg_index)
			{
				*reg_value = ntohll(*(uint64_t*)(ptr+4));
				OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_DEBUG, "reg index =%d value =%llx" , reg_index, *reg_value);
				return OF_SUCCESS;
			}
		}
		parsed_data_len = (OF_OXM_HDR_LEN + OXM_LENGTH(oxm_header));
		OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_DEBUG, "parsed data len %d", parsed_data_len);
		match_fields_len -= parsed_data_len;
		ptr             += parsed_data_len;
	}
	return OF_FAILURE;
}
/* Register Match field extension */
int32_t
of_set_nicira_register_match_field(struct of_msg *msg,
                            uint64_t *data, uint64_t *data_mask, uint8_t reg_index, uint8_t is_mask_set)
{
   struct of_msg_descriptor *msg_desc;

   msg_desc = &msg->desc;

   if(is_mask_set)
   {
  
      if( (msg_desc->ptr1 + msg_desc->length1 + NICIRA_REGISTER_MASK_LEN ) >
          (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
      {
         OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
         return OFU_NO_ROOM_IN_BUFFER;
      }

      switch(reg_index)
      {
        case MFF_REG0:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG0_W);
         break;
        case MFF_REG1:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG1_W);
         break;
        case MFF_REG2:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG2_W);
         break;
        case MFF_REG3:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG3_W);
         break;
        case MFF_REG4:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG4_W);
         break;
        case MFF_REG5:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG5_W);
         break;
        case MFF_REG6:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG6_W);
         break;
        case MFF_REG7:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG7_W);
         break;          
      }

      *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + 4) = htonll(*data); 
      *(uint64_t*)(msg_desc->ptr1+msg_desc->length1  + NICIRA_REGISTER_LEN)  = htonll(*data_mask); 

       msg_desc->length1 += NICIRA_REGISTER_MASK_LEN ;
   }
   else
   {
      if( (msg_desc->ptr1+msg_desc->length1+NICIRA_REGISTER_DATA_LEN ) >
          (msg->desc.start_of_data + msg_desc->buffer_len)   ) 
      {
         OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN,"%s %d :Length of buffer is not sufficient to add data\r\n",__FUNCTION__,__LINE__);
         return OFU_NO_ROOM_IN_BUFFER;
      }

      switch(reg_index)
      {
        case MFF_REG0:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG0);
         break;
        case MFF_REG1:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG1);
         break;
        case MFF_REG2:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG2);
         break;
        case MFF_REG3:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG3);
         break;
        case MFF_REG4:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG4);
         break;
        case MFF_REG5:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG5);
         break;
        case MFF_REG6:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG6);
         break;
        case MFF_REG7:
         *(uint32_t*)(msg_desc->ptr1+msg_desc->length1) = htonl(NXM_NX_REG7);
         break;          
      }

      *(uint64_t*)(msg_desc->ptr1+msg_desc->length1 + 4) = htonll(*data); 

       msg_desc->length1 += NICIRA_REGISTER_DATA_LEN;
   }

   return OFU_SET_FIELD_SUCCESS;
}
/* EX: of_set_nicira_register_match_field(msg, data, data_mask, MFF_REG0, 1 ); //if mask is present
of_set_nicira_register_match_field(msg, data, data_mask, MFF_REG0, 0 ); //if NO mask is exists
*/

/* LOAD Register Instruction */
/*index = Register index MFF_REG0 to MFF_REG7.
* data =   value that should be written in to the register.
* offset =  bit position from where this value should be written.
* n_bits =  data size/ number of bits to be written to register.
*/
int32_t
nicira_action_write_register(struct of_msg *msg, uint32_t index, uint64_t data, uint8_t offset, uint8_t n_bits)
{
   uint64_t data_n_bits ;
   struct of_msg_descriptor *msg_desc;
   struct nx_action_reg_load *reg_load;

   msg_desc = &msg->desc;
   if ((msg_desc->ptr3+msg_desc->length3 + (NX_ACTION_LOAD_REG_LEN)) >
       (msg->desc.start_of_data + msg_desc->buffer_len))
   {
      OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN, "Length of buffer is not suffeceient to add data");
      return OFU_NO_ROOM_IN_BUFFER;
   }

	/* Reject if a bit numbered 'n_bits' or higher is set to 1 in data. */
	if(n_bits > 64)
	{
		OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN, "n_bits value greater than 64");
		return OFPERR_OFPBAC_BAD_ARGUMENT;
	}
	data_n_bits = data >> n_bits;
	if (data_n_bits) 
	{
		OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN, "data_n_bits is non zero");
		return OFPERR_OFPBAC_BAD_ARGUMENT;
	}
	reg_load = 
		(struct nx_action_reg_load *)(msg_desc->ptr3+msg_desc->length3);

	reg_load->type = htons(OFPAT_EXPERIMENTER);// OFPAT_VENDOR); 
	reg_load->len = htons(NX_ACTION_LOAD_REG_LEN);
	reg_load->vendor = htonl(NX_VENDOR_ID);
	reg_load->subtype = htons(NXAST_REG_LOAD);
	reg_load->ofs_nbits = htons((offset << 6) | (n_bits -1));
	reg_load->dst = htonl(index);
	reg_load->value = htonll(data);

	msg_desc->length3 += (NX_ACTION_LOAD_REG_LEN);
	return OFU_SET_FIELD_SUCCESS;
}

/* Ex: nicira_action_copy_field(struct of_msg *msg, uint32_t src_oxm, uint32_t dst_oxm,
* 		uint16_t src_offset, uint16_t dst_offset, uint16_t num_bits,
* 		uint32_t src_len, uint32_t dst_len); 
*/

/* src_oxm - oxm header of src field. 
* dst_oxm - oxm header of dst field. 
* copy_field->type = OFPAT_EXPERIMENTER;
* copy_field->len = NX_ACTION_COPY_FIELD_LEN + (2 * sizeof(oxm_src_dst));
* copy_field->vendor = NX_VENDOR_ID;
* copy_field->subtype = NXAST_COPY_FIELD;
* copy_field->num_bits = 32;
* copy_field->src_offset = 0;
* copy_field->dst_offset = 0;
* copy_field->oxm_src_dst[0] = OFPXMT_OFB_ETH_DST;
* copy_field->oxm_src_dst[1] = MFF_REG0; 
* nicira_action_copy_field(copy_field, OFU_IPV4_DST_FIELD_LEN,NICIRA_REGISTER_LEN);
*/
/* Copy from next flow */
int32_t
nicira_action_copy_field(struct of_msg *msg, uint32_t src_oxm, uint32_t dst_oxm,
			uint16_t src_offset, uint16_t dst_offset, 	uint16_t num_bits)
{
  uint32_t src_class = OXM_CLASS(src_oxm);
  uint32_t dst_class = OXM_CLASS(dst_oxm);
  uint32_t src_field_len = OXM_LENGTH(src_oxm);
  uint32_t dst_field_len = OXM_LENGTH(dst_oxm);
  uint32_t src_field_id = OXM_FIELD(src_oxm);
  uint32_t dst_field_id = OXM_FIELD(dst_oxm);
  struct of_msg_descriptor *msg_desc;
  struct eric_action_copy_field *copy_data;

   msg_desc = &msg->desc;
   if ((msg_desc->ptr3+msg_desc->length3 + (NX_ACTION_COPY_FIELD_LEN)) >
       (msg->desc.start_of_data + msg_desc->buffer_len)) 
   {
      OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN,
                 "Length of buffer is not suffeceient to add data");
      return OFU_NO_ROOM_IN_BUFFER;
   }

  OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_DEBUG,
	    "src_oxm = 0x%x, dst_oxm = 0x%x, src_offset = %d, dst_offset = %d, num_bits = %d",
            src_oxm, dst_oxm, src_offset, dst_offset, num_bits);
  if (src_class == OFPXMC_OPENFLOW_BASIC)
  {
	  switch (src_field_id)	{
		  case OFPXMT_OFB_ARP_SHA:
		  case OFPXMT_OFB_ARP_THA:
		  case OFPXMT_OFB_ETH_DST:
		  case OFPXMT_OFB_ETH_SRC:
			  OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_ERROR,
			  "src_field_id=%d is not supported for this action",src_field_id);
			  return OFPET_BAD_ACTION;
	  }

  }

  if (dst_class == OFPXMC_OPENFLOW_BASIC)
  {
	  switch(dst_field_id)	{
		  case OFPXMT_OFB_ARP_SHA:
		  case OFPXMT_OFB_ARP_THA:
		  case OFPXMT_OFB_ETH_DST:
		  case OFPXMT_OFB_ETH_SRC:
			  OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_ERROR,
			  "dst_field_id=%d is not supported for this action",dst_field_id);
			  return OFPET_BAD_ACTION;
	  }
  }
//  if((copy_field->dst_offset + copy_field->num_bits) > dst_len)
  //  return OFPET_BAD_ACTION;
  if (((dst_offset + num_bits) > (dst_field_len * 8)) ||
	  ((src_offset + num_bits) > (src_field_len * 8))
	 )
  {
    OF_LOG_MSG(OF_LOG_EXT_NICIRA, OF_LOG_WARN,
	 "num_bits=%d is > src_field_len=%d or dst_field_len=%d",num_bits, src_field_len, dst_field_len);
    return OFPET_BAD_ACTION;
  }
  
  copy_data = (struct eric_action_copy_field *)(msg_desc->ptr3 + msg_desc->length3);

  copy_data->type = htons(OFPAT_EXPERIMENTER);
  copy_data->len = htons(NX_ACTION_COPY_FIELD_LEN);
  copy_data->vendor = htonl(NX_VENDOR_ID);
  copy_data->subtype = htons(NXAST_COPY_FIELD);
  copy_data->num_bits = htons(num_bits);
  copy_data->src_offset = htons(src_offset);
  copy_data->dst_offset = htons(dst_offset);

  copy_data->oxm_src_dst[0] = htonl(src_oxm);
  copy_data->oxm_src_dst[1] = htonl(dst_oxm);
 
  msg_desc->length3 += (NX_ACTION_COPY_FIELD_LEN); 
  return OFU_SET_FIELD_SUCCESS;
}

#endif
