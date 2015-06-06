/******************************************************************************/

#ifndef _SLL_C
#define _SLL_C

#include "controller.h"
#include "sll.h"

/**************************************************************************/
/*                 FUNCTION PROTOTYPE DEFINITIONS                         */
/**************************************************************************/

/***************************************************************************
 * Function Name : cntrl_sll_q_init_list
 * Description   : Initializes the singly linked list
 * Input         : sll_p - singly linked list
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void cntrl_sll_q_init_list(struct cntlr_sll_q *sll_p)
{
  (sll_p)->head.next_p = NULL;
  (sll_p)->tail_p = NULL;
  (sll_p)->count = 0;
  return;
}

/***************************************************************************
 * Function Name : cntrl_sll_q_append_node
 * Description   : Appends a given node to singly linked list
 * Input         : sll_p - singly linked list
 *                 sll_node_p - node to be appended
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void  cntrl_sll_q_append_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *sll_node_p)
{
  if (sll_p == NULL)
    return ;
  cntrl_sll_q_insert_node(sll_p, sll_p->tail_p, sll_node_p, NULL);
  return; 
}
/***************************************************************************
 * Function Name : cntrl_sll_q_prepend_node 
 * Description   : Prependes a given node to singly linked list
 * Input         : sll_p - singly linked list
 *                 sll_node_p - node to be prepended
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void  cntrl_sll_q_prepend_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *sll_node_p)
{
  if (sll_p == NULL)
    return;
  cntrl_sll_q_insert_node(sll_p, NULL, sll_node_p, (sll_p)->head.next_p);
  return;
}
/***************************************************************************
 * Function Name : cntrl_sll_q_insert_node 
 * Description   : Inserts a given node in between two nodes
 * Input         : sll_p - singly linked list
 *                 prev_node_p - previous node
 *                 sll_node_p - node to be inserted
 *                 next_node_p - next node
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void  cntrl_sll_q_insert_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *prev_node_p,
                        struct cntlr_sll_q_node *sll_node_p, struct cntlr_sll_q_node *next_node_p)
{
  if (sll_p == NULL)
    return ;

  if (prev_node_p == NULL)
    prev_node_p = &sll_p->head;

  sll_node_p->next_p = next_node_p;
  prev_node_p->next_p = sll_node_p;
  if (next_node_p == NULL)
  {
    /* last node of the list */
    sll_p->tail_p = sll_node_p;
  }
  sll_p->count++;
  return; 
}
/***************************************************************************
 * Function Name : cntrl_sll_q_delete_node
 * Description   : Deletes a given node from singly linked list
 * Input         : sll_p - singly linked list
 *                 sll_node_p - node to be deleted
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void  cntrl_sll_q_delete_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *sll_node_p)
{
  struct cntlr_sll_q_node *ptr_p = NULL;
  struct cntlr_sll_q_node *prev_p = NULL;

  if ((sll_p == NULL) || (CNTLR_SLLQ_COUNT (sll_p) == 0))
    return;
  ptr_p = (sll_p)->head.next_p;
  while (ptr_p)
  {
    if (ptr_p == sll_node_p)
    {
      /* found the node to be deleted */
      if (prev_p == NULL)
      {
        /* previous is null i.e this is the first node */
        (sll_p)->head.next_p = sll_node_p->next_p;
        if (sll_node_p == (sll_p)->tail_p)
        {
          /* this is the last node also, i.e only one node is present */
          (sll_p)->tail_p = NULL;
        }
      }
      else
      {
        /* the node is not the first node */
        if (sll_node_p == (sll_p)->tail_p)
        {
          /* it is the last node ?? */
          prev_p->next_p = NULL;
          (sll_p)->tail_p = prev_p;
        }
        else
        {
          /* the node is not either first/last, somewhere in the middle */
          prev_p->next_p = sll_node_p->next_p;
        }
      }
      sll_node_p->next_p = NULL;
      (sll_p)->count--;
      return;
    }
    prev_p = ptr_p;
    ptr_p = ptr_p->next_p;
  }
  return;
}
/***************************************************************************
 * Function Name : cntrl_sll_q_delete_node_in_middle
 * Description   : Deletes a given node from singly linked list
 * Input         : sll_p - singly linked list
 *                 prev_p - previous node
 *                 node_p - node to be deleted
 * Output        : None
 * Return value  : None.
 ***************************************************************************/
void  cntrl_sll_q_delete_node_in_middle(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *prev_p, 
                                        struct cntlr_sll_q_node *node_p)
{

  if ((sll_p == NULL) || (CNTLR_SLLQ_COUNT (sll_p) == 0))
    return;

  if ((prev_p == NULL) || (prev_p->next_p != node_p))
  {
    /* there is no previous ptr, hence scan and delete */
    cntrl_sll_q_delete_node(sll_p, node_p);
    return;
  }

  if (node_p == (sll_p)->tail_p)
  {
    /* it is the last node ?? */
    prev_p->next_p = NULL;
    (sll_p)->tail_p = prev_p;
  }
  else
  {
    /* the node is not either first/last, somewhere in the middle */
    prev_p->next_p = node_p->next_p;
  }
  node_p->next_p = NULL;
  (sll_p)->count--;
  return;
}

struct cntlr_sll_q_node* cntrl_sll_q_pop_node(struct cntlr_sll_q *sll_p)
{ 
   struct cntlr_sll_q_node *node_p; 
   node_p = (struct cntlr_sll_q_node*)CNTLR_SLLQ_FIRST(sll_p);
   if(node_p != NULL) 
   {
    CNTLR_SLLQ_DELETE_NODE(sll_p, node_p);
   }
   return node_p; 
}
#endif /* _SLL_C */
