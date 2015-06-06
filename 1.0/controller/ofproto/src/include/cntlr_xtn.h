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

/* File  : cntlr_xtn.h
 * Author: Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Date  : Feb, 2012  
 * Description: 
 *  This file contains  data_structure and other related definitions 
 *  transactions created in the controller.
 * History:
 */

#ifndef __OF_TRNSCTN_H
#define __OF_TRNSCTN_H

#define OF_TRANSACTION_TABLE_MAX_NO_OF_NODES  5000
#define OF_TRANSACTION_TABLE_SIZE             OF_TRANSACTION_TABLE_MAX_NO_OF_NODES/5

#define OF_TRANSACTION_TABLE_STATIC_NODES     (OF_TRANSACTION_TABLE_MAX_NO_OF_NODES*4)/5

#define CNTLR_XTN_NODE_TIMEOUT  5000000
#define CNTLR_XTN_TABLE_CLEAN_UP_TIME_PERIOD  5000
#define CNTLR_XTN_REC_TIEMOUT                 50000

#define OF_TRANSACTION_TABLE_HASH(Xid,DPIdHi,DPIdLo,hash)   \
{\
 OF_DOBBS_WORDS3 (Xid,DPIdHi,DPIdLo,   \
                       cntlr_xtn_hash_table_init_value_g, \
                       OF_TRANSACTION_TABLE_SIZE, hash);\
}

typedef struct mchash_noderef cntlr_xtn_node_saferef_t;

/* Following data_structure holds the transactions maintained by
 * controller. 
 *
 * These transactions are created while sending message to data_path. 
 * Created transactions are pushed to transaction table.
 *
 * The transactions records are released from the table after receiving all the
 * reponses from the data_path.
 *
 * In case no reponse is received in time, transaction record will be timedout
 */
typedef struct cntlr_transactn_rec_s
{
   struct mchash_table_link       xtn_tbl_link; /* Link to next transaction node*/
   cntlr_xtn_node_saferef_t       rec_safe_ref;
   uint32_t                       xid; /** Transaction ID assigned for the record*/
   uint64_t                       dp_handle; /** Datpath handle to which the message sent*/
   struct of_msg                 *msg; /** Actual message that sent to data_path*/
   enum ofp_type                  msg_type; /** Type of message, here it is Open flow message type*/
   enum ofp_multipart_types       msg_subtype; /**stroing the multipart subtype info **/
   struct timeval                 time_stamp; /** Transaction time stamp, re-stamp for every reponse received*/
   void                          *callback_fn; /** Callback function pointer to pass response data*/
   void                          *app_pvt_arg1; /** First application maintained private information, that will be
                                                    passed to either callback to receive response or error callback 
                                                    registered in case of error repsonse received from data_path*/
   void                          *app_pvt_arg2; /** Second application maintained private information.*/
   uint8_t                        is_heap; /** Boolean flag used by memory pool lib, it is to know whether memory
                                               allocated from heap or not, it need to pass it as paramter to API 
                                               to release memeory*/
struct tmr_saferef  timer_saferef;
}cntlr_transactn_rec_t;

#define TRANSACTION_TBL_LINK_OFFSET offsetof(cntlr_transactn_rec_t,xtn_tbl_link)

typedef struct mchash_table cntlr_xtn_table_t;

int32_t
cntlr_record_xtn_details(struct of_msg               *msg,
                         struct dprm_datapath_entry  *datapath,
                         void                        *callback_fn,
                         void                        *app_arg1,
                         void                        *app_arg2,
                         cntlr_xtn_node_saferef_t    *p_xtn_rec_saferef);
int32_t
cntlr_get_xtn_details(uint32_t           xid_p,
                      of_data_path_id_t  dp_id,
                      cntlr_transactn_rec_t **p_p_transaction_rec);

int32_t cntlr_del_xtn_with_rec(cntlr_transactn_rec_t *transaction_rec);

int32_t
cntlr_del_xtn_rec(uint32_t        xid,
                  of_data_path_id_t *dp_id);
inline int32_t
cntlr_del_xtn_with_safe_ref(cntlr_xtn_node_saferef_t *p_xtn_rec_saferef);

int32_t
cntlr_xtn_table_init();
#endif /*__OF_TRNSCTN_H*/
