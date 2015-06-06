/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/
/*
 *
 * File name: fc_view.h
 * Author: Varun Kumar Reddy <B36461@freescale.com>
 * Date:   03/11/2013
 * Description: 

@Description   This file contains the flow control db view  related 
                      macros, data structures
*//***************************************************************************/

#ifndef __FCVIEW_H
#define __FCVIEW_H


#define OF_VIEW_VXN_VMPORTS_MAC               "VxnVMPortsMac"
#define OF_VIEW_VXN_VMPORTS_SWITCH            "VxnVMPortsSwitch"
#define OF_VIEW_VXN_NW_UNICAST_PORTS_SWITCH   "VxnNwUnicastPortsSwitch"
#define OF_VIEW_VXN_NW_BROADCAST_PORTS_SWITCH "VxnNwBroadcastPortsSwitch"
#define OF_VIEW_RESERVED                      "reserved"
                                                                         


/* This structure represents view of dmac-vmports .
* this structure holds fc_dmac_vmport_view_node info*/
struct fc_dmac_vmport_database_view
{
  char view[64];/* view name */
  struct mchash_table *db_viewtbl_p;/* Hash table holding the fc_dmac_vmport_view_node nodes. */
  void                *db_viewmempool_p; 
  uint32_t no_of_view_node_buckets_g;
};
/* This structure represents one node in the hash table of a given
* fc_dmac_vmport_database_view.It contains the crm_port handle,
* destination mac and tunnel id
* */
struct fc_dmac_vmport_view_node
{
  struct   mchash_table_link  view_tbl_lnk; /* to link this view node to the fc_dmac_vmport_database_view Hash table */
  uint8_t  dst_mac[6];/* dst_mac is view node name*/
  uint32_t tun_id;/* tun_id is the view node value */
  uint64_t port_handle;/* crm_port handle */
  int32_t  index;
  int32_t  magic;
  uint8_t  heap_b;
};
#define FC_DMAC_VMPORT_VIEW_NODE_TBL_OFFSET offsetof(struct fc_dmac_vmport_view_node, view_tbl_lnk)




/* This structure represents view of swname-vmports .
* this structure holds fc_swname_vmport_view_node info*/
struct fc_swname_vmport_database_view
{
  char view[64];/* view name */
  struct mchash_table *db_viewtbl_p;/* Hash table holding the database_view_nodes. */
  void                *db_viewmempool_p; 
  uint32_t no_of_view_node_buckets_g;
  uint8_t  uniqueness_flag;        /* uniqueness_flag =1 node_name is not used in hashing. */
  uint8_t  valid_b;        /* validity */
};
struct fc_swname_vmport_view_node
{
  struct   mchash_table_link  view_tbl_lnk;  /* to link this view node to the view Hash table */
  char   swname[64];//this is valid for swname view.
  uint32_t tun_id;
  uint64_t port_handle;/* safe reference to the crm_port node */
  int32_t  index;
  int32_t  magic;
  uint8_t  heap_b;
};
#define FC_SWNAME_VMPORT_VIEW_NODE_TBL_OFFSET offsetof(struct fc_swname_vmport_view_node, view_tbl_lnk)



/* This structure represents view of swname-nw_unicast ports .
* this structure holds fc_swname_nw_unicast_port_view_node*/
struct fc_swname_nw_unicast_port_database_view
{
  char view[64];/* view name */
  struct mchash_table *db_viewtbl_p;/* Hash table holding the database_view_nodes. */
  void                *db_viewmempool_p; 
  uint32_t no_of_view_node_buckets_g;
  uint8_t  uniqueness_flag;        /* uniqueness_flag =1 node_name is not used in hashing. */
  uint8_t  valid_b;        /* validity */
};

struct fc_swname_nw_unicast_port_view_node
{
  struct   mchash_table_link  view_tbl_lnk;  /* to link this view node to the view Hash table */
  char   swname[64];//this is valid for swname view.
  uint16_t service_port;
  uint64_t port_handle;/* safe reference to the crm_port node */
  int32_t  index;
  int32_t  magic;
  uint8_t  heap_b;
};
#define FC_SWNAME_NW_UNICAST_PORT_VIEW_NODE_TBL_OFFSET offsetof(struct fc_swname_nw_unicast_port_view_node, view_tbl_lnk)







/* This structure represents view of swname-nw_broadcast ports .
* this structure holds fc_swname_nw_unicast_port_view_node*/
struct fc_swname_nw_broadcast_port_database_view
{
  char view[64];/* view name */
  struct mchash_table*  db_viewtbl_p;/* Hash table holding the database_view_nodes. */
  void                *db_viewmempool_p; 
  uint32_t no_of_view_node_buckets_g;
  uint8_t  uniqueness_flag;        /* uniqueness_flag =1 node_name is not used in hashing. */
  uint8_t  valid_b;        /* validity */
};
struct fc_swname_nw_broadcast_port_view_node
{
  struct   mchash_table_link  view_tbl_lnk;  /* to link this view node to the view Hash table */
  char  swname[64];//this is valid for swname view.
  uint16_t serviceport;
  uint32_t remote_ip;
  uint32_t tun_id;
  uint64_t port_handle;/* safe reference to the crm_port node */
  int32_t  index;
  int32_t  magic;
  uint8_t  heap_b;
};
#define FC_SWNAME_NW_BROADCAST_PORT_VIEW_NODE_TBL_OFFSET offsetof(struct fc_swname_nw_broadcast_port_view_node, view_tbl_lnk)


#define RCUNODE_OFFSET_IN_MCHASH_TBLLNK offsetof( struct mchash_table_link, rcu_head)

/* flow control views initialization */
int32_t of_fc_dmac_vmport_view_init(char *view_name_p);
int32_t of_fc_swname_vmport_view_uninit();
void fc_dmac_vmport_free_view_node_rcu(struct rcu_head *db_view_node_entry_p);
int32_t  crm_add_vmport_to_vmside_portmac_view(uint64_t crm_port_handle,uint32_t  vni);
int32_t  crm_del_vmport_from_vmside_portmac_view(uint64_t crm_port_handle, uint32_t  vni);
int32_t  crm_get_vxn_vmport_by_dmac_vni(uint8_t*  dst_mac_p, uint32_t  tun_id, struct  crm_port**  dst_crm_port_p_p);
void    of_fc_get_view_node_mempoolentries(uint32_t* view_node_entries_max,uint32_t* view_node_static_entries);

#endif
