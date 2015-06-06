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
 * File name: tkletgrp.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: Contains tasklet group structures, macros and prototypes.
 * 
 */

#ifndef _TKLETGRP_H_
#define _TKLETGRP_H_

#ifdef CM_TASKLET_DEBUG
#define TkletPrint(x) printk x
#else
#define TkletPrint(x) 
#endif

#define CM_MAX_TKLET_DATA_BUF_PER_CPU 50

struct TaskletHandler_s;
typedef void (*TaskletFunction) (struct TaskletHandler_s*);
typedef void (*TaskletCleanUpFunction) (struct TaskletHandler_s*);

struct cm_tasklet_data
{
   struct cm_tasklet_data  *next_p;
   void                *pAppliData;
   unsigned char               heap_bAllocated;
}struct cm_tasklet_data;

struct TaskletHandler_s
{
  struct tasklet_struct    Tasklet;
  struct cm_lock                Lock;
  struct cm_tasklet_data         *pHead;
  struct cm_tasklet_data         *pTail;
};
#define TaskletHandler_t struct TaskletHandler_s

#define CM_TASKLET_KILLED   1<<0
struct tklet_hdlr_list_node
{
	 UCMSllQNode_t            Next;
	 TaskletFunction          pTaskletFunc;
	 TaskletCleanUpFunction   pTaskletCleanUp;
	 TaskletHandler_t *pTaskletGrp;
	 uint8_t                  ucFlags;
};


void* UCMTaskletGrpCreate(TaskletFunction pTaskletFunc);
int32_t UCMTaskletGrpDelete(void* pTaskletHandler);
int32_t UCMTaskletGrpSchedule (void* pTaskletHandler, struct cm_tasklet_data *pData);
void UCMTaskletGrpInit(void);
int32_t UCMTaskletGrpFreeNode(struct cm_tasklet_data *pNode);
struct cm_tasklet_data*  UCMTaskletGrpCreateNode(void);
int32_t UCMTaskletGrpDeInit(int32_t iCnt,unsigned char bForce);
int32_t UCMTaskletGrpDisableAllTklets(void);
int32_t UCMTaskletGrpRegCleanUpFn(void* pTaskletHandler,TaskletCleanUpFunction pTaskletCleanUp);

#endif /*_TKLETGRP_H_*/
