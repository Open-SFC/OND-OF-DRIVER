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
 * File name: timer.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: Timer definitions.
 * 
 */

#ifndef CM_TIMER_H
#define CM_TIMER_H

#define TRUE            1
#define FALSE           0
#define OK              0
#define MAX_TIMER_LISTS 0x04
#define TMR_INACTIVE    0x10000005


/*  TIMER*/
int32_t cm_timer_create (cm_timer_t ** pTimer);
int32_t cm_timer_start (cm_timer_t * pTimer, VOIDFUNPTR proutine, void *parg,
                         uint8_t timer_type_ui, uint32_t timer_value_ui,
                         uint32_t debug_info_ui);
int32_t cm_timer_stop (cm_timer_t * pTimer);
int32_t cm_timer_delete (cm_timer_t * pTimer);
int32_t cm_timer_restart (cm_timer_t * pTimer);
int32_t cm_timer_restart_with_new_value (cm_timer_t * pTimer,
                                       uint32_t uinewValue);
cm_timer_t *cm_timer_alloc (void);
void cm_timer_free (cm_timer_t * pTimer);
int32_t cm_timer_init (void);
int32_t cm_timer_get_remaining_time (cm_timer_t * pTimer,
                                    uint32_t * pgetRemngTime);
unsigned char cm_is_active_timer (cm_timer_t * pTimer, uint8_t timer_type_ui);
uint32_t cm_timer_duration (cm_timer_t * pTimer);
VOIDFUNPTR cm_timer_get_callback_fn (cm_timer_t * pTimer);
void *cm_timer_get_callback_arg (cm_timer_t * pTimer);
void cm_timer_set_callback_arg (cm_timer_t * pTimer, void * parg);
int32_t cm_time_out_action_handler (cm_timer_t * pTimer, void * arg);
void cm_timer_process (uint32_t units);
void cm_timer_node_update (cm_timer_t * pTimer, uint32_t uiactualTime);
int32_t cm_get_first_timer_timeout_val (void);
void cm_timer_call_out (cm_timer_t * pTimer);
uint32_t cm_get_current_tick_value(void);
int32_t cm_timer_process_nodes(uint32_t ulTimeOutPassedInMsec);
#endif
