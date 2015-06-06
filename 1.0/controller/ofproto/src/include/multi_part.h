
#ifndef __CNTLR_MULTIPART_H
#define __CNTLR_MULTIPART_H


void
cntlr_handle_port_stats_msg(
		uint64_t controller_handle,
		uint64_t domain_handle,
		uint64_t datapath_handle,
		uint8_t  port_no,
		struct of_msg *msg,
		void  *cbk_arg1,
		void  *cbk_arg2,
		uint8_t status,
		struct ofi_port_stats_info *port_stats,
		uint8_t more_ports);


void
cntlr_handle_table_stats_msg(
		uint64_t controller_handle,
		uint64_t domain_handle,
		uint64_t datapath_handle,
		uint8_t  port_no,
		struct of_msg *msg,
		void  *cbk_arg1,
		void  *cbk_arg2,
		uint8_t status,
		struct ofi_table_stats_info *table_stats,
		uint8_t more_ports);


void
cntlr_handle_aggregate_stats_msg(
		uint64_t controller_handle,
		uint64_t domain_handle,
		uint64_t datapath_handle,
		uint8_t  port_no,
		struct of_msg *msg,
		void  *cbk_arg1,
		void  *cbk_arg2,
		uint8_t status,
		struct ofi_aggregate_flow_stats *aggregate_stats,
		uint8_t more_ports);

void
cntlr_handle_flow_stats_msg(
		uint64_t controller_handle,
		uint64_t domain_handle,
		uint64_t datapath_handle,
		uint8_t  port_no,
		struct of_msg *msg,
		void  *cbk_arg1,
		void  *cbk_arg2,
		uint8_t status,
		struct ofi_flow_entry_info *flow_stats,
		uint8_t more_ports);



int32_t
of_get_aggregate_flow_stats(struct of_msg *msg,
		uint64_t datapath_handle,
		uint8_t  table_id,
		uint32_t out_port,
		uint32_t out_group,
		uint64_t cookie,
		uint64_t cookie_mask,
		of_aggregate_stats_cbk_fn aggregat_stats_cbk_fn,
		void *cbk_arg1,
		void *cbk_arg2);


int32_t
of_get_flow_entries(struct of_msg *msg,
		uint64_t datapath_handle,
		uint8_t  table_id,
		uint32_t out_port,
		uint32_t out_group,
		uint64_t cookie,
		uint64_t cookie_mask,
		of_flow_entry_stats_cbk_fn flow_entry_cbk_fn,
		void *cbk_arg1,
		void *cbk_arg2);

int32_t
of_get_table_features( struct of_msg *msg,
                       uint64_t datapath_handle,
                       of_table_features_cbk_fn table_features_cbk, 
                       void *cbk_arg1,
                       void *cbk_arg2);
#endif
