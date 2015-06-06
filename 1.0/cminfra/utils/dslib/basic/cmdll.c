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
 * File name: ucmdll.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This file contains the APIs which are used by
 *              applications for their doubly linked list operations.
 * 
 */

#ifndef _DLL_C
#define _DLL_C

#include "cmincld.h"

/**************************************************************************/
/*                 FUNCTION PROTOTYPE DEFINITIONS                         */
/**************************************************************************/

/***************************************************************************
 * Function Name : cm_dllq_init_list
 * Description   : Initializes the doubly linked list
 * Input         : pDll - doubly linked list
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void cm_dllq_init_list (UCMDllQ_t * pDll)
{
#ifdef DSLIB_DBG
  (pDll)->Head.next_p = &(pDll)->Head;
  (pDll)->Head.pPrev = &(pDll)->Head;
#else
  (pDll)->Head.next_p = NULL;
  (pDll)->Head.pPrev = NULL;
#endif
  (pDll)->ulCount = 0;
  return;
}

/***************************************************************************
 * Function Name : cm_dllq_insert_node_at_list_pos
 * Description   : Initializes the doubly linked list
 * Input         : pDll - doubly linked list
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
int32_t cm_dllq_insert_node_at_list_pos (UCMDllQ_t * pDll, UCMDllQNode_t * pNode,
                                    uint32_t uiListPos_p)
{
  uint32_t count_i = 0, uiEleCnt;
  UCMDllQNode_t *pTempDllq = NULL;

  if ((pDll == NULL) || (pNode == NULL) || (uiListPos_p == 0))
  {
    return OF_FAILURE;
  }

  uiEleCnt = CM_DLLQ_COUNT (pDll);
  if ((uiEleCnt + 1) < uiListPos_p)
  {
    return OF_FAILURE;
  }

  /* if the node is either beginning or end of the list, call appropriate
   * function.
   */
  if (uiListPos_p == 1)
  {
    CM_DLLQ_PREPEND_NODE (pDll, pNode);
    return OF_SUCCESS;
  }
  else if ((uiEleCnt + 1) == uiListPos_p)
  {
    CM_DLLQ_APPEND_NODE (pDll, pNode);
    return OF_SUCCESS;
  }

  for (pTempDllq = pDll->Head.next_p; pTempDllq != NULL;
       pTempDllq = pTempDllq->next_p)
  {
    count_i++;
    if (count_i == (uiListPos_p - 1))
    {
      /* position found */
      break;
    }
  }

  if (pTempDllq)                /* to eliminate coverity error FORWARD_NULL (pTempDllq) */
  {
    cm_dllq_insert_in_middle (pDll, pTempDllq, pNode, pTempDllq->next_p);
  }
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_dllq_insert_in_middle
 * Description   : Deletes a given node from the doubly linked list
 * Input         : pDll - doubly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 *                 next_p - next node
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void cm_dllq_insert_in_middle (UCMDllQ_t * pDll, UCMDllQNode_t * pPrev,
                              UCMDllQNode_t * pNode, UCMDllQNode_t * next_p)
{
  if ((pDll == NULL) || (pNode == NULL))
  {
    return;
  }

  pDll->ulCount++;
  pNode->next_p = next_p;
  pNode->pPrev = pPrev;

  if (pPrev == NULL)
  {
    pPrev = &(pDll)->Head;
  }
  pPrev->next_p = pNode;
  if (next_p == NULL)
  {
    next_p = &(pDll)->Head;
  }
  next_p->pPrev = pNode;
  return;
}

/***************************************************************************
 * Function Name : cm_dllq_delete_in_middle
 * Description   : Deletes a given node from the doubly linked list
 * Input         : pDll - doubly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 *                 next_p - next node
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void cm_dllq_delete_in_middle (UCMDllQ_t * pDll, UCMDllQNode_t * pPrev,
                              UCMDllQNode_t * pNode, UCMDllQNode_t * next_p)
{
  unsigned char bPrevNULL = FALSE;

#ifdef DSLIB_DBG
  pPrev->next_p = next_p;
  next_p->pPrev = pPrev;
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
    if ((!next_p) && ((pDll)->Head.next_p != pNode))
    {
      return;
    }
    pPrev = &(pDll)->Head;
    bPrevNULL = TRUE;
  }
  pPrev->next_p = next_p;
  if (next_p == NULL)
  {
    next_p = &(pDll)->Head;
  }
  next_p->pPrev = (bPrevNULL == TRUE) ? NULL : pPrev;
#endif
  CM_DLLQ_INIT_NODE (pNode);

  (pDll)->ulCount--;

  return;
}

/* This routine is implemnted as needed by Traffic Anomaly Module.
   reference tags : AD_ECO_SYSTEM_LOCK_SUPPORT
 */
/***************************************************************************
 * Function Name : cm_dllq_prepend_list
 * Description   : Prepends the NewList to the existing old list.
 * Input         : pNewListHead - contains new list that need to be prepended
 *                 pOldListHead - contains old list to which new list need to
                   be prepended
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void cm_dllq_prepend_list (UCMDllQ_t * pNewListHead, UCMDllQ_t * pOldListHead)
{
  if (CM_DLLQ_COUNT (pNewListHead) == 0)
  {
    return;
  }

#ifdef DSLIB_DBG
  /* FIXME : This need to be implemented */
#else
  if (CM_DLLQ_COUNT (pOldListHead) == 0)
  {
    pOldListHead->Head.next_p = pNewListHead->Head.next_p;
    pOldListHead->Head.pPrev = pNewListHead->Head.pPrev;
    pOldListHead->ulCount = pNewListHead->ulCount;
  }
  else
  {
    pOldListHead->Head.next_p->pPrev = pNewListHead->Head.pPrev;
    pNewListHead->Head.pPrev->next_p = pOldListHead->Head.next_p;
    pOldListHead->Head.next_p = pNewListHead->Head.next_p;
  }

  pOldListHead->ulCount += pNewListHead->ulCount;
  CM_DLLQ_INIT_LIST (pNewListHead);
#endif

}

#endif /* _DLL_C */
