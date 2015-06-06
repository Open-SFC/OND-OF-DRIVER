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
 * File name: ucmsll.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This file contains the APIs which are used by
 *              applications for their single linked list operations.
 * 
 */

#ifndef _SLL_C
#define _SLL_C

#include "cmincld.h"

/**************************************************************************/
/*                 FUNCTION PROTOTYPE DEFINITIONS                         */
/**************************************************************************/

/***************************************************************************
 * Function Name : cm_sllq_init_list
 * Description   : Initializes the singly linked list
 * Input         : pSll - singly linked list
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cm_sllq_init_list (UCMSllQ_t * pSll)
{
#ifdef DSLIB_DBG
  (pSll)->Head.next_p = &(pSll)->Head;
  (pSll)->pTail = &(pSll)->Head;
#else
  (pSll)->Head.next_p = NULL;
  (pSll)->pTail = NULL;
#endif
  (pSll)->ulCount = 0;
  return;
}

/***************************************************************************
 * Function Name : cm_sllq_append_node
 * Description   : Appends a given node to singly linked list
 * Input         : pSll - singly linked list
 *                 pSllNode - node to be appended
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cm_sllq_append_node (UCMSllQ_t * pSll, UCMSllQNode_t * pSllNode)
{
  if (pSll == NULL)
  {
    return;
  }
#ifdef DSLIB_DBG
  cm_sllq_insert_node (pSll, pSll->pTail, pSllNode, &(pSll)->Head);
#else
  cm_sllq_insert_node (pSll, pSll->pTail, pSllNode, NULL);
#endif
  return;
}

/***************************************************************************
 * Function Name : cm_sllq_prepend_node
 * Description   : Prependes a given node to singly linked list
 * Input         : pSll - singly linked list
 *                 pSllNode - node to be prepended
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cm_sllq_prepend_node (UCMSllQ_t * pSll, UCMSllQNode_t * pSllNode)
{
  if (pSll == NULL)
  {
    return;
  }
#ifdef DSLIB_DBG
  cm_sllq_insert_node (pSll, &(pSll)->Head, pSllNode, (pSll)->Head.next_p);
#else
  cm_sllq_insert_node (pSll, NULL, pSllNode, (pSll)->Head.next_p);
#endif
  return;
}

/***************************************************************************
 * Function Name : cm_sllq_insert_node
 * Description   : Inserts a given node in between two nodes
 * Input         : pSll - singly linked list
 *                 pPrevNode - previous node
 *                 pSllNode - node to be inserted
 *                 next_node_p - next node
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cm_sllq_insert_node (UCMSllQ_t * pSll, UCMSllQNode_t * pPrevNode,
                          UCMSllQNode_t * pSllNode, UCMSllQNode_t * next_node_p)
{
  if (pSll == NULL)
  {
    return;
  }

  if (pPrevNode == NULL)
  {
    pPrevNode = &pSll->Head;
  }

#ifdef DSLIB_DBG
  if (next_node_p == NULL)
  {
    next_node_p = &pSll->Head;
  }
#endif

  pSllNode->next_p = next_node_p;
  pPrevNode->next_p = pSllNode;
#ifdef DSLIB
  if (next_node_p == &pSll->Head)
#else
  if (next_node_p == NULL)
#endif
  {
    /* last node of the list */
    pSll->pTail = pSllNode;
  }
  pSll->ulCount++;
  return;
}

/***************************************************************************
 * Function Name : cm_sllq_delete_node
 * Description   : Deletes a given node from singly linked list
 * Input         : pSll - singly linked list
 *                 pSllNode - node to be deleted
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cm_sllq_delete_node (UCMSllQ_t * pSll, UCMSllQNode_t * pSllNode)
{
  UCMSllQNode_t *pPtr = NULL;
  UCMSllQNode_t *pPrev = NULL;

  if ((pSll == NULL) || (CM_SLLQ_COUNT (pSll) == 0))
  {
    return;
  }
  pPtr = (pSll)->Head.next_p;
  while (pPtr)
  {
    if (pPtr == pSllNode)
    {
      /* found the node to be deleted */
      if (pPrev == NULL)
      {
        /* previous is null i.e this is the first node */
        (pSll)->Head.next_p = pSllNode->next_p;
        if (pSllNode == (pSll)->pTail)
        {
          /* this is the last node also, i.e only one node is present */
#ifdef DSLIB_DBG
          (pSll)->pTail = &pSll->Head;
#else
          (pSll)->pTail = NULL;
#endif
        }
      }
      else
      {
        /* the node is not the first node */
        if (pSllNode == (pSll)->pTail)
        {
          /* it is the last node ?? */
#ifdef DSLIB_DBG
          pPrev->next_p = &pSll->Head;
#else
          pPrev->next_p = NULL;
#endif
          (pSll)->pTail = pPrev;
        }
        else
        {
          /* the node is not either first/last, somewhere in the middle */
          pPrev->next_p = pSllNode->next_p;
        }
      }
      pSllNode->next_p = NULL;
      (pSll)->ulCount--;
      return;
    }
    pPrev = pPtr;
    pPtr = pPtr->next_p;
  }
  return;
}

/***************************************************************************
 * Function Name : cm_sllq_delete_node_in_middle
 * Description   : Deletes a given node from singly linked list
 * Input         : pSll - singly linked list
 *                 pPrev - previous node
 *                 pNode - node to be deleted
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cm_sllq_delete_node_in_middle (UCMSllQ_t * pSll, UCMSllQNode_t * pPrev,
                                  UCMSllQNode_t * pNode)
{

  if ((pSll == NULL) || (CM_SLLQ_COUNT (pSll) == 0))
  {
    return;
  }

  if ((pPrev == NULL) || (pPrev->next_p != pNode))
  {
    /* there is no previous ptr, hence scan and delete */
    cm_sllq_delete_node (pSll, pNode);
    return;
  }

  if (pNode == (pSll)->pTail)
  {
    /* it is the last node ?? */
#ifdef DSLIB_DBG
    pPrev->next_p = &pSll->Head;
#else
    pPrev->next_p = NULL;
#endif
    (pSll)->pTail = pPrev;
  }
  else
  {
    /* the node is not either first/last, somewhere in the middle */
    pPrev->next_p = pNode->next_p;
  }
  pNode->next_p = NULL;
  (pSll)->ulCount--;
  return;
}

UCMSllQNode_t *cm_sllq_pop_node (UCMSllQ_t * pSll)
{
  UCMSllQNode_t *pNode;
  pNode = (UCMSllQNode_t *) CM_SLLQ_FIRST (pSll);
  if (pNode != NULL)
  {
    CM_SLLQ_DELETE_NODE (pSll, pNode);
  }
  return pNode;
}

#endif /* _SLL_C */
