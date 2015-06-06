/* @(#) DLL 1.0  18/10/04  */
/**************************************************************************
 * Copyright 2011-2012, Freescale Semiconductor, Inc. All rights reserved.
 ***************************************************************************/
/*
 * File:	dll.c
 *
 * Description:
 *  This file contains the APIs which are used by applications for their
 *  doubly linked list operations
 *
 * Authors:	Madabhushi Rajesh Kumar <B38870@freescale.com>
 * Modifier:    Rajendran A S <rajendran.as@freescale.com>
 *
 */
/*
 * History
 * 21 Sep 2011 - Rajendran A S  - Took the header file from POC and modified
 *                                the data types.
 *
 */
/******************************************************************************/

#ifndef _DLL_C
#define _DLL_C

#include "controller.h"
#include "dll.h"

/**************************************************************************/
/*                 FUNCTION PROTOTYPE DEFINITIONS                         */
/**************************************************************************/

/***************************************************************************
 * Function Name : dll_queue_init_list
 * Description   : Initializes the doubly linked list
 * Input         : pDll - doubly linked list
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void  dll_queue_init_list (struct dll_queue *pDll)
{
#ifdef CONFIG_PSP_DSLIB_DBG
  (pDll)->Head.pNext = &(pDll)->Head;
  (pDll)->Head.pPrev = &(pDll)->Head;
#else
  (pDll)->Head.pNext = NULL;
  (pDll)->Head.pPrev = NULL;
#endif
  (pDll)->ulCount = 0;
  return;
}
#ifdef CONFIG_PSP_DSLIB_RCU_SUPPORT
/***************************************************************************
 * Function Name : dll_queue_insert_node_at_list_position_rcu
 * Description   : inserts the node at the specified location
 * Input         : pDll - doubly linked list
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/

int32_t dll_queue_insert_node_at_list_position_rcu(struct dll_queue *pDll, struct dll_queue_node *pNode,
                                   uint32_t uiListPos_p)
{
  uint32_t        iCount=0, uiEleCnt;
  struct dll_queue_node  *pTempDllq = NULL;

  if ((pDll == NULL) || (pNode == NULL) || (uiListPos_p == 0))
  {
    return PSP_FAILURE;
  }

  uiEleCnt = PSP_DLLQ_COUNT(pDll);
  if((uiEleCnt+1) < uiListPos_p)
  {
    return PSP_FAILURE;
  }

  /* if the node is either beginning or end of the list, call appropriate
   * function.
   */
  if (uiListPos_p == 1)
  {
/*    DLL_QUEUE_PREPEND_NODE(pDll,pNode);*/
    DLL_QUEUE_PREPEND_NODE_RCU(pDll,pNode);
    return PSP_SUCCESS;
  }
  else if ((uiEleCnt+1) == uiListPos_p)
  {
/*    DLL_QUEUE_APPEND_NODE(pDll,pNode);*/
    DLL_QUEUE_APPEND_NODE_RCU(pDll,pNode);
    return PSP_SUCCESS;
  }

  for(pTempDllq = RCU_DE_REF(pDll->Head.pNext); pTempDllq !=NULL;
      pTempDllq = RCU_DE_REF(pTempDllq->pNext))
  {
    iCount++;
    if(iCount == (uiListPos_p-1))
    {
      /* position found */
      break;
    }
  }

  if (pTempDllq) /* to eliminate coverity error FORWARD_NULL (pTempDllq) */
  {
    dll_queue_insert_node_in_middle_rcu(pDll,pTempDllq,pNode,pTempDllq->pNext);
  }
  return PSP_SUCCESS;
}
#endif

