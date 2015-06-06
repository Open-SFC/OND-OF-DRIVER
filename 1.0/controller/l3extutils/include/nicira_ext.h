/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/
#ifndef __NICIRA_EXT_H
#define __NICIRA_EXT_H 
//#define NX_VENDOR_ID 0x0000049F
#define NX_VENDOR_ID 0x00002320
#define NXM_HEADER__(VENDOR, FIELD, HASMASK, LENGTH) \
    (((VENDOR) << 16) | ((FIELD) << 9) | ((HASMASK) << 8) | (LENGTH))
#define NXM_HEADER(VENDOR, FIELD, LENGTH) \
    NXM_HEADER__(VENDOR, FIELD, 0, LENGTH)
#define NXM_HEADER_W(VENDOR, FIELD, LENGTH) \
    NXM_HEADER__(VENDOR, FIELD, 1, (LENGTH) * 2)
#define NXM_VENDOR(HEADER) ((HEADER) >> 16)
#define NXM_FIELD(HEADER) (((HEADER) >> 9) & 0x7f)
#define NXM_TYPE(HEADER) (((HEADER) >> 9) & 0x7fffff)
#define NXM_HASMASK(HEADER) (((HEADER) >> 8) & 1)
#define NXM_LENGTH(HEADER) ((HEADER) & 0xff)

//! Nicira Match Extensions
/*! This module lists all the Nicira extensions.
 *  \addtogroup Nicira_Extensions
 */

/** \ingroup Nicira_Extensions 
 * <b>Metadata registers:</b>\n
 *
 * Registers initially have value 0.  Actions allow register values to be
 * manipulated.
 *
 * Prereqs: None.
 *
 * Format: Array of 64-bit integer registers.  Space is reserved for up to
 * NXM_NX_MAX_REGS registers, but switches may implement fewer.
 * 
 * Freescale - Modifying NICIRA actual 32-bit registers to 64 bit size.

 * Masking: Arbitrary masks.
 */

#define NXM_NX_MAX_REGS 16
#define NXM_NX_REG(IDX)   NXM_HEADER  (0x0001, IDX, 8)
#define NXM_NX_REG_W(IDX) NXM_HEADER_W(0x0001, IDX, 8)
#define NXM_NX_REG_IDX(HEADER) NXM_FIELD(HEADER)
#define NXM_IS_NX_REG(HEADER) (!((((HEADER) ^ NXM_NX_REG0)) & 0xffffe1ff))
#define NXM_IS_NX_REG_W(HEADER) (!((((HEADER) ^ NXM_NX_REG0_W)) & 0xffffe1ff))
#define NXM_NX_REG0       NXM_HEADER  (0x0001, 0, 8)
#define NXM_NX_REG0_W     NXM_HEADER_W(0x0001, 0, 8)
#define NXM_NX_REG1       NXM_HEADER  (0x0001, 1, 8)
#define NXM_NX_REG1_W     NXM_HEADER_W(0x0001, 1, 8)
#define NXM_NX_REG2       NXM_HEADER  (0x0001, 2, 8)
#define NXM_NX_REG2_W     NXM_HEADER_W(0x0001, 2, 8)
#define NXM_NX_REG3       NXM_HEADER  (0x0001, 3, 8)
#define NXM_NX_REG3_W     NXM_HEADER_W(0x0001, 3, 8)
#define NXM_NX_REG4       NXM_HEADER  (0x0001, 4, 8)
#define NXM_NX_REG4_W     NXM_HEADER_W(0x0001, 4, 8)
#define NXM_NX_REG5       NXM_HEADER  (0x0001, 5, 8)
#define NXM_NX_REG5_W     NXM_HEADER_W(0x0001, 5, 8)
#define NXM_NX_REG6       NXM_HEADER  (0x0001, 6, 8)
#define NXM_NX_REG6_W     NXM_HEADER_W(0x0001, 6, 8)
#define NXM_NX_REG7       NXM_HEADER  (0x0001, 7, 8)
#define NXM_NX_REG7_W     NXM_HEADER_W(0x0001, 7, 8)

