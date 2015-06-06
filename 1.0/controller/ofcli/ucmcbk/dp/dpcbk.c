#ifdef OF_CM_DP_CHANNEL_SUPPORT


#include "cbcmninc.h"
#include "dpcbk.h"
#include "openflow.h"
#include "cmcntrlr.h"
#include "cmcomm.h"


#define CPDPCOMM_CM_APPID      IGWCPDP_DP_UCM
#define CPDPCOMM_CM_SUBAPPID  IGWCPDP_DP_UCM

void *dp_xtn_rec_p="ABC";


void *UCMDP_BeginConfigTransaction(uint32_t uiAppId,struct cm_array_of_iv_pairs * arr_p,
			uint32_t uiCmdId,
			struct cm_app_result ** result_p)
{
	 char *pTempBuf;;
	 int32_t iTotalLen=0,return_value,iError;
	 uint32_t uiLen=0;
	 void *pt=NULL;
	 struct cm_app_result *app_result_p=NULL;
	 ucmcommmsg_t CommMsg={};


	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT("Entry uiAppId=%lu",uiAppId);

	 if(cm_je_copy_ivpair_arr_to_buffer(arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return NULL;
	 }
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;


	 app_result_p = (struct cm_app_result *)of_calloc(1,sizeof(struct cm_app_result));
	 if(!app_result_p)
	 {
			CM_CBK_DEBUG_PRINT("memory allocation failed");
			return NULL;
	 }
	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, uiCmdId, CM_CPDP_APPL_TRANS_BEGIN_CMD, 0,iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,app_result_p);
	 if(app_result_p->result_code==OF_FAILURE)
	 {
			*result_p = app_result_p ;
			if ((return_value==CM_NO_APPL_TRANS_SUPPORT) || (return_value == CM_APPL_NULL_CALLBACK))
			{
				 CM_CBK_DEBUG_PRINT("No transaction support. Sending temp trans");
				 return dp_xtn_rec_p;
			}
			if (return_value == CM_APPL_START_TRANS_FAILED)
			{
				 CM_CBK_DEBUG_PRINT("transaction start failed");
			}
			return NULL;
	 }

	 CM_CBK_DEBUG_PRINT("transaction exists");
	 pTempBuf=CommMsg.cntlrespmsg.msg;
#if defined(_LP64)
	 pt = (void *)of_mbuf_get_64(pTempBuf);
#else
	 pt=(void *)of_mbuf_get_32(pTempBuf);
#endif
	 pTempBuf+=sizeof(pt);
	 of_free(app_result_p);
	 return pt;
}

int32_t UCMDP_EndConfigTransaction(uint32_t uiAppId,void * config_trans_p,uint32_t uiCmdId,struct cm_app_result ** result_p)
{
	 char *pTempBuf;;
	 struct cm_app_result *app_result_p=NULL;
	 int32_t iTotalLen=0,return_value;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT(" Entry uiAppId=%ld",uiAppId);

#if defined(_LP64)
	 pTempBuf= of_mbuf_put_64(pTempBuf, config_trans_p);
#else
	 pTempBuf= of_mbuf_put_32(pTempBuf, config_trans_p);
#endif
	 iTotalLen+=sizeof(long);

	 app_result_p = (struct cm_app_result *)of_calloc(1,sizeof(struct cm_app_result));
	 if(!app_result_p)
	 {
			CM_CBK_DEBUG_PRINT("memory allocation failed");
			return OF_FAILURE;
	 }

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, uiCmdId, CM_CPDP_APPL_TRANS_END_CMD, sizeof(long), iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,app_result_p);

	 if(app_result_p->result_code==OF_FAILURE)
	 {
			if ( (return_value == CM_APPL_NULL_CALLBACK))
			{
				 CM_CBK_DEBUG_PRINT("No transaction support. Sending temp trans");
				 return OF_SUCCESS;
			}
			if (return_value == CM_APPL_END_TRANS_FAILED)
			{
				 *result_p = app_result_p ;
				 CM_CBK_DEBUG_PRINT("transaction start failed");
			}
			return OF_FAILURE;
	 }
	 of_free(app_result_p);
	 return OF_SUCCESS;
}

