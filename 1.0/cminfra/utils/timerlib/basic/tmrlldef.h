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
 * File name: tmrlldef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: Timer library, provides the timer management          
 *              functionality to process/tasks/thread. It is a thread 
 *              safe library. This header file contains the local    
 *              definitions of this library.
 * 
 */

#ifndef _CM_LIBTMR_LDEF_H_
#define _CM_LIBTMR_LDEF_H_

/*
 * structure identifying the library instance context.
 */
typedef  struct UCMLibTimerCtxt_s
{
#ifdef CM_LIB_TIMER_DEBUG
  uint32_t            pattern1_ui;
#endif
  uint32_t            ulMaxStaticTimers;
  uint32_t            ulMaxTimers;
  UCMDllQ_t           ActiveHead;  /* active list. sorted on expiry values */
  UCMDllQ_t           PassiveHead;  /* passive list. unsorted list */
  uint32_t            ulClkTicksPerSec;
  uint32_t            ulMilliSecsPerClkTick;
  uint32_t            ulPreWaitTimeStamp;
  uint32_t            ulPostWaitTimeStamp;
  uint32_t            ulFlag;
#ifdef CM_LIB_TIMER_DEBUG
  uint32_t            pattern2_ui;
#endif
  void*             pMemPool; /* memory pool for timer nodes */
  UCMLibTimerStats_t  stats;   
}UCMLibTimerCtxt_t;  

typedef  enum
{
  CM_LIB_TIMER_PRE_WAIT_TIME_STAMP  =  (1 << 0),
}UCMLibTimerCtxtFlags_t;

/*
 * structure identifying a timer node
 */
typedef  struct  UCMLibTimer_s
{
  UCMDllQNode_t          PrevNext;  /* used for maintaining double linked list */
#ifdef CM_LIB_TIMER_DEBUG
  uint32_t               pattern1_ui;
#endif
  CM_LIBTIMER_TIMER_HANDLER   pHandler;
  UCMLibTimerUserData_t  UserData;
  uint32_t               ulTimerType;
  uint32_t               ulTimerVal; /* units depends on ulTimerType */
  uint32_t               ulTimerValInClkTicks; 
  uint32_t               ulRemTimerVal; /* units are always clock ticks */
                                     /* valid IFF timers are in active list */
  uint32_t               ulFlags;
}UCMLibTimer_t;

/*
 * enumeration identifying the different values assigned to the "ulFlags" field
 * of the timer node.
 * CM_LIB_TIMER_ACTIVE and CM_LIB_TIMER_PASSIVE are mutually exclusive.
 */
typedef  enum
{
  CM_LIB_TIMER_ACTIVE     = (1 << 0),
  CM_LIB_TIMER_PERIODIC   = (1 << 1),
  CM_LIB_TIMER_HEAP       = (1 << 2),
  CM_LIB_TIMER_PASSIVE    = (1 << 3),
  CM_LIB_TIMER_RUNNING    = (1 << 4),
}UCMLibTimerFlags_t;


typedef  enum
{
  CM_LIB_TIMER_ACTIVE_LIST  = 1,
  CM_LIB_TIMER_PASSIVE_LIST  = 2,
}UCMLibTimerListTypes_t;

/*
 * useful macros
 */
#define  CM_LIB_TIMER_TYPE_VALID_BITS_MASK   (~(CM_LIBTIMER_PERIODIC_TIMER))

#define CM_LIB_TIMER_PATTERN 0xADF8EB9

#ifdef CM_LIB_TIMER_DEBUG                   
#define  CM_LIB_TIMER_INIT_TIMER_CTXT_PATTERN_FIELDS(pTimerCtxt)   \
            {                                                       \
                pTimerCtxt->pattern1_ui = CM_LIB_TIMER_PATTERN;     \
                pTimerCtxt->pattern2_ui = CM_LIB_TIMER_PATTERN;     \
            }

#define  CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) \
            (((pTimerCtxt->pattern1_ui == CM_LIB_TIMER_PATTERN) &&     \
             (pTimerCtxt->pattern2_ui == CM_LIB_TIMER_PATTERN)) ?      \
             TRUE : FALSE)

