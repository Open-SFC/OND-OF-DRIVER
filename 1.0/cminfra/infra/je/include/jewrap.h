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
 * File name: jewrap.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifndef UCMJE_WRAP_H
#define UCMJE_WRAP_H

/*****************************************************************************
 * * * * * * * * * * *   M A C R O  D E F I N I T I O N S * * * * * * * * * * *
 *****************************************************************************/

#ifdef CM_JETRANS_DEBUG
#define CM_JETRANS_DEBUG_PRINT printf("\n%35s(%4d): ",__FUNCTION__,__LINE__),\
                    printf
#else
#define CM_JETRANS_DEBUG_PRINT(format,args...)
#endif

/*****************************************************************************
 * * * * * * * * *  F U N C T I O N   P R O T O T Y P E S  * * * * * * * * * * 
 *****************************************************************************/
/*JE WRAPPER UTILITIES */
int32_t UCMCopyArrNvPairArrToArrNvPairArr (struct cm_array_of_nv_pair *
                                           arr_in_nv_pair_arr_p,
                                           uint32_t uiArrayCount,
                                           struct cm_array_of_nv_pair *
                                           arr_out_nv_pair_arr_p);

int32_t UCMCopyNvPairArrToNvPairArr (struct cm_array_of_nv_pair * pInNvPairArr,
                                     struct cm_array_of_nv_pair * pOutNvPairArr);

int32_t UCMCopyNvPairToNvPair (struct cm_nv_pair * pInNvPair,
                               struct cm_nv_pair * pOutNvPair);

/*
 * Prototypes for UCM JE transport channel APIs.
 */

void cm_fill_generic_header (uint32_t uiEngineId, uint32_t uiCmdId,
                             struct cm_role_info * role_info_p,
                             struct cm_msg_generic_header * gen_hdr_p);

int32_t cm_fill_request_message (struct cm_request_msg * pRequestMsg,
                               uint32_t sub_command_id,
                               unsigned char * dm_path_p,
                               uint32_t count_ui,
                               struct cm_array_of_nv_pair * pNvPairAry,
                               struct cm_array_of_nv_pair * pPrevRecNvPair, uint32_t * puiLen);

int32_t UCMExtractResponseFromBuff (uint32_t command_id,
                                    char * pRespBuff,
                                    uint32_t * puiReqBufLen,
                                    struct cm_result * resp_msg_p);

int32_t cm_frame_tnsprt_request (struct cm_request_msg * pReqMsg,
                                     unsigned char * pSendMsg);

int32_t UCMJEFreeRequestMsg (struct cm_request_msg * pUCMRequestMsg);
int32_t UCMFreeUCMResult (struct cm_result * pUCMResult);

int32_t UCMCopyNvPairFromBuffer (char * pRequestData,
                                 struct cm_nv_pair * pNvPair, uint32_t * puiLen);
int32_t UCMCopyDMPathFromBuffer (char * pUCMTmpRespBuf,
                                 struct cm_result * cm_resp_msg_p, uint32_t * puiLen);

int32_t UCMCopyResultMessageFromBuffer (char * pUCMTmpRespBuf,
                                        struct cm_result * cm_resp_msg_p,
                                        uint32_t * puiLen);

void UCMCopyOpaqueInfoFromRespBuff (char ** pUCMRespBuff,
                                      uint32_t opaque_info_length,
                                      void ** opaque_info_pp);

void UCMCopyNodeInfoFromRespBuff (char ** pUCMRespBuff,
                                    uint32_t uiNodeCnt,
                                    struct cm_dm_node_info ** node_info_p);
void je_free_commandArray (struct cm_array_of_commands * pCmdArray,
                              uint32_t count_ui);
void cm_je_copy_nvpair_to_buffer(char **tmp_buffer_p, struct cm_nv_pair *pNvPair);

int32_t ucmJECopyErrorInfoFromoBuffer (char * pUCMTempRespBuf,
			struct cm_result * cm_resp_msg_p,
			uint32_t * puiLen);
int32_t UCMGetFirstNLogs (void * tnsprt_channel_p,
                             uint32_t mgmt_engine_id,
                             char * admin_role_p,
                             char * dm_path_p,
                             struct cm_array_of_nv_pair * keys_array_p,
                             uint32_t * pcount_ui,
                             struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p);
int32_t UCMGetNextNLogs (void * tnsprt_channel_p,
                            uint32_t mgmt_engine_id,
                            char * admin_role_p,
                            char * dm_path_p,
                            //struct cm_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * prev_rec_key_p,
                            struct cm_array_of_nv_pair * keys_array_p,
                            uint32_t * pcount_ui,
                            struct cm_array_of_nv_pair ** out_array_of_nv_pair_arr_p);
#endif
