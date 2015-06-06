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
 * File name: tmrgdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: Timer Module Global Defines.
 * 
 */

#ifndef TMRGDEF_H
#define TMRGDEF_H

#define PERIODIC_TMR    0
#define MILLI_SECS_TMR  1
#define SECS_TMR        2
#define MINS_TMR        3

typedef struct itimer_s cm_timer_t;
typedef void (*VOIDFUNPTR) (void *);
typedef struct itimer_s
{
  VOIDFUNPTR proutine;
  void *parg;
  uint32_t cur_tmr_count_ui;
  uint32_t timer_value_ui;
  uint8_t timer_type_ui;
  unsigned char b_dynamic;
  uint32_t debug_info_ui;
  uint32_t uiactive;
  struct itimer_s *pnext;
  struct itimer_s *pprev;
} itimer_t;
typedef struct timer_statistics_s
{
  uint32_t static_alloc_hits_ul;
  uint32_t dynamic_alloc_hits_ul;
  uint32_t static_alloc_frees_ul;
  uint32_t dynamic_alloc_frees_ul;
  uint32_t dynamic_alloc_fails_ul;
  uint32_t not_present_error;
  uint32_t insert_errors;
  uint32_t timer_id_present;
  uint32_t invalid_timer_type;
  uint32_t invalid_timer_id;
  uint32_t alloc_failures;
} timer_statistics_t;

typedef struct timer_node_info_s
{
  uint32_t cur_tmr_count_ui;
  uint32_t timer_value_ui;
  uint32_t cmu_time_ui;
  uint8_t timer_type_ui;
  struct itimer_s *my_addr_p;
} timer_node_info_t;

typedef struct dst_record_s
{
  uint32_t cmd_type_ui;
  uint32_t uindex_i;
  unsigned char enable_b;
} dst_record_t;

typedef enum dst_enable_s
{
  SET_DST = 1,
  RESET_DST,
  SET_DST_ENABLE,
  GET_DST_ENABLE,
  DST_SNTPC_SET,
  SET_GLOBALDST_ENABLE,
  GET_GLOBALDST_ENABLE,
  TIME_SET,
  SET_ZONE
} dst_enable_t;

#endif