#define  CM_LIB_TIMER_INIT_TIMER_NODE_PATTERN_FIELDS(pTimer)       \
            {                                                       \
                pTimer->pattern1_ui = CM_LIB_TIMER_PATTERN;         \
            }

#define  CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer)        \
            ((pTimer->pattern1_ui == CM_LIB_TIMER_PATTERN) ? TRUE : FALSE)

#endif


#define  CM_LIB_TIMER_CLEAR_TIMER_FLAGS(pTimer)     \
              pTimer->ulFlags = 0;

#define  CM_LIB_TIMER_IS_TIMER_ACTIVE(pTimer)       \
              ((pTimer->ulFlags & CM_LIB_TIMER_ACTIVE) ? TRUE : FALSE)

#define  CM_LIB_TIMER_SET_TIMER_ACTIVE(pTimer)      \
              (pTimer->ulFlags |= CM_LIB_TIMER_ACTIVE)

#define  CM_LIB_TIMER_CLEAR_TIMER_ACTIVE(pTimer)    \
              (pTimer->ulFlags &= (~CM_LIB_TIMER_ACTIVE))



#define  CM_LIB_TIMER_IS_TIMER_PERIODIC(pTimer)      \
              ((pTimer->ulFlags & CM_LIB_TIMER_PERIODIC) ? TRUE : FALSE)

#define  CM_LIB_TIMER_SET_TIMER_PERIODIC(pTimer)     \
              (pTimer->ulFlags |= CM_LIB_TIMER_PERIODIC)

#define  CM_LIB_TIMER_CLEAR_TIMER_PERIODIC(pTimer)   \
              (pTimer->ulFlags &= (~CM_LIB_TIMER_PERIODIC))



#define  CM_LIB_TIMER_GET_TIMER_HEAP(pTimer)          \
              ((pTimer->ulFlags & CM_LIB_TIMER_HEAP) ? TRUE : FALSE)

#define  CM_LIB_TIMER_SET_TIMER_HEAP(pTimer, heap_b)   \
              if( heap_b == TRUE )                      \
                pTimer->ulFlags |= CM_LIB_TIMER_HEAP; \
              else                                     \
               pTimer->ulFlags &= (~CM_LIB_TIMER_HEAP)


#define  CM_LIB_TIMER_IS_TIMER_PASSIVE(pTimer)      \
              ((pTimer->ulFlags & CM_LIB_TIMER_PASSIVE) ? TRUE : FALSE)

#define  CM_LIB_TIMER_SET_TIMER_PASSIVE(pTimer)     \
              (pTimer->ulFlags |= CM_LIB_TIMER_PASSIVE)

#define  CM_LIB_TIMER_CLEAR_TIMER_PASSIVE(pTimer)   \
              (pTimer->ulFlags &= (~CM_LIB_TIMER_PASSIVE))


#define  CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer)     \
              ((pTimer->ulFlags & CM_LIB_TIMER_RUNNING) ? TRUE : FALSE)

#define  CM_LIB_TIMER_SET_TIMER_RUNNING(pTimer)     \
              (pTimer->ulFlags |= CM_LIB_TIMER_RUNNING)

#define  CM_LIB_TIMER_CLEAR_TIMER_RUNNING(pTimer)   \
              (pTimer->ulFlags &= (~CM_LIB_TIMER_RUNNING))

#endif /*  _CM_LIBTMR_LDEF_H_ */

/**************************************************************************/
/*                                                                        */
/*                        REVISION  HISTORY                               */
/*                        =================                               */
/*                                                                        */
/*  version      Date        Author         Change Description            */
/*  -------    --------      ------        ----------------------         */
/*  1.0        05-25-2005   Sridhar M       Initial implementation        */
/*  1.1        11-06-2006   Sridhar M       Introduced macro's            */
/*  1.2        12-06-2006   Sridhar M       passive list(BR 27876)        */
/*  1.3        13-06-2006   Sridhar M       timer latency fixes(BR 27877) */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