int32_t UCMDP_AddRecord(uint32_t uiAppId ,void * config_trans_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * member_arr_p,
			struct cm_app_result ** result_p)
{
	 char *pTempBuf;;
	 int32_t iTotalLen=0,return_value;
	 uint32_t uiLen=0;
	 struct cm_app_result *app_result_p=NULL;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT(" Entry uiAppId=%ld",uiAppId);
#if defined(_LP64)
	 pTempBuf= of_mbuf_put_64(pTempBuf, config_trans_p);
#else
	 pTempBuf= of_mbuf_put_32(pTempBuf, config_trans_p);
#endif
	 iTotalLen+=sizeof(long);
	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 } 
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;
	 uiLen=0;

	 if(cm_je_copy_ivpair_arr_to_buffer(member_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 iTotalLen+=uiLen;

	 app_result_p = (struct cm_app_result *)of_calloc(1,sizeof(struct cm_app_result));
	 if(!app_result_p)
	 {
			CM_CBK_DEBUG_PRINT("memory allocation failed");
			return OF_FAILURE;
	 }

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, CM_CPDP_APPL_ADD_CMD, sizeof(long), iTotalLen);
	 return_value=UCMCntrlCommHandler( &CommMsg,app_result_p);

	 if(return_value == OF_SUCCESS)
	 {
			of_free(app_result_p);
			return OF_SUCCESS;
	 }

	 *result_p = app_result_p;
	 return return_value;
}

int32_t UCMDP_SetRecord(uint32_t uiAppId,void * config_trans_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_array_of_iv_pairs * member_arr_p,
			struct cm_app_result ** result_p)
{
	 char *pTempBuf;;
	 int32_t iTotalLen=0,return_value;
	 uint32_t uiLen=0;
	 struct cm_app_result *app_result_p=NULL;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT(" Entry uiAppId=%ld",uiAppId);

#if defined(_LP64)
	 pTempBuf= of_mbuf_put_64(pTempBuf, config_trans_p);
#else
	 pTempBuf= of_mbuf_put_32(pTempBuf, config_trans_p);
#endif
	 iTotalLen+=sizeof(long);
	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 } 
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;
	 uiLen=0;

	 if(cm_je_copy_ivpair_arr_to_buffer(member_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }

	 app_result_p = (struct cm_app_result *)of_calloc(1,sizeof(struct cm_app_result));
	 if(!app_result_p)
	 {
			CM_CBK_DEBUG_PRINT("memory allocation failed");
			return OF_FAILURE;
	 }  

	 iTotalLen+=uiLen;
	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, CM_CPDP_APPL_SET_CMD, sizeof(long), iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,app_result_p);

	 if(return_value == OF_SUCCESS)
	 {
			of_free(app_result_p);
			return OF_SUCCESS;
	 }

	 *result_p = app_result_p;
	 return return_value;
}

int32_t UCMDP_DelRecord(uint32_t uiAppId,void * config_trans_p,
			struct cm_array_of_iv_pairs * keys_arr_p,
			struct cm_app_result ** result_p)
{
	 char *pTempBuf;;
	 int32_t iTotalLen=0,return_value;
	 uint32_t uiLen=0;
	 struct cm_app_result *app_result_p=NULL;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT(" Entry uiAppId=%ld",uiAppId);


#if defined(_LP64)
	 pTempBuf= of_mbuf_put_64(pTempBuf, config_trans_p);
#else
	 pTempBuf= of_mbuf_put_32(pTempBuf, config_trans_p);
#endif
	 iTotalLen+=sizeof(long);
	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;

	 app_result_p = (struct cm_app_result *)of_calloc(1,sizeof(struct cm_app_result));
	 if(!app_result_p)
	 {
			CM_CBK_DEBUG_PRINT("memory allocation failed");
			return OF_FAILURE;
	 }  

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, CM_CPDP_APPL_DEL_CMD, sizeof(long), iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,app_result_p);

	 if(return_value == OF_SUCCESS)
	 {
			of_free(app_result_p);
			return OF_SUCCESS;
	 }

	 *result_p = app_result_p;
	 return return_value;
}


int32_t UCMDP_GetFirstNRecs(uint32_t uiAppId,struct cm_array_of_iv_pairs *
			keys_arr_p,
			uint32_t * count_p,
			struct cm_array_of_iv_pairs **first_n_record_data_p)
{
	 char *pRespBuf,*pTempBuf;
	 char aResponseBuf[CM_CNTRL_MAX_MULTIPART_RESP_LEN];
	 int32_t iTotalLen=0,return_value=0,ii=0;
	 uint32_t uiLen=0;
	 struct cm_array_of_iv_pairs *pRecArr;
	 char *pMultipartBuf;
	 uint32_t uiRecCnt=0, buf_len_ui=0, uiTmpLen=0, uiTotalRecs=0;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT("Entry uiAppId=%ld",uiAppId);

	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;

