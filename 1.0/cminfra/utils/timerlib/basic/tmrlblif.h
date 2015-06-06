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
 * File name: tmrlblif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: Timer library module provides the timer management    
 *              functionality to process/tasks/threads. It is thread  
 *              safe library. This header file contains the local    
 *              prototypes of this library.
 * 
 */

#ifndef _CM_LIBTMR_LIF_H_
#define _CM_LIBTMR_LIF_H_


 inline int32_t  cm_lib_timer_process_timers(
                                              UCMLibTimerCtxt_t  *pTimerCtxt,
                                              uint32_t           ulElapsedTime  
                                            );

  int32_t  cm_lib_timer_insert_node(
                                            UCMLibTimerCtxt_t*  pTimerCtxt,
                                            UCMLibTimer_t       *pTimer,
                                            uint32_t            ulListType
                                          );

  int32_t  cm_lib_timer_remove_node(
                                            UCMLibTimerCtxt_t*  pTimerLibCtxt,
                                            UCMLibTimer_t*      pTimer
                                          );

  void  cm_lib_timer_process_passive_list(
                                             UCMLibTimerCtxt_t  *pTimerCtxt 
                                            );

 inline  unsigned char   cm_lib_timer_is_timer_type_valid(
                                                          uint32_t ulTimerType
                                                        );

 inline  uint32_t  cm_lib_timer_convert_timer_val(
                                             UCMLibTimerCtxt_t  *pTimerCtxt,
                                             uint32_t           ulTimerType,
                                             uint32_t           ulTimerVal,
                                             uint32_t           ulDesTimerType
                                            );

TR_IMPORT uint32_t  cmi_lib_timer_get_total_clock_ticks_per_sec(
                                                          void
                                                       );

#endif /* _CM_LIBTMR_LIF_H_ */

/**************************************************************************/
/*                                                                        */
/*                        REVISION  HISTORY                               */
/*                        =================                               */
/*                                                                        */
/*  version      Date        Author         Change Description            */
/*  -------    --------      ------        ----------------------         */
/*  1.0        05-25-2005   Sridhar M       Initial implementation        */
/*  1.1        12-06-2006   Sridhar M       passive list(BR 27876)        */
/*  1.2        13-06-2006   Sridhar M       timer latency fixes(BR 27877) */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


