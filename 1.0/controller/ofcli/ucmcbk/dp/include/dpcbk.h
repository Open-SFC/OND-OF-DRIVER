
#ifndef CM_DPCBK_H
#define CM_DPCBK_H

#define CM_CBK_DEBUG
#ifdef CM_CBK_DEBUG
#define CM_CBK_DEBUG_PRINT printf("\n%35s(%4d): ",__FUNCTION__,__LINE__),\
	                          printf 
#else
#define CM_CBK_DEBUG_PRINT(format,args...)
#endif

void *UCMDP_BeginConfigTransaction(uint32_t app_id,struct cm_array_of_iv_pairs * arr_p,
			uint32_t cmd_id,
			struct cm_app_result ** result_p);

int32_t UCMDP_EndConfigTransaction(uint32_t app_id,void * config_trans_p,
			uint32_t cmd_id
			,struct cm_app_result ** result_p);
int32_t UCMDP_AddRecord(uint32_t app_id,
			void * config_trans_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * member_arr_p,
			struct cm_app_result ** result_p);

int32_t UCMDP_SetRecord(uint32_t app_id,void * config_trans_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * member_arr_p,
			struct cm_app_result ** result_p);

int32_t UCMDP_DelRecord(uint32_t app_id,void * config_transaction_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p);


/* Information retrieval Callback functions */
int32_t UCMDP_GetFirstNRecs(uint32_t app_id,struct cm_array_of_iv_pairs *
			keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs **
			first_n_record_data_p);
int32_t UCMDP_GetNextNRecs(uint32_t app_id,struct cm_array_of_iv_pairs *
			keys_arr_p,
			struct cm_array_of_iv_pairs *
			prev_record_key_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs **
			next_n_record_data_p);
int32_t UCMDP_GetExactRec(uint32_t app_id,struct cm_array_of_iv_pairs *
			keys_arr_p,
			struct cm_array_of_iv_pairs **
			record_data_p);

int32_t UCMDP_VerifyConfig(uint32_t app_id,struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t cmd_id,
			struct cm_app_result ** result_p);


#endif /* CM_CNTRLR_H */
