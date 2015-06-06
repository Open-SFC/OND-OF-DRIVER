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
 * File name: cmipcwrp.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

#ifndef _UCMIPCWRP_H
#define _UCMIPCWRP_H
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

#define CM_SEM_CREATE(Lock, flags)     ((Lock=cm_mutex_init(flags)) != 0) \
                                            ? OF_SUCCESS : OF_FAILURE
#define CM_SEM_TAKE(Lock, bsemWait)    cm_mutex_lock(Lock, bsemWait)
#define CM_SEM_GIVE(Lock)              cm_mutex_unlock(Lock)
#define CM_SEM_DEL(Lock)               cm_mutex_destroy(Lock)

t_sem_id_t cm_sem_init (uint32_t count_ui, uint32_t flags_ui);
int32_t cm_sem_destroy (t_sem_id_t sm_id_ui);
int32_t cm_sem_wait (t_sem_id_t sm_id_ui);
int32_t cm_sem_post (t_sem_id_t sm_id_ui);

t_sem_id_t cm_mutex_init (uint32_t flags_ui);
int32_t cm_mutex_destroy (t_sem_id_t sm_id_ui);
int32_t cm_mutex_lock (t_sem_id_t sm_id_ui, uint32_t flags_ui);
int32_t cm_mutex_unlock (t_sem_id_t sm_id_ui);

#endif
