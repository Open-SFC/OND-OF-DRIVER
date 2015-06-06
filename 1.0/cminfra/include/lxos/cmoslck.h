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
 * File name: ucmoslck.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains lock macros.
 * 
 */

#ifndef _UCMOSLCK_H_
#define _UCMOSLCK_H_

//#include "controller.h"

static inline unsigned char cm_return_true (void)
{
  return TRUE;
}

static inline int32_t cm_return_success (void)
{
  return OF_SUCCESS;
}

typedef struct
{
} cm_lock_t;
#define CM_LOCK_INIT(Lock)            return_true()
#define CM_LOCK_DELETE(Lock)
#define CM_LOCK_INIT_AND_TAKE(Lock)   return_true()
#define CM_LOCK_TAKE(Lock)
#define CM_LOCK_TRY(Lock)             return_success()
#define CM_LOCK_RELEASE(Lock)

typedef struct
{
} cm_rw_lock_t;
#define CM_RWLOCK_INIT(Lock)          return_true()
#define CM_RWLOCK_DELETE(Lock)
#define CM_READ_LOCK_TAKE(Lock)
#define CM_READ_LOCK_RELEASE(Lock)
#define CM_WRITE_LOCK_TAKE(Lock)
#define CM_WRITE_LOCK_RELEASE(Lock)

#define UCMSmpLock_t                   cm_lock_t
#define CM_SMP_LOCK_INIT(Lock)        return_true()
#define CM_SMP_LOCK_DELETE(Lock)
#define CM_SMP_LOCK_INIT_AND_TAKE(Lock) return_true()
#define CM_SMP_LOCK_TAKE(Lock)
#define CM_SMP_LOCK_TRY(Lock)         return_success()
#define CM_SMP_LOCK_RELEASE(Lock)

#define UCMSmpRWLock_t                       cm_rw_lock_t
#define CM_SMP_RWLOCK_INIT(Lock)            return_true()
#define CM_SMP_RWLOCK_DELETE(Lock)
#define CM_SMP_READ_LOCK_TAKE(Lock)
#define CM_SMP_READ_LOCK_RELEASE(Lock)
#define CM_SMP_WRITE_LOCK_TAKE(Lock)
#define CM_SMP_WRITE_LOCK_RELEASE(Lock)

#define CM_MUTEX_DECLARE(Lock)
#define CM_MUTEX_INIT(Lock)           return_true()
#define CM_MUTEX_DELETE(Lock)
#define CM_MUTEX_TAKE(Lock)
#define CM_MUTEX_RELEASE(Lock)

#endif
