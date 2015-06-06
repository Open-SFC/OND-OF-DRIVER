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
 * File name: tmrlbprt.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: Timer library, provides the timer management          
 *              functionality to process/tasks/threads. It is a thread
 *              safe library.
 * 
 */

#include <sys/times.h>
#include <unistd.h>
#include "cmincld.h"
#include "tmrlgdef.h"
#include "tmrlgif.h"

#define  CM_LIB_TMR_MAX_TIMESTAMP  0xfffffffe

uint32_t  cmi_lib_timer_get_total_clock_ticks_per_sec(
                                               void
                                             );

/*****************************************************************************/
/* Function        : cm_lib_timer_get_time_stamp                                 */
/* Description     : This API can be used by the application to record the   */
/*                   timestamp immediately before waiting for the events     */
/*                   and after immediately after finishing waiting for the   */
/*                   events. The difference in the two time stamps give a    */
/*                   rough estimation of time spent in waiting for events.   */
/*                   For timestamp linux "times" system call is used. Since  */
/*                   this system call returns the number of clock ticks that */
/*                   have elapsed since the system booted, there is chance of*/
/*                   this integer value overlapping. So use the API (below)  */
/*                   cm_lib_timer_get_elapsed_time to calculate the time elapsed */
/*                   in waiting for the events. This API takes care of the   */
/*                   overlapping case.                                       */
/* Input Arguments :                                                         */
/*    pulTimeStamp   -  pointer to memory where timestamp should be filled.  */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : None.                                                   */
/*****************************************************************************/
void  cm_lib_timer_get_time_stamp(
                                 uint32_t   *pulTimeStamp
                                )
{
 /*
  * times return jiffies(i.e., elapsed time in clock ticks from system reboot).
  */
  *pulTimeStamp = times(NULL);

  return;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_get_elapsed_time                               */
/* Description     : This API can be used by the application to estimate the */
/*                   time elapsed in waiting for the events. The elapsed time*/
/*                   returned by this API is passed as input to the API      */
/*                   cmo_lib_timer_process_timers which is used to process the   */
/*                   active timers. It is assumed that the timestamps passed */
/*                   to this API are taken using API cm_lib_timer_get_time_stamp.*/
/* Input Arguments :                                                         */
/*    ulTimeStampBefWait  -  timestamp before waiting for the events.        */
/*    ulTimeStampAftWait  -  timestamp after coming out of waiting.          */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : elapsed time in clock ticks.                            */
/*****************************************************************************/
uint32_t  cm_lib_timer_get_elapsed_time(
                                   uint32_t  ulTimeStampBefWait,
                                   uint32_t  ulTimeStampAftWait
                                 )
{
  uint32_t  ulElapsedTime;

  if(ulTimeStampAftWait >= ulTimeStampBefWait)
  {
    ulElapsedTime = ulTimeStampAftWait - ulTimeStampBefWait;
  }
  else
  {
    ulElapsedTime = (CM_LIB_TMR_MAX_TIMESTAMP - ulTimeStampBefWait) +
                     ulTimeStampAftWait;
  }

#if 0
 /*
  * convert it in to seconds. iGateway API GetTicksPerSec()
  * can also be used here.
  */
  ulElapsedTime /= sysconf(_SC_CLK_TCK);
#endif

  return ulElapsedTime;
}

/*****************************************************************************/
/* Function        : cmi_lib_timer_get_total_clock_ticks_per_sec                     */
/* Description     : This API returns the number of clock ticks in a second  */
/* Input Arguments :                                                         */
/*    NONE                                                                   */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : number of clock ticks per second.                       */
/*****************************************************************************/
uint32_t  cmi_lib_timer_get_total_clock_ticks_per_sec(
                                                void
                                             )
{
  return sysconf(_SC_CLK_TCK);
}



/**************************************************************************/
/*                                                                        */
/*                        REVISION  HISTORY                               */
/*                        =================                               */
/*                                                                        */
/*  version      Date        Author         Change Description            */
/*  -------    --------      ------        ----------------------         */
/*  1.0        26-01-2006   Sridhar M       Initial implementation        */
/*  1.1        31-01-2006   Sridhar M       added function headers        */
/*  1.2        13-06-2006   Sridhar M       timer latency fixes(BR 27877) */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


