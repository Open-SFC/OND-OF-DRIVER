enum
{
  CM_STATS_REQ_TO_COALESCAR = 100,
  CM_STATS_REQ_TO_RESPONDER,
  CM_STATS_RESP_FROM_RESPONDER
};

struct cm_stats_req_msg
{
  uint32_t command_id;
  uint32_t uiSubCmnd;
  char *dm_path_p;
  struct cm_role_info role_info;
  struct cm_tnsprt_channel MgmtToJEChannel;

};

struct cm_stats_rsp_msg
{
  int32_t  command_id;
  int32_t  result_code;
  int32_t  count_ui;
  struct cm_array_of_nv_pair *pnv_pair_array;
};


void UcmStatsGetReqMsgLen (struct cm_stats_req_msg *pStatsReqMsg, uint32_t *msg_len_p);

int32_t UcmFrameStatsReq ( struct cm_stats_req_msg *pStatsReqMsg , char  *pBuf,  uint32_t uiMsgLen);

int32_t UcmStatsSndRqToCoalescar( struct cm_tnsprt_channel *pStatsChannel,
                                  struct cm_command *command_p,
                                  struct cm_role_info *role_info_p,
                                  struct cm_tnsprt_channel *pCurrentChannel);

struct cm_device_info
{ 
  uint64_t device_ID;
  unsigned char priority; 
  unsigned char device_role;
  void *NeighDownTimer;
  struct igw_ip_addr Mgmt_IP;
  uint64_t Master_ID;
  uint64_t runtime_cfg_ver_num;
  uint64_t saved_cfg_ver_num;
  struct UCMDeviceInfo_s *next_p;
};