#define FLOW_N_REGS 8
#define OFPERR_OFS (1 << 30)
#define NX_ACTION_LOAD_REG_LEN      sizeof(struct nx_action_reg_load )
/* nx action copy length need  2 * oxm header length */
#define NX_ACTION_COPY_FIELD_LEN    (sizeof(struct eric_action_copy_field) + ( 2* sizeof(uint32_t)) )

#define NICIRA_REGISTER_LEN    12  /** OXM Header len (4) + size of Register (8) = 12 */
#define NICIRA_REGISTER_MASK_LEN 20 /** OXM Header len (4) + size of Register (8) + mask len (8)= 20 */

#define NICIRA_REGISTER_DATA_LEN 12 /** OXM Header len (4) + size of Register (8) = 12 */

#define NICIRA_MATCH_FIELD_START_ID 100

/** \ingroup Nicira_Extensions 
 *  \enum mf_field_id
 *  <b>Description:</b>\n
 *  Meta-flow match fields.
 */
enum mf_field_id 
{
#if FLOW_N_REGS > 0
    MFF_REG0 = NICIRA_MATCH_FIELD_START_ID,                   
#endif
#if FLOW_N_REGS > 1
    MFF_REG1,                   
#endif
#if FLOW_N_REGS > 2
    MFF_REG2,
#endif
#if FLOW_N_REGS > 3
    MFF_REG3, 
#endif
#if FLOW_N_REGS > 4
    MFF_REG4,
#endif
#if FLOW_N_REGS > 5
    MFF_REG5,
#endif
#if FLOW_N_REGS > 6
    MFF_REG6, 
#endif
#if FLOW_N_REGS > 7
    MFF_REG7,
#endif
};

/* Use this macro as CASE_MFF_REGS: in a switch statement to choose all of the
 * MFF_REGx cases. */
#if FLOW_N_REGS == 1
# define CASE_MFF_REGS                                          \
    case MFF_REG0
#elif FLOW_N_REGS == 2
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1
#elif FLOW_N_REGS == 3
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1: case MFF_REG2
#elif FLOW_N_REGS == 4
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1: case MFF_REG2: case MFF_REG3
#elif FLOW_N_REGS == 5
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1: case MFF_REG2: case MFF_REG3: \
    case MFF_REG4
#elif FLOW_N_REGS == 6
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1: case MFF_REG2: case MFF_REG3: \
    case MFF_REG4: case MFF_REG5
#elif FLOW_N_REGS == 7
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1: case MFF_REG2: case MFF_REG3: \
    case MFF_REG4: case MFF_REG5: case MFF_REG6
#elif FLOW_N_REGS == 8
# define CASE_MFF_REGS                                          \
    case MFF_REG0: case MFF_REG1: case MFF_REG2: case MFF_REG3: \
    case MFF_REG4: case MFF_REG5: case MFF_REG6: case MFF_REG7
#else
# error
#endif

enum nx_error_message
{
  OFPERR_OFPBAC_BAD_ARGUMENT = OFPERR_OFS, 
};

enum nx_action_subtype 
{
    /** struct nx_action_reg_load */
    NXAST_REG_LOAD = 900,             
    /** struct nx_action_reg_copy */
    NXAST_COPY_FIELD = 901           
};

struct nx_action_header {
    /** OFPAT_EXPERIMENTER. */
    uint16_t type;            
    /** Length is 16. */
    uint16_t len;             
    /** NX_VENDOR_ID. */
    uint32_t vendor;          
    /** NXAT_* */
    uint16_t subtype;         
    /** Pad to 64 bits */
    uint8_t  pad[6];
};
OFP_ASSERT(sizeof(struct nx_action_header) == 16);

