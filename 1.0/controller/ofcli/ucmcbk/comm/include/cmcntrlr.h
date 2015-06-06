
#ifndef CM_CNTRLR_H
#define CM_CNTRLR_H

#define  CM_CNTRL_MAX_REQMSG_LEN (2*1024)
#define  CM_CNTRL_MAX_RESPMSG_LEN (2*1024)
#define CM_CNTRL_MAX_MULTIPART_RESP_LEN (100*1024)


//#define cntrlrucm_debugmsg(format,args...) printf("\r\n%s(%d)::"format,__FUNCTION__,__LINE__,##args)
#define cntrlrucm_debugmsg(format,args...)

#define CM_CNTRL_CONFIG_REQUEST 1
#define CM_CNTRL_CONFIG_RESPONSE 2
#define CM_CNTRL_MULTIPART_RESPONSE 3
#define CM_CNTRL_ASYNC_RESPONSE 4
#define CM_CNTRL_MULTIPART_REPLY_MORE 5


typedef struct ucmcntrolmsg_s
{
	 uint32_t length;
	 uint32_t app_id;
	 uint32_t cmd_id;
	 uint32_t sub_cmd_id;
	 uint32_t trans_len;
	 uint32_t  result_code;
	 uint32_t  error_code;
	 char multi_part;
	 char msg_type; /* request or response */
	 char pad[2];
	 char msg[CM_CNTRL_MAX_REQMSG_LEN];
}ucmcntrlmsg_t;

typedef struct ucmcommmsg_s
{
	 ucmcntrlmsg_t cntlreqmsg;
	 ucmcntrlmsg_t cntlrespmsg;
	 uint32_t msglen;
	 char *pmultipart; /* It points to multipart buffer only, when response type is OFPT_MULTIPART_REPLY */
}ucmcommmsg_t;

#if 0
typedef struct ucmCntrlrMsg_s
{
	 uint32_t length;
	 uint32_t app_id;
	 char cmd_id;
	 char sub_cmd_id;
	 char pads[6];
	 char msg[CM_CNTRL_MAX_REQMSG_LEN];
}ucmcntrlreqmsg_t;
typedef  struct UCMIGWCmnCntxtInfo_s
{ 
	 uint32_t length;        /*command string length*/
	 uint32_t  result_code;
	 uint32_t  trans_len;
//	 void  *xtn_entry_pId;
	 char msg_type;
	 char pads[2];
	 char msg[CM_CNTRL_MAX_RESPMSG_LEN];     /*pointer to Command string*/
}  ucmcntrlrespmsg_t;
#endif


#define CM_CNTRL_PRINT_PACKET(c, len) \
{\
 uint32_t i; \
 for (i=0; i< len; i++) \
{\
	 if ( i % 16 == 0)\
			printf("\n");\
	 printf(" %02x", *(c+i));\
}\
}



#define CM_CNTRL_PREPARE_CNTRL_CFG_REQUEST(UCMRequest, uiAppId, uiCmdId, sub_command_idId, uiTransLen, MsgLen)\
{\
	 UCMRequest.msg[MsgLen] = '\0';\
	 UCMRequest.app_id=uiAppId;\
	 UCMRequest.cmd_id=uiCmdId;\
	 UCMRequest.sub_cmd_id=sub_command_idId; \
	 UCMRequest.length = MsgLen; \
	 UCMRequest.trans_len = uiTransLen;\
	 CM_COMM_DEBUG_PRINT("msg length=%d app_id =%d cmdid=%d subcmdid=%d translen=%d",MsgLen, UCMRequest.app_id,UCMRequest.cmd_id,UCMRequest.sub_cmd_id, UCMRequest.trans_len);\
}
int32_t UCMCntrlCommHandler (ucmcommmsg_t *pCommMsg, struct cm_app_result *app_result_p);
#endif /* CM_CNTRLR_H */