/***************************************************************************
 * Function Name : dll_queue_insert_node_at_list_position
 * Description   : Initializes the doubly linked list
 * Input         : pDll - doubly linked list
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
int32_t dll_queue_insert_node_at_list_position(struct dll_queue *pDll, struct dll_queue_node *pNode,
                                   uint32_t uiListPos_p)
{
  uint32_t        iCount=0, uiEleCnt;
  struct dll_queue_node  *pTempDllq = NULL;

  if ((pDll == NULL) || (pNode == NULL) || (uiListPos_p == 0))
  {
    return PSP_FAILURE;
  }

  uiEleCnt = PSP_DLLQ_COUNT(pDll);
  if((uiEleCnt+1) < uiListPos_p)
  {
    return PSP_FAILURE;
  }

  /* if the node is either beginning or end of the list, call appropriate
   * function.
   */
  if (uiListPos_p == 1)
  {
    DLL_QUEUE_PREPEND_NODE(pDll,pNode);
    return PSP_SUCCESS;
  }
  else if ((uiEleCnt+1) == uiListPos_p)
  {
    DLL_QUEUE_APPEND_NODE(pDll,pNode);
    return PSP_SUCCESS;
  }

  for(pTempDllq = pDll->Head.pNext; pTempDllq !=NULL;
      pTempDllq = pTempDllq->pNext)
  {
    iCount++;
    if(iCount == (uiListPos_p-1))
    {
      /* position found */
      break;
    }
  }

  if (pTempDllq) /* to eliminate coverity error FORWARD_NULL (pTempDllq) */
  {
    dll_queue_insert_in_middle(pDll,pTempDllq,pNode,pTempDllq->pNext);
  }
  return PSP_SUCCESS;
}
#ifdef CONFIG_PSP_DSLIB_RCU_SUPPORT
/***************************************************************************
 * Function Name : dll_queue_insert_node_in_middle_rcu
 * Description   : Deletes a given node from the doubly linked list
 * Input         : pDll - doubly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 *                 pNext - next node
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void  dll_queue_insert_node_in_middle_rcu (struct dll_queue *pDll, struct dll_queue_node *pPrev,
                               struct dll_queue_node *pNode, struct dll_queue_node *pNext)
{
  if ((pDll == NULL) || (pNode == NULL))
  {
    return;
  }

  pDll->ulCount++;
  pNode->pNext = pNext;
  pNode->pPrev = pPrev;

  if (pPrev == NULL)
  {
    pPrev = &(pDll)->Head;
  }
  /* Mark as stale case so add rcu_assign_pointer here --LSR */
//  pPrev->pNext = pNode;
  CNTLR_RCU_ASSIGN_POINTER(pPrev->pNext, pNode);
  if (pNext == NULL)
  {
    pNext = &(pDll)->Head;
  }
//  pNext->pPrev = pNode;
  CNTLR_RCU_ASSIGN_POINTER(pNext->pPrev, pNode);
  return;
}
#endif

/***************************************************************************
 * Function Name : dll_queue_insert_in_middle
 * Description   : Deletes a given node from the doubly linked list
 * Input         : pDll - doubly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 *                 pNext - next node
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void  dll_queue_insert_in_middle (struct dll_queue *pDll, struct dll_queue_node *pPrev,
                               struct dll_queue_node *pNode, struct dll_queue_node *pNext)
{
  if ((pDll == NULL) || (pNode == NULL))
  {
    return;
  }

  pDll->ulCount++;
  pNode->pNext = pNext;
  pNode->pPrev = pPrev;

  if (pPrev == NULL)
  {
    pPrev = &(pDll)->Head;
  }
  pPrev->pNext = pNode;
  if (pNext == NULL)
  {
    pNext = &(pDll)->Head;
  }
  pNext->pPrev = pNode;
  return;
}
// VQA_OS_EXPORT_SYMBOL(dll_queue_insert_in_middle);

/***************************************************************************
 * Function Name : dll_queue_delete_in_middle_rcu
 * Description   : Deletes a given node from the doubly linked list
 * Input         : pDll - doubly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 *                 pNext - next node
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void dll_queue_delete_in_middle_rcu (struct dll_queue * pDll, struct dll_queue_node * pPrev,
                             struct dll_queue_node * pNode, struct dll_queue_node * pNext)
{
  uchar8_t bPrevNULL = FALSE;

#ifdef CONFIG_PSP_DSLIB_DBG
  pPrev->pNext = pNext;
  pNext->pPrev = pPrev;
#else
  if (pPrev == NULL)
  {
    /*
      check if the next pointer is null,
      if it is null,
      then there is only one node in list
         or node was already deleted,
      if the node was already deleted, just return
     */
    if ((!pNext) && ((pDll)->Head.pNext != pNode))
    {
      return;
    }
    pPrev = &(pDll)->Head;
    bPrevNULL = TRUE;
  }
  /*pPrev->pNext = pNext;*/
  CNTLR_RCU_ASSIGN_POINTER(pPrev->pNext, pNext);
  if (pNext == NULL)
  {
    pNext = &(pDll)->Head;
  }
  /*pNext->pPrev = (bPrevNULL == TRUE) ? NULL : pPrev;*/
  if(bPrevNULL == TRUE)
    CNTLR_RCU_ASSIGN_POINTER(pNext->pPrev, NULL);
  else
    CNTLR_RCU_ASSIGN_POINTER(pNext->pPrev, pPrev);
