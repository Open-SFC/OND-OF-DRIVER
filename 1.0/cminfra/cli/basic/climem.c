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
 * File name:  climem.c
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: 
*/

#ifdef CM_CLIENG_SUPPORT
#include "cliincld.h"
#include "cmsock.h"
#include "clildef.h"
#include "clilif.h"

void *data_elements_mempool_g = NULL;

#define CM_CLI_MAX_DATAELE_NODES               100
#define CM_CLI_MAX_STATIC_DATAELE_NODES        (CM_CLI_MAX_DATAELE_NODES/2)
#define CM_CLI_MAX_DATAELE_NODE_THRESHOLD_CNT  \
                                      (CM_CLI_MAX_STATIC_DATAELE_NODES/3)

#define CM_CLI_MIN_DATAELE_NODE_THRESHOLD_CNT  \
                                      (CM_CLI_MAX_STATIC_DATAELE_NODES/6)

#define CM_CLI_DATAELE_NODE_REPLINISH_CNT  \
                                      (CM_CLI_MAX_STATIC_DATAELE_NODES/10)
/******************************************************************************
 * Function Name : cm_cli_mem_init
 * Description   : This function is used to Initialize memory pools for CLI data
 *               : element nodes.
 * Input params  : NONE
 * Output params : NONE
 * Return value  : NONE
 *****************************************************************************/
void cm_cli_mem_init (void)
{
  cm_cli_create_data_element_node_mempool ();
}

/******************************************************************************
 * Function Name :
 * Input params  :
 * Output params :
 * Description   :
 * Return value  :
 *****************************************************************************/
void cm_cli_create_data_element_node_mempool (void)
{
  int32_t return_value;

  /* static memory pool for Configuration Cache  Nodes */
  if (CM_CLI_MAX_STATIC_DATAELE_NODES > 0)
  {
    data_elements_mempool_g =
      cm_create_mempool (CM_CLI_MAX_STATIC_DATAELE_NODES,
                        sizeof (struct cm_cli_data_ele_node),
                        CM_CLI_MAX_DATAELE_NODES, TRUE);
    if (data_elements_mempool_g == NULL)
    {
      fprintf (stderr,
                 "UCMCLICreateDataEleNodeMemPool :: cm_create_mempool failed\r\n");
      return;                   /* Exit the task that is calling this function */
    }
    return_value = cm_mempool_set_threshold_vals (data_elements_mempool_g,
                                          CM_CLI_MIN_DATAELE_NODE_THRESHOLD_CNT,
                                          CM_CLI_MAX_DATAELE_NODE_THRESHOLD_CNT,
                                          CM_CLI_DATAELE_NODE_REPLINISH_CNT);
    if (return_value != OF_SUCCESS)
    {
      fprintf (stderr,
                 "%s :: cm_mempool_set_threshold_vals failed\r\n", __FUNCTION__);
      cm_delete_mempool (data_elements_mempool_g);
      return;
    }
  }

  return;
}

/****************************************************************************
 * FunctionName  : UCMCLIDataEleNodeAlloc
 * Description   : This function is used to allocate memory block for a Configuration Cache 
 *                  node from preallocated memory pool. If no buffers 
 *                 exists in the Pool will allocate memory from Heap.
 * Input         : NONE
 * Output        : NONE
 * Return value  : on success returns Allocate Memory block address
 *                 NULL - on failure
 ***************************************************************************/
void *cm_cli_data_element_node_alloc (void)
{
  struct cm_cli_data_ele_node *data_element_node_p = NULL;
  unsigned char heap_b = TRUE;
  int16_t return_value;

  /* Always we will set the heap_b value to TRUE.
   * While calling cm_mempool_get_block() , this heap_b will act as Input and
   * Output parameter. If memory block is allocated from the Pool, it's value
   * will be set to FALSE. Otherwise, if memory is exhausted in thr Pool,
   * based on heap_b value(TRUE) memory will be allocated from Heap.*/

  /* Memory allocation failed */
  return_value = cm_mempool_get_block (data_elements_mempool_g,
                                (unsigned char **) & data_element_node_p, &heap_b);

  /* Memory allocation failed */
  if (data_element_node_p == NULL || return_value != OF_SUCCESS)
  {
    UCMCLIDEBUG ("cm_mempool_get_block Failed for policy");
    return (NULL);
  }
  data_element_node_p->heap_b = heap_b;
  return (data_element_node_p);
}

/****************************************************************************
 * FunctionName  : UCMCLIDataEleNodeFree
 * Description   : Used to release the memory block allocated for a Configuration Cache
 *                  node.
 * Input         : pFree -- points to the memory block
 *                 heap_b -- specifies whether the memory for the block is
 *                          allocated from Preallocated memory Pool (OR) from
 *                          Heap.
 * Output        : NONE
 * Return value  : NONE
 ***************************************************************************/
void cm_cli_data_element_node_free (struct cm_cli_data_ele_node * free_p)
{
  if (cm_mempool_release_block (data_elements_mempool_g, (unsigned char *) free_p,
                              free_p->heap_b) != OF_SUCCESS)
  {
    UCMCLIDEBUG ("%s :: cm_mempool_release_block Failed for CLIDataElement\
       node\n\r", __FUNCTION__);
    return;
  }

  return;
}

#endif /*CM_CLI_SUPPORT */
