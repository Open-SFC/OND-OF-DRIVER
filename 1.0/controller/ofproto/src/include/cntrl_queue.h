/**************************************************************************
 *   Copyright 2011-2012, Freescale Semiconductor, Inc. All rights reserved.
 ***************************************************************************/
/*
 *  File:        of_queue.h
 * 
 * Description: Message Equeue routine defintions 
 *
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Modifier:    
 * 
 *
 */
/*
 *  History
 *  27 August 2012 - Rajesh Madabushi - Initial Implementation.
 * 
 ****************************************************************************/
#ifndef __OF_QUEUE_H
#define __OF_QUEUE_H

/*DEFINE_START  *************************************************************/
#define OF_MAX_MSGS_IN_RING (64)
/*DEFINE_END    *************************************************************/

typedef struct of_MsgEntry_s
{
  struct of_MsgEntry_s *pNext;
  void               *pMsgData;
  uint8_t              bHeap;
}of_MsgEntry_t;

typedef struct of_MsgList_s
{
  of_MsgEntry_t  *pHead;
  of_MsgEntry_t  *pTail;
  uint32_t        uiMsgListCnt;
}of_MsgList_t;

typedef struct of_MsgQueue_s
{
  int32_t        ulWriteIndx;
  int32_t        ulReadIndx;
  void           *pMsgData[OF_MAX_MSGS_IN_RING];
  cntlr_lock_t    Lock;
  of_MsgList_t    MsgList;
}of_MsgQueue_t;

typedef struct of_msg_q_box_s
{
   of_MsgQueue_t   *MsgQueue;
   void          *pListMempool;
   int32_t        iLoopbackSockFd;
   uint16_t        usLoopbackPort;
   uint8_t	    bSendEvent; /*Boolean flag indicate whether to send event to channel queue or not*/
} of_msg_q_box_t;

/* FUNCTION_PROTOTYP_START  *****************************************************/
of_msg_q_box_t* of_MsgQBoxCreate (uint32_t ulNoOfListNodesInQ, uint32_t num_queues);
void of_msg_q_box_delete(of_msg_q_box_t  *pMsgQ);
int32_t of_msg_enqueue(of_msg_q_box_t  *msg_q_box, uint32_t q_index, void *msg);
int32_t of_msg_dequeue(of_msg_q_box_t  *msg_q_box, uint32_t q_index, void **msg);
void  of_MsgQBoxDelete(of_msg_q_box_t  *pMsgQBox);
int32_t  of_MsgEnqueue(of_msg_q_box_t  *pMsgQBox, uint32_t uiQIndex, void *pMsg, uint8_t try_lock_less_b);
int32_t of_MsgDequeue(of_msg_q_box_t  *pMsgQBox, uint32_t  uiQIndex, void  **pMsg, uint8_t try_lock_less_b);
/* FUNCTION_PROTOTYP_END  *******************************************************/

#endif /*__OF_QUEUE_H*/
