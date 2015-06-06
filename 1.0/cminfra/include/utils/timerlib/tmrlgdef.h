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
 * File name: tmrlgdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: Timer library module provides the timer management
 *              functionality to process/tasks/threads. It is thread
 *              safe library. This header file contains the global
 *              definitions of this library. 
 * 
 */

#ifndef _CM_LIBTMR_GDEF_H_
#define _CM_LIBTMR_GDEF_H_

/*
 * structure used as call back argument for the timer user handler routine
 */
typedef struct  UCMLibTimerUserData_s
{
  void*  pArg1;
  void*  pArg2;
}UCMLibTimerUserData_t;


/*
 * structure used for giving the information required for UCMLibTimer library 
 * intialization. The callers are supposed to memset the structure field and
 * then start filling the fields as in future some more initialization fields
 * (optinal) may be added
 */
typedef  struct  UCMLibTimerLibInitInfo_s
{
  uint32_t  ulStaticTimers;
  uint32_t  ulTotalTimers;
  uint32_t  ulLockType;
}UCMLibTimerLibInitInfo_t;


typedef enum
{
  CM_LIBTMR_NO_LOCK = 0,
#if 0
  CM_LIBTMR_THREAD_LOCK,  
  CM_LIBTMR_NON_THREAD_LOCK
#endif
}UCMLibTimerLockTypes_t;

/*
 *  timer handler routine definition
 */
typedef  void  (*CM_LIBTIMER_TIMER_HANDLER) (void*  pTimer, \
                                             UCMLibTimerUserData_t*  user_data_p);

/*
 * enumaration for various timer types
 */
typedef  enum  
{
  CM_LIBTIMER_MILLI_SECS_TIMER, /* not yet supported */
  CM_LIBTIMER_SECS_TIMER,
  CM_LIBTIMER_MINS_TIMER,  /* not yet supported */
  CM_LIBTIMER_MAX_TIMER_TYPES ,
  CM_LIBTIMER_CLOCK_TICKS_TIMER, /* for internal use */
  CM_LIBTIMER_PERIODIC_TIMER = 0xA0000000,  /* not yet supported */
}UCMLibTimerTypes_t;

/*
 * enumeration for the library module's error values
 */
#define CM_LIB_TIMER_ERR_BASE_START  CM_LIB_TIMER_ERR_BASE 
#define CM_LIB_TIMER_ERR_BASE_END   (CM_LIB_TIMER_ERR_BASE + 990) 

typedef  enum
{
  CM_LIB_TIMER_ERR_INVALID_TOTAL_TIMERS = CM_LIB_TIMER_ERR_BASE_START,
  CM_LIB_TIMER_ERR_INVALID_ARGUMENT,
  CM_LIB_TIMER_ERR_INVALID_LOCK_TYPE,
  CM_LIB_TIMER_ERR_MEM_ALLOC_FAIL,
  CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT,
  CM_LIB_TIMER_ERR_INVALID_TIMER_NODE,
  CM_LIB_TIMER_ERR_TIMERS_EXHAUSTED,
  CM_LIB_TIMER_ERR_INVALID_TIMER_TYPE,
  CM_LIB_TIMER_ERR_INVALID_TIMEOUT_VALUE,
  CM_LIB_TIMER_ERR_TIMER_ACTIVE,
  CM_LIB_TIMER_ERR_TIMER_INACTIVE,
  CM_LIB_TIMER_ERR_TIMER_PRE_WAIT_CALL_NOT_INVOKED,
  CM_LIB_TIMER_ERR_MAX_ERRORS  = CM_LIB_TIMER_ERR_BASE_END
}UCMLibTimerErrors_t;


/* 
 * structure identifying the library instance statistics
 */
typedef struct UCMLibTimerStats_s
{
   uint32_t  ulMaxStaticTimers;
   uint32_t  ulMaxHeapTimers;

   uint32_t  ulAvailStaticTimers;
   uint32_t  ulAvailHeapTimers;

   uint32_t  ulAllocHits;
   uint32_t  ulReleaseHits;
 
   uint32_t  ulAllocFails;
   uint32_t  ulReleaseFails;

   uint32_t  ulActiveTimersStart;
   uint32_t  ulInactiveTimersStop;
   uint32_t  ulInactiveTimersRestart;
   uint32_t  ulInvalidTimerTypes;
   uint32_t  ulInvalidTimeoutValues;
}UCMLibTimerStats_t;


#define  CM_LIB_TMR_MILLI_SECONDS_PER_SECOND   1000
#define  CM_LIB_TMR_SECONDS_PER_MINUTE           60
#define  CM_LIB_TMR_MINUTES_PER_HOUR             60
#define  CM_LIB_TMR_CLOCK_TICKS_PER_SECOND       \
                         cm_lib_timer_get_total_clock_ticks_per_sec(NULL)


#endif /* _CM_LIBTMR_GDEF_H_ */


/**************************************************************************/
/*                                                                        */
/*                        REVISION  HISTORY                               */
/*                        =================                               */
/*                                                                        */
/*  version      Date        Author         Change Description            */
/*  -------    --------      ------        ----------------------         */
/*  1.0        05-25-2005   Sridhar M       Initial implementation        */
/*  1.1        13-06-2006   Sridhar M       timer latency fixes(BR 27877) */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


