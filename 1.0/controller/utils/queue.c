
/**
 *  Copyright (c) 2012, 2013  Freescale.
 *   
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 **/
/*
 * File:        of_queue.c
 * 
 * Description: Open Flow message queue rourintes 
 *
 * Authors:     Rajesh Madabushi <rajesh.madabushi@freescale.com>
 * Modifier:    
 */
/*
 *  History
 *  October 2012 - Rajesh Madabushi - Initial Implementation.
 */

/* INCLUDE_START ************************************************************/
#include "controller.h"
#include "cntrl_queue.h"
#include "oflog.h"
/* INCLUDE_END **************************************************************/

/* GLOBAL_START *************************************************************/
uint32_t  of_msgListMempoolId = 1;
/* GLOBAL_END   *************************************************************/

/* DEFINE_START  ************************************************************/
//#define of_debug printf
/* DEFINE_END    ************************************************************/

/* Function Prototype_start *************************************************/
/* Function Prototype_end   *************************************************/

/*Maximum number of list items present in the queue 'ulNoOfListNodesInQ' need to pass as input paramter*/
of_msg_q_box_t* of_MsgQBoxCreate (uint32_t ulNoOfListNodesInQ, uint32_t num_queues)
{
   of_msg_q_box_t   *pMsgQBox;
   int32_t         iDynSockHandle;
   uint16_t        usDynLoopbackPort;
   uint32_t        ulMaxMsgLists;
   uint32_t        ulMaxStaticMsgLists;
   char            PoolName[32];
   int32_t         ii;
   int32_t         lRetVal = OF_SUCCESS;
   struct mempool_params mempool_info={};

   //if ((num_queues < 1) || (num_queues > CNTLR_NO_OF_CORES_IN_SYSTEM)) {
   if (num_queues < 1){
      OF_LOG_MSG (OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d Invalid num_queues %d\r\n",
		__FUNCTION__, __LINE__, num_queues);
      return NULL;
   }
   pMsgQBox = (of_msg_q_box_t *) of_mem_calloc(1,sizeof(of_msg_q_box_t));
   if(pMsgQBox == NULL)
   {
      OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d Failed to Allocate Memory \r\n",__FUNCTION__,__LINE__);
      return NULL;
   }

   /* Create loopback socket, bind to a port  to receive msg q events */
   pMsgQBox->usLoopbackPort  = 0;
   pMsgQBox->iLoopbackSockFd = 0;
   lRetVal = of_get_ephermal_port(OF_IPPROTO_UDP,
                                OF_LOOPBACK_SOCKET_IPADDR,
                                &iDynSockHandle,
                                &usDynLoopbackPort);
   if( lRetVal != OF_SUCCESS )
   {
      OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d error 0x%x in getting an ephermal loopback port\r\n",
                     __FUNCTION__,__LINE__,
                      lRetVal);
      of_mem_free(pMsgQBox);
      return NULL;
   }
   pMsgQBox->usLoopbackPort  = usDynLoopbackPort;
   pMsgQBox->iLoopbackSockFd = iDynSockHandle;
   pMsgQBox->bSendEvent      = TRUE;

   OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"channel loopback port %d sock fd %d" ,  pMsgQBox->usLoopbackPort, pMsgQBox->iLoopbackSockFd);
   pMsgQBox->MsgQueue = (of_MsgQueue_t *) of_mem_calloc(num_queues,
                                                       sizeof(of_MsgQueue_t) );
   if(pMsgQBox->MsgQueue == NULL)
   {
      OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d Failed to Allocate Memory \r\n",__FUNCTION__,__LINE__);
      close(pMsgQBox->iLoopbackSockFd);
      of_mem_free(pMsgQBox);
      return NULL;
   }
   for (ii =0 ; ii < num_queues; ii++ )
   {
	   pMsgQBox->MsgQueue[ii].ulWriteIndx=-1;
	   pMsgQBox->MsgQueue[ii].ulReadIndx=-1;
   }
   ulMaxMsgLists       = ulNoOfListNodesInQ * num_queues; 
   ulMaxStaticMsgLists = ulMaxMsgLists / 2;    /* 50%*/

   /*Create memory pool queue lists , TBD need to tune paramters passed in the call*/
   of_msgListMempoolId++;
   if(of_msgListMempoolId == 0) 
      of_msgListMempoolId = 1;
   sprintf(PoolName,"%s%d","msg_list_",of_msgListMempoolId);

   mempool_info.mempool_type = MEMPOOL_TYPE_HEAP;  
   mempool_info.block_size = sizeof(of_MsgEntry_t);
   mempool_info.max_blocks = ulMaxMsgLists;
   mempool_info.static_blocks = ulMaxStaticMsgLists;
   mempool_info.threshold_min = ulMaxStaticMsgLists/10;
   mempool_info.threshold_max = ulMaxStaticMsgLists/3;
   mempool_info.replenish_count = ulMaxStaticMsgLists/10;
   mempool_info.b_memset_not_required =  FALSE;
   mempool_info.b_list_per_thread_required =  TRUE;
   mempool_info.noof_blocks_to_move_to_thread_list = 0;

   lRetVal = mempool_create_pool(PoolName, &mempool_info,
		   &pMsgQBox->pListMempool) ;
   if(lRetVal != MEMPOOL_SUCCESS)
    {
       OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d Create mempool failure MSG QBox mem pools, retval=%d \r\n",
                                                       __FUNCTION__,__LINE__,lRetVal);
       
       close(pMsgQBox->iLoopbackSockFd);
       of_mem_free(pMsgQBox->MsgQueue);
       of_mem_free(pMsgQBox);
      return NULL;
    }

    for(ii=0; ii < num_queues;ii++ )
    {
        CNTLR_LOCK_INIT( pMsgQBox->MsgQueue[ii].Lock );
    }

    return pMsgQBox;
}

