/****************************************************************************
 *  File                : sll.h
 *  Description         :
 *  This file contains the APIs prototypes and data structure definitions 
 *  which are used by applications for their singly linked list operations
 *  Revision History    :
****************************************************************************/

#ifndef _SLL_H
#define _SLL_H

/***************************************************************************/
/*                  DATA STRUCTURE DEFINITIONS                             */
/***************************************************************************/
struct cntlr_sll_q_node
{
  struct cntlr_sll_q_node *next_p;
};

struct cntlr_sll_q
{
  struct    cntlr_sll_q_node  head;
  struct    cntlr_sll_q_node  *tail_p;
  uint32_t  count;
};

/***************************************************************************/
/*                      FUNCTION PROTOTYPE DECLERATIONS                    */
/***************************************************************************/
void  cntrl_sll_q_init_list(struct cntlr_sll_q *sll_p);
void  cntrl_sll_q_append_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *sll_node_p);
void  cntrl_sll_q_prepend_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *sll_node_p);
void  cntrl_sll_q_delete_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *sll_node_p);
void  cntrl_sll_q_insert_node(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *prev_node_p,
                         struct cntlr_sll_q_node *sll_node_p, struct cntlr_sll_q_node *next_node_p);
void  cntrl_sll_q_delete_node_in_middle(struct cntlr_sll_q *sll_p, struct cntlr_sll_q_node *prev_p, 
                                   struct cntlr_sll_q_node *node_p);
struct cntlr_sll_q_node* cntrl_sll_q_pop_node(struct cntlr_sll_q *sll_p);
/***************************************************************************/
/*                      MACRO DEFINITIONS                                  */
/***************************************************************************/
#define CNTLR_SLLQ_INIT_NODE(node_p) ((node_p)->next_p = NULL)
#define CNTLR_SLLQ_INIT_LIST(sll_p)    cntrl_sll_q_init_list((sll_p))
#define CNTLR_SLLQ_COUNT(sll_p) ((sll_p)->count)

#define CNTLR_SLLQ_FIRST(sll_p) ((sll_p)->head.next_p)
#define CNTLR_SLLQ_LAST(sll_p)  ((sll_p)->tail_p)
#define CNTLR_SLLQ_NEXT(sll_p,node_p) ((node_p)->next_p)
#define CNTLR_SLLQ_IS_NODE_IN_LIST(node_p) 1

#define CNTLR_SLLQ_APPEND_NODE(sll_p, node_p)  cntrl_sll_q_append_node((sll_p), (node_p))
#define CNTLR_SLLQ_PREPEND_NODE(sll_p, node_p) cntrl_sll_q_prepend_node((sll_p), (node_p))
#define CNTLR_SLLQ_INSERT_NODE(sll_p, prev_p, node_p, next_p) \
                     cntrl_sll_q_insert_node((sll_p), (prev_p), (node_p), (next_p))
#define CNTLR_SLLQ_DELETE_NODE(sll_p, node_p) \
        (CNTLR_SLLQ_IS_NODE_IN_LIST ((node_p)) != 0) ? \
        (cntrl_sll_q_delete_node((sll_p), (node_p)), 1) : 0
#define CNTLR_SLLQ_DELETE_NODE_IN_MIDDLE(sll_p, prev_p, node_p) \
        (CNTLR_SLLQ_IS_NODE_IN_LIST ((node_p)) != 0) ? \
        (cntrl_sll_q_delete_node_in_middle((sll_p), (prev_p), (node_p)), 1) : 0
#define CNTLR_SLLQ_SCAN(sll_p, node_p, typecast) \
        for (node_p = (typecast) (CNTLR_SLLQ_FIRST ((sll_p)));\
             node_p != NULL;\
             node_p = \
             (typecast) CNTLR_SLLQ_NEXT ((sll_p), (struct cntlr_sll_q_node *)(node_p)))

#define CNTLR_SLLQ_LIST_MEMBER(node_p, typecast, member) \
      ((typecast)((t_uchar8 *)(node_p)-(t_uint32)(&((typecast)0)->member)))

#endif /* _SLL_H */
