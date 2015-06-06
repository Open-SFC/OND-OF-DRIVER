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
 * File name: memuport.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This file contains the APIs which are used by applications for their
 *              memory management.
 * 
 */

#ifndef _MEMUPORT_C
#define _MEMUPORT_C

#include "cmincld.h"
#include "cmmempol.h"
#include "memds.h"

/***************************************************************************
 * Function Name : cm_mempool_set_threshold_vals
 * Description   : Sets the controllable values for a memory pool
 * Input         : pPool - memory pool for which values to be set
 *                 ulMinThreshold - mimimum threshold value
 *                 ulMaxThreshold - maximum threshold value
 *                 ulReplenishCnt - replenished node count
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_set_threshold_vals (void * pPool,
                                    uint32_t ulMinThreshold,
                                    uint32_t ulMaxThreshold,
                                    uint32_t ulReplenishCnt)
{
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_wakeup_kernel_thread
 * Description   : Wakes up kernel thread for memory chunk creation
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_wakeup_kernel_thread (UCMMemPool_t * pMemPool)
{
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_derive_threshold_vals
 * Description   : Derives the threshold values based on ulBlocksCnt
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_derive_threshold_vals (UCMMemPool_t * pMemPool)
{
  pMemPool->MemPoolStats.ulMinThreshold = 0;
  pMemPool->MemPoolStats.ulMaxThreshold = pMemPool->MemPoolStats.ulBlocksCnt;
  pMemPool->MemPoolStats.ulReplenishCnt = 0;
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_lock_init
 * Description   : Initializes Semaphore
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_lock_init (UCMMemPool_t * pMemPool)
{
  if ((pMemPool->MemPoolLock.MemPoolULock = cm_mutex_init (0)) == 0)
  {
    return (OF_FAILURE);
  }
  pMemPool->bLock = TRUE;
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_locktake
 * Description   : Takes Semaphore
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_locktake (UCMMemPool_t * pMemPool)
{
  if (pMemPool->bLock == TRUE)
  {
    /* block if sem is not available */
    cm_mutex_lock (pMemPool->MemPoolLock.MemPoolULock, T_SEM_WAIT);
  }
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_lock_give
 * Description   : Takes Semaphore
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_lock_give (UCMMemPool_t * pMemPool)
{
  if (pMemPool->bLock == TRUE)
  {
    cm_mutex_unlock (pMemPool->MemPoolLock.MemPoolULock);
  }
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_lock_del
 * Description   : Destroys Semaphore
 * Input         : pPool - memory pool pointer
 * Output        : None
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_mempool_lock_del (UCMMemPool_t * pMemPool)
{
  if (pMemPool->bLock == TRUE)
  {
    cm_mutex_destroy (pMemPool->MemPoolLock.MemPoolULock);
  }
  return (OF_SUCCESS);
}

/***************************************************************************
 * Function Name : cm_mempool_get_max_cpus
 * Description   : 
 * Input         : 
 * Output        : 
 * Return value  : 
 ***************************************************************************/
int32_t cm_mempool_get_max_cpus (void)
{
  return 1;
}

/***************************************************************************
 * Function Name : cm_mempool_get_cur_cpu
 * Description   : 
 * Input         :
 * Output        : 
 * Return value  : 
 ***************************************************************************/
int32_t cm_mempool_get_cur_cpu (void)
{
  return 0;
}
#endif /* _MEMUPORT_C */