#endif
  /*DLL_QUEUE_INIT_NODE (pNode);*/
  /* Do not set next & prev to NULL for RCU sake (:+
  pNode->pPrev = NULL; */

  (pDll)->ulCount--;

    return;
}

/***************************************************************************
 * Function Name : dll_queue_delete_in_middle
 * Description   : Deletes a given node from the doubly linked list
 * Input         : pDll - doubly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 *                 pNext - next node
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void dll_queue_delete_in_middle (struct dll_queue * pDll, struct dll_queue_node * pPrev,
                             struct dll_queue_node * pNode, struct dll_queue_node * pNext)
{
  uchar8_t bPrevNULL = FALSE;

#ifdef CONFIG_PSP_DSLIB_DBG
  pPrev->pNext = pNext;
  pNext->pPrev = pPrev;
#else
  if (pPrev == NULL)
  {
    /*
      check if the next pointer is null,
      if it is null,
      then there is only one node in list
         or node was already deleted,
      if the node was already deleted, just return
     */
    if ((!pNext) && ((pDll)->Head.pNext != pNode))
    {
      return;
    }
    pPrev = &(pDll)->Head;
    bPrevNULL = TRUE;
  }
  pPrev->pNext = pNext;
  if (pNext == NULL)
  {
    pNext = &(pDll)->Head;
  }
  pNext->pPrev = (bPrevNULL == TRUE) ? NULL : pPrev;
#endif
  DLL_QUEUE_INIT_NODE (pNode);

  (pDll)->ulCount--;

    return;
}
/* This routine is implemnted as needed by Traffic Anomaly Module.
   reference tags : AD_ECO_SYSTEM_LOCK_SUPPORT
 */
/***************************************************************************
 * Function Name : dll_queue_prepend_list
 * Description   : Prepends the NewList to the existing old list.
 * Input         : pNewListHead - contains new list that need to be prepended
 *                 pOldListHead - contains old list to which new list need to
                   be prepended
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void dll_queue_prepend_list(struct dll_queue* pNewListHead, struct dll_queue* pOldListHead)
{
   if(PSP_DLLQ_COUNT(pNewListHead) == 0)
   {
      return;
   }

#ifdef CONFIG_PSP_DSLIB_DBG
   /* FIXME : This need to be implemented */
#else
   if(PSP_DLLQ_COUNT(pOldListHead) == 0)
   {
      pOldListHead->Head.pNext = pNewListHead->Head.pNext;
      pOldListHead->Head.pPrev = pNewListHead->Head.pPrev;
      pOldListHead->ulCount = pNewListHead->ulCount;
   }
   else
   {
      pOldListHead->Head.pNext->pPrev = pNewListHead->Head.pPrev;
      pNewListHead->Head.pPrev->pNext = pOldListHead->Head.pNext;
      pOldListHead->Head.pNext = pNewListHead->Head.pNext;
   }

   pOldListHead->ulCount += pNewListHead->ulCount;
   DLL_QUEUE_INIT_LIST(pNewListHead);
#endif

}

//VQA_OS_EXPORT_SYMBOL(dll_queue_delete_in_middle);
//VQA_OS_EXPORT_SYMBOL(dll_queue_init_list);

#endif /* _DLL_C */