/*TBD of following functionality*/
void
of_MsgQBoxDelete(of_msg_q_box_t  *pMsgQBox)
{
    cntlr_assert(pMsgQBox);

    mempool_delete_pool(pMsgQBox->pListMempool);
    close(pMsgQBox->iLoopbackSockFd);
    of_mem_free(pMsgQBox->MsgQueue);
    of_mem_free(pMsgQBox);
}

int32_t
of_MsgEnqueue(of_msg_q_box_t  *pMsgQBox,
              uint32_t       uiQIndex,
              void        *pMsg,
              uint8_t    try_lock_less_b)
{
   of_MsgQueue_t  *pMsgQ;
   of_MsgEntry_t  *pMsgEntry;
   of_MsgList_t   *pMsgQList;
   uint32_t        ulNextWriteIndex;
   uint8_t         bHeap;

   pMsgQ            = &pMsgQBox->MsgQueue[uiQIndex];
   ulNextWriteIndex = pMsgQ->ulWriteIndx+1;
   //OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d  Write Index = %d  read index =%d \r\n",__FUNCTION__,__LINE__,ulNextWriteIndex,pMsgQ->ulReadIndx);

   if (!try_lock_less_b)
   	CNTLR_LOCK_TAKE(pMsgQ->Lock);

   if ( ulNextWriteIndex >= (OF_MAX_MSGS_IN_RING))
   {  
	   if ( pMsgQ->ulReadIndx > (ulNextWriteIndex % (OF_MAX_MSGS_IN_RING)))
	   {
		   ulNextWriteIndex %=(OF_MAX_MSGS_IN_RING);
	   }
   }

   if( ulNextWriteIndex != pMsgQ->ulReadIndx  && ( ulNextWriteIndex < (OF_MAX_MSGS_IN_RING)) )
   {
	   pMsgQ->pMsgData[ulNextWriteIndex] = pMsg;
	   pMsgQ->ulWriteIndx = ulNextWriteIndex;
//	   OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d  Added Msg at Write Index = %d \r\n",__FUNCTION__,__LINE__,ulNextWriteIndex);
	   if (!try_lock_less_b)
		CNTLR_LOCK_RELEASE (pMsgQ->Lock);
	   return OF_SUCCESS;
   }
   /*Static array used for messages are full, queue the message to linked list*/
   if(mempool_get_mem_block(pMsgQBox->pListMempool,
                         (uint8_t **)&pMsgEntry,
                         (uint8_t *)&bHeap) != OF_SUCCESS)
   {
     // OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d Message Entry memory alloc failure\r\n", __FUNCTION__,__LINE__);
      if (!try_lock_less_b)
          CNTLR_LOCK_RELEASE (pMsgQ->Lock);
      return OF_FAILURE;
   }

   pMsgEntry->bHeap    = bHeap;
   pMsgEntry->pMsgData = pMsg;
   pMsgEntry->pNext    = NULL;

   pMsgQList = &(pMsgQ->MsgList);

   if (try_lock_less_b)
   	CNTLR_LOCK_TAKE(pMsgQ->Lock);

   if(pMsgQList->pTail == NULL)
   {
      pMsgQList->pHead = pMsgEntry;
   }
   else
   {
      pMsgQList->pTail->pNext = pMsgEntry;
   }
   pMsgQList->pTail = pMsgEntry;
   pMsgQList->uiMsgListCnt++;

   CNTLR_LOCK_RELEASE(pMsgQ->Lock);

   return OF_SUCCESS;
}

