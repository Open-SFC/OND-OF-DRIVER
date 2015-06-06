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
 * File name: tmrlgif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: Timer library module provides the timer management
 *              functionality to process/tasks/threads. It is thread
 *              safe library. This header file contains the global
 *              prototypes of this library.
 * 
 */

#ifndef  _CM_LIBTMR_GIF_H_
#define  _CM_LIBTMR_GIF_H_

int32_t  cm_lib_timer_init(
                          UCMLibTimerLibInitInfo_t  *pInitInfo,
                          void                    **ppTimerLibCtxt
                        );

int32_t  cm_lib_timer_de_init(
                            void   **ppTimerLibCtxt
                          );

int32_t  cm_lib_timer_create_timer(
                                 void       *pTimerLibCtxt,
                                 void       **ppTimer
                               );

int32_t  cm_lib_timer_delete_timer(
                                 void        *pTimerLibCtxt,
                                 void        **ppTimer
                                );

int32_t  cm_lib_timer_start_timer(
                                void                      *pTimerLibCtxt,
                                void                      *pTimer,
                                CM_LIBTIMER_TIMER_HANDLER  pHandler,
                                UCMLibTimerUserData_t       *user_data_p,
                                uint32_t                    ulTimerType,
                                uint32_t                    ulTimerValue
                              );

int32_t  cm_lib_timer_stop_timer(
                               void        *pTimerLibCtxt,
                               void        *pTimer
                             );

int32_t  cm_lib_timer_restart_timer(
                                  void        *pTimerLibCtxt,
                                  void        *pTimer
                                );

int32_t  cm_lib_timer_restart_timer_with_new_value(
                                              void        *pTimerLibCtxt,
                                              void        *pTimer,
                                              uint32_t       ulTimerValue
                                            );

int32_t  cm_lib_timer_get_next_expiry_timer_remval(
                                               void     *pTimerLibCtxt,
                                               uint32_t   *pTimerVal
                                             );

int32_t  cmo_lib_timer_process_timers(
                                   void    *pTimerLibCtxt,
                                   uint32_t   ulElapsedTime  
                                 );

int32_t  cm_lib_timer_pre_wait(
                             void   *pTimerLibCtxt,
                             uint32_t *pTimerVal  
                           );

int32_t  cm_lib_timer_post_wait(
                              void   *pTimerLibCtxt
                            );

unsigned char  cm_lib_timer_is_timer_active(
                                   void   *pTimerLibCtxt,
                                   void   *pTimer  
                                 );

int32_t  cm_lib_timer_get_timer_expiry_val(
                                       void   *pTimerLibCtxt,
                                       void   *pTmr,
                                       uint32_t *pulRemVal 
                                     );

int32_t  cm_lib_timer_get_stats(
                              void               *pTimerLibCtxt,
                              UCMLibTimerStats_t   *pStats     
                            );

void  cm_lib_timer_get_time_stamp(
                                 uint32_t  *pulTimeStamp
                                );

uint32_t  cm_lib_timer_get_elapsed_time(
                                   uint32_t  ulTimeStampBefWait,
                                   uint32_t  ulTimeStampAftWait
                                 );

uint32_t  cm_lib_timer_get_total_clock_ticks_per_sec(
                                                void     *pTimerLibCtxt
                                             );

#endif /* _CM_LIBTMR_GIF_H_ */

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