	 pTempBuf= of_mbuf_put_32 (pTempBuf, *count_p);
	 iTotalLen+=4;

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, 
				 CM_CPDP_APPL_GET_FIRSTN_CMD, 0, iTotalLen);

	 return_value=UCMCntrlCommHandler(&CommMsg,NULL);
	 if(return_value==OF_FAILURE)
			return OF_FAILURE;

	 if (CommMsg.cntlrespmsg.msg_type == CM_CNTRL_MULTIPART_RESPONSE)
	 {
			pRespBuf=CommMsg.pmultipart;
			iTotalLen=CommMsg.msglen;
	 }
	 else
	 {
			pRespBuf=&CommMsg.cntlrespmsg.msg[0];
			iTotalLen=CommMsg.cntlrespmsg.length;
	 }
	 CM_CBK_DEBUG_PRINT("Total Len %d",iTotalLen);

	 pTempBuf=pRespBuf;
	 buf_len_ui=0;
	 while(buf_len_ui < iTotalLen )
	 {
			uiRecCnt+=of_mbuf_get_32(pTempBuf);
			CM_CBK_DEBUG_PRINT(" Records %d", uiRecCnt);
			pTempBuf+=4;
			buf_len_ui+=4;
			uiTmpLen=0;
			cm_je_get_ivpair_array_len_from_buffer(pTempBuf,&uiTmpLen);	
			pTempBuf +=uiTmpLen;
			buf_len_ui += uiTmpLen;
	 }

	 buf_len_ui=0;
	 pTempBuf=pRespBuf;
	 *count_p=uiRecCnt;
	 CM_CBK_DEBUG_PRINT("Total Number of Records %d", *count_p);
	 pRecArr=(struct cm_array_of_iv_pairs *)of_calloc(*count_p,sizeof(struct cm_array_of_iv_pairs));
	 if(pRecArr==NULL)
	 {
			CM_CBK_DEBUG_PRINT("Memory allocation failed");
			return OF_FAILURE;
	 }
	 
	 for(ii=0;ii<*count_p;ii++)
	 {
			pTempBuf+=4; /* Rec Count field.. we have already read */ 
			buf_len_ui+=4; /* Rec Count field... we have already read */
			uiLen=0;
			if(cm_je_copy_ivpair_arr_from_buffer(pTempBuf,&pRecArr[ii],&uiLen)!=OF_SUCCESS)
			{
				 CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
				 return OF_FAILURE;
			}
			pTempBuf+=uiLen;
			buf_len_ui += uiLen;
	 }
	 *first_n_record_data_p=pRecArr;
	
	 if (CommMsg.cntlrespmsg.msg_type == CM_CNTRL_MULTIPART_RESPONSE)
			of_free(CommMsg.pmultipart);

	 return OF_SUCCESS;
}

int32_t UCMDP_GetNextNRecs(uint32_t uiAppId,struct cm_array_of_iv_pairs *
			keys_arr_p,	struct cm_array_of_iv_pairs *	prev_record_key_p,
			uint32_t * count_p,			struct cm_array_of_iv_pairs **next_n_record_data_p)
{
	 char *pTempBuf;;
	 int32_t iTotalLen=0,ii=0,return_value=0;
	 uint32_t uiLen=0;
	 struct cm_array_of_iv_pairs *pRecArr;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;
	 CM_CBK_DEBUG_PRINT(" Entry uiAppId=%ld",uiAppId);

	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;
	 uiLen=0;
	 if(cm_je_copy_ivpair_arr_to_buffer(prev_record_key_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_ivpair_to_buffer Failed");
			return OF_FAILURE;
	 }
	 CM_CBK_DEBUG_PRINT("After uiLen = %ld",uiLen);
	 pTempBuf+=uiLen;
	 iTotalLen+=uiLen;

	 pTempBuf= of_mbuf_put_32 (pTempBuf, *count_p);
	 iTotalLen+=4;

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, CM_CPDP_APPL_GET_NEXTN_CMD, 0, iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,NULL);
	 if(return_value==OF_FAILURE)
			return OF_FAILURE;

	 pTempBuf=CommMsg.cntlrespmsg.msg;
	 *count_p=of_mbuf_get_32(pTempBuf);
	 pTempBuf+=4;

	 pRecArr=(struct cm_array_of_iv_pairs *)of_calloc(*count_p,sizeof(struct cm_array_of_iv_pairs));
	 if(pRecArr==NULL)
			return OF_FAILURE;

	 for(ii=0;ii<*count_p;ii++)
	 {
			uiLen=0;
			if(cm_je_copy_ivpair_arr_from_buffer(pTempBuf,&pRecArr[ii],&uiLen)!=OF_SUCCESS)
			{
				 CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
				 return OF_FAILURE;
			}
			pTempBuf+=uiLen;
	 }
	 *next_n_record_data_p=pRecArr;
	 return OF_SUCCESS;
}