/** \ingroup Nicira_Extensions 
 *  \struct nx_action_reg_load
 *  \brief Action structure for NXAST_REG_LOAD.\n
 *  <b>Description:</b>\n
 * Copies value[0:n_bits] to dst[ofs:ofs+n_bits], where a[b:c] denotes the bits
 * within 'a' numbered 'b' through 'c' (not including bit 'c').  Bit numbering
 * starts at 0 for the least significant bit, 1 for the next most significant
 * bit, and so on.
 *
 * 'dst' is an nxm_header with nxm_hasmask=0. See the documentation for
 * NXAST_REG_MOVE (above) for the permitted fields and for the side effects of
 * loading them.
 *
 * The 'ofs' and 'n_bits' fields are combined into a single 'ofs_nbits' field
 * to avoid enlarging the structure by another 8 bytes.  To allow 'n_bits' to
 * take a value between 1 and 64 (inclusive) while taking up only 6 bits, it is
 * also stored as one less than its true value:
 *
 *  15                           6 5                0
 * +------------------------------+------------------+
 * |              ofs             |    n_bits - 1    |
 * +------------------------------+------------------+
 *
 * The switch will reject actions for which ofs+n_bits is greater than the
 * width of 'dst', or in which any bits in 'value' with value 2**n_bits or
 * greater are set to 1, with error type OFPET_BAD_ACTION, code
 * OFPBAC_BAD_ARGUMENT.
 */
struct nx_action_reg_load {
    /** OFPAT_VENDOR. */
    uint16_t type;                  
    /** Length is 24. */
    uint16_t len;                   
    /** NX_VENDOR_ID. */
    uint32_t vendor;                
    /** NXAST_REG_LOAD. */
    uint16_t subtype;               
    /** (ofs << 6) | (n_bits - 1). */
    uint16_t ofs_nbits;             
    /** Destination register. */
    uint32_t dst;                   
    /** Immediate value. */
    uint64_t value;                 
};
OFP_ASSERT(sizeof(struct nx_action_reg_load) == 24);

#define NX_OXM_IDS_NO_OF_FIELDS(length) \
  ((length > sizeof(struct eric_action_copy_field )) ? ( (length - sizeof(struct eric_action_copy_field ))/sizeof(uint32_t) ): 0) 

/** \ingroup Nicira_Extensions 
 *  \struct eric_action_copy_field
 *  \brief Action structure for NXAST_COPY_FIELD.\n
 *  <b>Description:</b>\n
 *  Action structure for NXAST_COPY_FIELD.
 */
struct eric_action_copy_field 
{
  /** OFPAT_EXPERIMENTER. */
  uint16_t type; 
  /** Length is 16. */
  uint16_t len; 
  /** NX_VENDOR_ID. */
  uint32_t vendor;                
  /** NXAST_COPY_FIELD. */
  uint16_t subtype;               
  /** Number of bits to copy. */
  uint16_t num_bits; 
  /** Starting bit offset in source */
  uint16_t src_offset; 
  /** Starting bit offset in destination */
  uint16_t dst_offset; 
  /* Followed by:
  * - Exactly (len - 4) bytes containing the source and destination
  OXM headers, then
  * - Exactly (len + 7)/8*8 - (len) (between 0 and 7)
  * bytes of all-zero bytes */
  /** The source and destination OXM headers */
  uint32_t oxm_src_dst[0]; 
};
OFP_ASSERT(sizeof(struct eric_action_copy_field ) == 16);

/** \ingroup Nicira_Extensions 
 *  \struct fslx_action_copy_field_from_next_flow
 *  \brief Action structure for NXAST_COPY_FIELD_FROM_NEXT_FLOW.\n
 *  <b>Description:</b>\n
 *  Action structure for NXAST_COPY_FIELD_FROM_NEXT_FLOW.
 */
struct fslx_action_copy_field_from_next_flow 
{
  /** OFPAT_EXPERIMENTER. */
  uint16_t type; 
  /** Length is 24. */
  uint16_t len; 
  /** NX_VENDOR_ID. */
  uint32_t vendor; 
  /** NXAST_COPY_FIELD_FROM_NEXT_FLOW. */
  uint16_t subtype; 
  /** Pad to 64 bits */
  uint8_t pad[6]; 
  /** OXM Source ID */
  uint32_t src_id; 
  /** Destination ID */
  uint32_t dst_id; 
};
OFP_ASSERT(sizeof(struct fslx_action_copy_field_from_next_flow ) == 24);
#endif