/* Return value OF_FAILURE means Queue is empy*/
int32_t
of_MsgDequeue(of_msg_q_box_t  *pMsgQBox,
              uint32_t       uiQIndex, 
              void           **pMsg,
              uint8_t        try_lock_less_b)
{
  of_MsgQueue_t  *pMsgQ;
  of_MsgList_t   *pMsgQList;
  of_MsgEntry_t  *pTmpMsgEntry;

  pMsgQ = &pMsgQBox->MsgQueue[uiQIndex];

 // OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d  write index %d and read index %d  \r\n",__FUNCTION__,__LINE__,pMsgQ->ulWriteIndx,pMsgQ->ulReadIndx );
  // check if ring is empty
  if(( pMsgQ->ulWriteIndx < OF_MAX_MSGS_IN_RING) && ( pMsgQ->ulWriteIndx == pMsgQ->ulReadIndx))
  {
	  return OF_FAILURE;
  }
  if( pMsgQ->ulWriteIndx < OF_MAX_MSGS_IN_RING)
  {
    if (!try_lock_less_b)
        CNTLR_LOCK_TAKE(pMsgQ->Lock);

    pMsgQ->ulReadIndx++;
    pMsgQ->ulReadIndx %= (OF_MAX_MSGS_IN_RING );
   // OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d  Read Index = %d \r\n",__FUNCTION__,__LINE__,pMsgQ->ulReadIndx);
    *pMsg  =  pMsgQ->pMsgData[pMsgQ->ulReadIndx];
    pMsgQ->pMsgData[pMsgQ->ulReadIndx] = NULL;

    if (!try_lock_less_b)
        CNTLR_LOCK_RELEASE(pMsgQ->Lock);
  }
  else
  {
    //OF_LOG_MSG(OF_LOG_UTIL, OF_LOG_ERROR,"%s:%d  write index and read index are same \r\n",__FUNCTION__,__LINE__);
    CNTLR_LOCK_TAKE(pMsgQ->Lock);
    pMsgQList = &(pMsgQ->MsgList);
    if(pMsgQList->uiMsgListCnt == 0)
       return OF_FAILURE;
    pTmpMsgEntry = pMsgQList->pHead;
    *pMsg = pMsgQList->pHead->pMsgData;
    pMsgQList->pHead = pMsgQList->pHead->pNext;
    if (pMsgQList->pHead == NULL)
	pMsgQList->pTail = NULL;
    mempool_release_mem_block (pMsgQBox->pListMempool,(unsigned char *)pTmpMsgEntry,pTmpMsgEntry->bHeap);
    pMsgQList->uiMsgListCnt--;
    CNTLR_LOCK_RELEASE(pMsgQ->Lock);
  }
  return OF_SUCCESS;
}