int32_t UCMDP_GetExactRec(uint32_t uiAppId,struct cm_array_of_iv_pairs *
			keys_arr_p,		struct cm_array_of_iv_pairs **record_data_pp)
{
	 char *pRespBuf;
	 char *pReqBuf;
	 int32_t iTotalLen=0,return_value=0;
	 uint32_t uiLen=0, count=0;
	 struct cm_array_of_iv_pairs *pRecArr;
	 ucmcommmsg_t CommMsg={};

	 pReqBuf=CommMsg.cntlreqmsg.msg;

	 CM_CBK_DEBUG_PRINT("entry");
	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pReqBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 pReqBuf+=uiLen;
	 iTotalLen+=uiLen;

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, CM_CPDP_APPL_GET_EACT_CMD, 0, iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,NULL);
	 if(return_value==OF_FAILURE)
			return OF_FAILURE;

	 if (CommMsg.cntlrespmsg.msg_type == CM_CNTRL_MULTIPART_RESPONSE)
	 {
		 pRespBuf=CommMsg.pmultipart;
		 iTotalLen=CommMsg.msglen;
		 pRespBuf= of_mbuf_put_32 (pRespBuf, count);
		 iTotalLen+=4;
	 }
	 else
	 {
			pRespBuf=&CommMsg.cntlrespmsg.msg[0];
			iTotalLen=CommMsg.cntlrespmsg.length;
	 }
	 CM_CBK_DEBUG_PRINT("Total Len %d",iTotalLen);

	 pRecArr=(struct cm_array_of_iv_pairs *)of_calloc(1,sizeof(struct cm_array_of_iv_pairs));
	 uiLen=0;
	 if(cm_je_copy_ivpair_arr_from_buffer(pRespBuf,pRecArr,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 pRespBuf+=uiLen;
	 *record_data_pp=pRecArr;

	 if (CommMsg.cntlrespmsg.msg_type == CM_CNTRL_MULTIPART_RESPONSE)
			of_free(CommMsg.pmultipart);

	 return OF_SUCCESS;
}


int32_t UCMDP_VerifyConfig(uint32_t uiAppId,struct cm_array_of_iv_pairs * keys_arr_p,
			uint32_t uiCmdId,
			struct cm_app_result ** result_p)
{
	 char *pTempBuf;;
	 uint32_t uiLen=0;
	 int32_t return_value, iTotalLen=0;
	 struct cm_app_result *app_result_p=NULL;
	 ucmcommmsg_t CommMsg={};

	 pTempBuf=CommMsg.cntlreqmsg.msg;

	 uiLen = 0;
	 if(cm_je_copy_ivpair_arr_to_buffer(keys_arr_p,pTempBuf,&uiLen)!=OF_SUCCESS)
	 {
			CM_CBK_DEBUG_PRINT("cm_je_copy_ivpair_arr_to_buffer Failed");
			return OF_FAILURE;
	 }
	 pTempBuf+=uiLen;
	 iTotalLen += uiLen;

	 app_result_p = (struct cm_app_result *)of_calloc(1,sizeof(struct cm_app_result));
	 if(!app_result_p)
	 {
			CM_CBK_DEBUG_PRINT("memory allocation failed");
			return OF_FAILURE;
	 }

	 CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(CommMsg.cntlreqmsg, uiAppId, 0, CM_CPDP_APPL_TEST_CMD, 0, iTotalLen);
	 return_value=UCMCntrlCommHandler(&CommMsg,app_result_p);

	 if(return_value == OF_SUCCESS)
	 {
			of_free(app_result_p);
			return OF_SUCCESS;
	 }

	 *result_p = app_result_p;
	 return return_value;
}
#endif
