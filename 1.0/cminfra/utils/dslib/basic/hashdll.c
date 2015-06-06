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
 * File name: hashdll.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: This file contains the APIs which are used by applications for their
 *              Hash Table doubly linked list operations.
 * 
 */

#ifndef _HASHDLL_C
#define _HASHDLL_C

#include "cmincld.h"
#include "hashdll.h"

#define DSLIB_ID

#define TRACE_SEVERE
#ifdef __KERNEL__
#define printf printk
#endif
/**************************************************************************/
/*                 FUNCTION PROTOTYPE DEFINITIONS                         */
/**************************************************************************/

/***************************************************************************
 * Function Name : cm_hash_dll_table_create
 * Description   : Creates hash table and initializes the doubly linked list
 *                 buckets
 * Input         : ulBuckets - number of buckets
 * Output        : None.
 * Return value  : Hash Table pointer/ NULL
 ***************************************************************************/
struct cm_hash_dll_table *cm_hash_dll_table_create (uint32_t ulBuckets)
{
  struct cm_hash_dll_table    *pHashTab = NULL;
  uint32_t              ulMemsize = 0;
  uint32_t              ulKey = 0;

  /* calculate the memory required */
  ulMemsize = sizeof (struct cm_hash_dll_table) +
              ((ulBuckets -1) * sizeof (UCMHashDllBucketList_t));
  
  pHashTab = (struct cm_hash_dll_table *) of_malloc (ulMemsize);
  if (pHashTab == NULL)
  {
/*    Trace (DSLIB_ID, TRACE_SEVERE,
      "cm_hash_dll_table_create: Unable to allocate memory for hashtable\n\r");*/
    return NULL;
  }
  pHashTab->ulBuckets = ulBuckets;
  /* initialize the bucket lists */
  for (ulKey = 0; ulKey < pHashTab->ulBuckets; ulKey++)
  {
    CM_HASH_DLL_BUCKET_INIT (pHashTab, ulKey);
  }
  return pHashTab;
}

/***************************************************************************
 * Function Name : cm_hash_dll_table_delete
 * Description   : Deletes hash table
 * Input         : pHashTab - hash table pointer
 * Output        : None.
 * Return value  : None.
 ***************************************************************************/
void  cm_hash_dll_table_delete (struct cm_hash_dll_table *pHashTab)
{
  /* release the memory 
   * assumption - before calling this function, application would have
   * deleted all its pointers
   */
  of_free (pHashTab);
/*  Trace (DSLIB_ID, TRACE_SEVERE,
    "cm_hash_dll_table_delete: Hashtable Deleted Successfully...\n\r");*/
  return;
}

#endif /* _HASHDLL_C */
