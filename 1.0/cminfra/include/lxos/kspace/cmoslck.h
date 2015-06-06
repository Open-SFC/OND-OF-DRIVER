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
#ifdef CM_LOCKS_HEAVY_DEBUG
#define PRINT_LEVEL KERN_CRIT
#define LockPrint(x) printk x
#else
#define PRINT_LEVEL 
#define LockPrint(x) 
#endif

#define  

#define PSL(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s lock\n\r",__FUNCTION__,__LINE__,#Lock))
#define PSR(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s Rele\n\r",__FUNCTION__,__LINE__,#Lock))
#define PST(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s Try success\n\r",__FUNCTION__,__LINE__,#Lock))

#define PWL(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s Wr lock\n\r",__FUNCTION__,__LINE__,#Lock))
#define PWR(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s Wr Rele\n\r",__FUNCTION__,__LINE__,#Lock))
#define PRL(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s Rd lock\n\r",__FUNCTION__,__LINE__,#Lock))
#define PRR(Lock)   LockPrint((PRINT_LEVEL "%s:%d %s Rd Rele\n\r",__FUNCTION__,__LINE__,#Lock))

static inline unsigned char return_true(void)
{
  return TRUE;
}
static inline int32_t return_success(void)
{
  return OF_SUCCESS;
}

#ifndef CM_LOCKS_DEBUG

#define cm_lock_t                            spinlock_t
#define CM_LOCK_INIT(Lock)                  spin_lock_init(&(Lock))
static inline unsigned char spin_lock_init(spinlock_t *Lock)
{
    *Lock = SPIN_LOCK_UNLOCKED;
    return TRUE;
}

#define CM_LOCK_INIT_AND_TAKE(Lock)         spin_lock_init_and_take(&(Lock))
static inline unsigned char spin_lock_init_and_take(spinlock_t *Lock)
{
    *Lock = SPIN_LOCK_UNLOCKED;
    spin_lock_bh(Lock);
    return TRUE;
}
#define CM_LOCK_DELETE(Lock)                

#define CM_LOCK_TAKE(Lock)                  do{PSL(Lock); \
                                                spin_lock_bh(&(Lock)); \
                                             }while(0)
#define CM_LOCK_TRY(Lock)                   ((spin_trylock_bh(&(Lock)) != 0) ? OF_SUCCESS : OF_FAILURE)
#define CM_LOCK_RELEASE(Lock)               do{PSR(Lock); \
                                                spin_unlock_bh(&(Lock)); \
                                             }while(0)

/*#define cm_rw_lock_t                          rwlock_t*/
typedef struct { 
  rwlock_t  RWLock;
  cm_atomic_t  lock_write_cntr; /* to avoid WRITE starvation */
  cm_lock_t wr_ctr_lck;
} cm_rw_lock_t;

#define CM_RWLOCK_INIT(Lock)                rw_lock_init(&(Lock))
#define CM_RWLOCK_DELETE(Lock)
 
static inline unsigned char rw_lock_init(cm_rw_lock_t *Lock)
{
    Lock->RWLock = RW_LOCK_UNLOCKED;
    CM_LOCK_INIT(Lock->wr_ctr_lck);
    CM_ATOMIC_SET(Lock->lock_write_cntr, 0);
    return TRUE;
}

#define CM_READ_LOCK_TAKE(Lock)             do{PRL(Lock); \
                                                CM_WHILE_COUNTER_NOT_EQUAL_TO_ZERO_TMP((Lock).lock_write_cntr); \
                                                read_lock_bh(&((Lock).RWLock)); \
                                             }while(0)
#define CM_READ_LOCK_RELEASE(Lock)          do{PRR(Lock); \
                                                read_unlock_bh(&((Lock).RWLock)); \
                                             }while(0)
#define CM_WRITE_LOCK_TAKE(Lock)            do{PWL(Lock); \
            CM_LOCK_TAKE((Lock).wr_ctr_lck);\
            CM_ATOMIC_INC ((Lock).lock_write_cntr); \
            write_lock_bh(&((Lock).RWLock)); \
            CM_ATOMIC_DEC ((Lock).lock_write_cntr); \
            CM_LOCK_RELEASE((Lock).wr_ctr_lck);\
            }while(0)
#define CM_WRITE_LOCK_RELEASE(Lock)         do{PWR(Lock); \
                                                write_unlock_bh(&((Lock).RWLock)); \
                                             }while(0)

#define CM_MUTEX_DECLARE(Lock)              DECLARE_MUTEX(Lock)
#define CM_MUTEX_INIT(Lock)                 return_true()
#define CM_MUTEX_TAKE(Lock)                 down(&(Lock))
#define CM_MUTEX_RELEASE(Lock)              up(&(Lock))
#define CM_MUTEX_DELETE(Lock)               

#else /*CM_LOCKS_DEBUG*/

void ucmLockDebugAddNodeToList(void *pPtr,
                                 unsigned char *pFunctionName,
                                 unsigned char *pLockName,
                                 int8_t iFlags);

void ucmLockDebugRemoveNodeFromList(void **pPtr);
extern uint32_t GetCurrentTickValue();

#define CM_SPIN_MAGIC 0xf0f00f0f
/* iGateway Instrumentation changes */

#define MAX_CPUS  NR_CPUS
#define LOCK_TYPE_SPIN      1
#define LOCK_TYPE_RW_READ   2
#define LOCK_TYPE_RW_WRITE  3

#define SPIN_LOCK_ADD_NODE_TO_LIST(lock,function) \
            ucmLockDebugAddNodeToList((void *)lock,function,#lock,LOCK_TYPE_SPIN)

#define READ_LOCK_ADD_NODE_TO_LIST(lock,function) \
            ucmLockDebugAddNodeToList((void *)lock,function,#lock,LOCK_TYPE_RW_READ)

#define WRITE_LOCK_ADD_NODE_TO_LIST(lock,function) \
            ucmLockDebugAddNodeToList((void *)lock,function,#lock,LOCK_TYPE_RW_WRITE)

#define LOCK_REMOVE_NODE_FROM_LIST(locknode) \
            ucmLockDebugRemoveNodeFromList((void **)(&(locknode)))

typedef struct 
{
  spinlock_t l;
  atomic_t locked_by;
  unsigned magic;
  void *pNode;
}cm_lock_t;

typedef struct 
{
   rwlock_t l;
   long read_locked_map;
   long write_locked_map;
   unsigned magic;
  void *pWriteNode;
  unsigned long ulReadNodes[MAX_CPUS];/*To Store the timer node when Read lock acuired*/
}cm_rw_lock_t;

#define CM_LOCK_DECLARE(Lock)               cm_lock_t Lock
#define CM_LOCK_INIT(Lock)                  spin_lock_init(&(Lock))
static inline unsigned char spin_lock_init(cm_lock_t *Lock)
{
    cm_lock_t LocalLock = {SPIN_LOCK_UNLOCKED,ATOMIC_INIT(-1),CM_SPIN_MAGIC,NULL};
    *Lock = LocalLock;
    return TRUE;
}


#define CM_LOCK_MUST_BE_INITIALIZED(Lock) \
do { \
    if ((Lock)->magic != CM_SPIN_MAGIC) { \
       printk(KERN_EMERG "##### \n %s is not initialized\n",#Lock); \
       printk(KERN_EMERG "trying to lock/unlock in file %s at line %d \n#####\n", \
                                                          __FILE__,__LINE__); \
       KernDumpStack();\
       KernPanic("Trying to lock/unlock a uninitilized Lock"); \
    } \
} while(0)


#define CM_LOCK_MUST_BE_UNLOCKED(Lock) \
do { \
     if (atomic_read(&(Lock)->locked_by) == smp_processor_id()) { \
        printk(KERN_EMERG "###DEAD LOCK: Trying to lock %s again in %s at line %u \n", \
               #Lock,__FILE__, __LINE__); \
       KernDumpStack();\
       KernPanic("DEAD LOCK: Trying to lock again"); \
     } \
} while(0)

#ifdef CM_CONFIG_SMP
#define SPIN_IS_LOCKED_STRING(Lock) \
    if (!spin_is_locked(&(Lock)->l)) { \
        printk(KERN_EMERG "###ASSERT:\nTrying to release %s that is not locked\n in %s:%d\n###", \
               #Lock,__FILE__,__LINE__); \
        KernDumpStack();\
        KernPanic("Releasing a lock that is not aquired"); \
   } 
#else
#define SPIN_IS_LOCKED_STRING(Lock) 
#endif

#define CM_LOCK_MUST_BE_LOCKED(Lock) \
do { \
    SPIN_IS_LOCKED_STRING(Lock)  \
    if (atomic_read(&(Lock)->locked_by) != smp_processor_id()) { \
        printk(KERN_EMERG "####\nTrying to release %s that is locked on different cpu\n",#Lock); \
        printk(KERN_EMERG "%s:%u\n####", __FILE__, __LINE__); \
        KernDumpStack();\
        KernPanic("Releasing lock on a different CPU"); \
    } \
} while(0)


#define CM_LOCK_TAKE(Lock) \
do { \
     PSL(Lock); \
     CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
     CM_LOCK_MUST_BE_UNLOCKED(&(Lock)); \
     spin_lock_bh(&((&(Lock))->l)); \
     SPIN_LOCK_ADD_NODE_TO_LIST((&Lock),(unsigned char *)__FUNCTION__); \
     atomic_set(&((&(Lock))->locked_by), smp_processor_id()); \
} while(0)

#define CM_LOCK_TRY(Lock) SpinTryLock(&(Lock),(unsigned char *)__FUNCTION__)
static inline int32_t SpinTryLock(cm_lock_t *Lock,unsigned char *name_p)
{
    CM_LOCK_MUST_BE_INITIALIZED(Lock);
    if(spin_trylock_bh(&((Lock)->l)) != 0)
    {
      PST(Lock);
      SPIN_LOCK_ADD_NODE_TO_LIST(Lock,name_p);
      atomic_set(&((Lock)->locked_by), smp_processor_id());
      return OF_SUCCESS;
    }
    else
    {
      return OF_FAILURE;
    }
}

#define CM_LOCK_RELEASE(Lock) \
do { \
     PSR(Lock); \
     CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
     CM_LOCK_MUST_BE_LOCKED(&(Lock)); \
     atomic_set(&((&(Lock))->locked_by), -1); \
     LOCK_REMOVE_NODE_FROM_LIST((&(Lock))->pNode); \
     spin_unlock_bh(&((&(Lock))->l)); \
} while(0)

#define CM_LOCK_INIT_AND_TAKE(Lock)         spin_lock_init_and_take(&(Lock),(unsigned char *)__FUNCTION__)
static inline unsigned char spin_lock_init_and_take(cm_lock_t *Lock,unsigned char *name_p)
{
    cm_lock_t LocalLock = {SPIN_LOCK_UNLOCKED,ATOMIC_INIT(-1),CM_SPIN_MAGIC,NULL};
    *Lock = LocalLock;
    spin_lock_bh(&(Lock->l));
    SPIN_LOCK_ADD_NODE_TO_LIST(Lock,name_p);
    atomic_set(&((Lock)->locked_by), smp_processor_id());
    return TRUE;
}
#define CM_LOCK_DELETE(Lock)                


#define CM_RWLOCK_MUST_BE_READ_WRITE_UNLOCKED(Lock) \
do { if ((Lock)->read_locked_map & (1 << smp_processor_id())) { \
        printk(KERN_ALERT "###\nASSERT:Trying for read lock \"%s\" that is already acquired",#Lock); \
        printk(KERN_ALERT "\n%s:%d\n###",__FILE__, __LINE__); \
     } \
     else if ((Lock)->write_locked_map & (1 << smp_processor_id())){ \
        printk(KERN_EMERG "DeadLock###Trying for write lock %s again in file %s at line %d \n###", \
               #Lock,__FILE__, __LINE__); \
        KernDumpStack();\
        KernPanic("Trying to acquire an already acquired write lock on the same CPU"); \
     } \
} while(0)

#define CM_RWLOCK_MUST_BE_WRITE_LOCKED(Lock) \
do { \
     if (!((Lock)->write_locked_map & (1 << smp_processor_id()))){ \
        printk(KERN_EMERG "###Trying to release write lock \"%s\", which is not taken at all",#Lock); \
        printk(KERN_EMERG "or taken on different cpu\n%s:%d\n####",__FILE__, __LINE__); \
        KernDumpStack();\
        KernPanic("Trying to release a lock, without taking it"); \
     } \
} while(0)


#define CM_RWLOCK_INIT(Lock)                rw_lock_init(&(Lock))
static inline unsigned char rw_lock_init(cm_rw_lock_t *Lock)
{
    int32_t ii = 0;
    Lock->l = RW_LOCK_UNLOCKED;
    Lock->read_locked_map = Lock->write_locked_map = 0;
    Lock->magic = CM_SPIN_MAGIC;
    Lock->pWriteNode = NULL;
    for(ii=0;ii<MAX_CPUS;ii++)
    {
      Lock->ulReadNodes[ii] = 0;
    }
    return TRUE;
}
#define CM_RWLOCK_DELETE(Lock)              

#define CM_READ_LOCK_TAKE(Lock) \
do { \
    PRL(Lock); \
    CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
    CM_RWLOCK_MUST_BE_READ_WRITE_UNLOCKED(&(Lock)); \
    read_lock_bh(&((&(Lock))->l)); \
    READ_LOCK_ADD_NODE_TO_LIST((&Lock),(unsigned char *)__FUNCTION__); \
    set_bit(smp_processor_id(), &((&(Lock))->read_locked_map)); \
} while(0)

#define CM_READ_LOCK_RELEASE(Lock) \
do { \
    PRR(Lock); \
    CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
    if (!(((&(Lock))->read_locked_map) & (1 << smp_processor_id()))) { \
        printk(KERN_EMERG "###\nASSERT: trying to release read lock \"%s\" which is not taken at all or",#Lock); \
        printk(KERN_EMERG "taken on different cpu\n%s:%d\n###",__FILE__, __LINE__); \
        KernDumpStack();\
        KernPanic("Trying to release read lock (not taken/taken on different CPU)"); \
    } \
    clear_bit(smp_processor_id(), &((&(Lock))->read_locked_map)); \
    LOCK_REMOVE_NODE_FROM_LIST((&(Lock))->ulReadNodes[smp_processor_id()]); \
    read_unlock_bh(&((&(Lock))->l)); \
} while(0)

#define CM_WRITE_LOCK_TAKE(Lock) \
do { \
    PWL(Lock); \
    CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
    CM_RWLOCK_MUST_BE_READ_WRITE_UNLOCKED(&(Lock)); \
    write_lock_bh(&(&(Lock))->l); \
    WRITE_LOCK_ADD_NODE_TO_LIST((&Lock),(unsigned char *)__FUNCTION__); \
    set_bit(smp_processor_id(), &(&(Lock))->write_locked_map); \
} while(0)

#define CM_WRITE_LOCK_RELEASE(Lock) \
do { \
    PWR(Lock); \
    CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
    CM_RWLOCK_MUST_BE_WRITE_LOCKED(&(Lock)); \
    clear_bit(smp_processor_id(), &(&(Lock))->write_locked_map); \
    LOCK_REMOVE_NODE_FROM_LIST((&(Lock))->pWriteNode); \
    write_unlock_bh(&(&(Lock))->l); \
} while(0)

#define CM_MUTEX_DECLARE(Lock)              DECLARE_MUTEX(Lock)
#define CM_MUTEX_INIT(Lock)                 return_true()
#define CM_MUTEX_TAKE(Lock)                 down(&(Lock))
#define CM_MUTEX_RELEASE(Lock)              up(&(Lock))
#define CM_MUTEX_DELETE(Lock)               
/* DEBUG_LOCK_TAKE and DEBUG_LOCK_RELEASE macros are used to protect
 * the Per CPU Global list which is added as part code instrumentation.
 * These two new macros are added as we can not use the existing macros 
 * to protect the global list
 */

#define DEBUG_LOCK_TAKE(Lock) \
do { \
     CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
     CM_LOCK_MUST_BE_UNLOCKED(&(Lock)); \
     spin_lock_bh(&((&(Lock))->l)); \
     atomic_set(&((&(Lock))->locked_by), smp_processor_id()); \
} while(0)


#define DEBUG_LOCK_RELEASE(Lock) \
do { \
    CM_LOCK_MUST_BE_INITIALIZED(&(Lock)); \
    CM_LOCK_MUST_BE_LOCKED(&(Lock)); \
    atomic_set(&((&(Lock))->locked_by), -1); \
    spin_unlock_bh(&((&(Lock))->l)); \
} while(0)


#endif /*CM_LOCKS_DEBUG*/

#ifdef CM_CONFIG_SMP
#define UCMSmpLock_t                       cm_lock_t
#define CM_SMP_LOCK_INIT                  CM_LOCK_INIT
#define CM_SMP_LOCK_DELETE                CM_LOCK_DELETE
#define CM_SMP_LOCK_INIT_AND_TAKE         CM_LOCK_INIT_AND_TAKE
#define CM_SMP_LOCK_TAKE                  CM_LOCK_TAKE
#define CM_SMP_LOCK_TRY                   CM_LOCK_TRY
#define CM_SMP_LOCK_RELEASE               CM_LOCK_RELEASE

#define UCMSmpRWLock_t                     cm_rw_lock_t
#define CM_SMP_RWLOCK_INIT                CM_RWLOCK_INIT
#define CM_SMP_RWLOCK_DELETE              CM_RWLOCK_DELETE
#define CM_SMP_READ_LOCK_TAKE             CM_READ_LOCK_TAKE
#define CM_SMP_READ_LOCK_RELEASE          CM_READ_LOCK_RELEASE
#define CM_SMP_WRITE_LOCK_TAKE            CM_WRITE_LOCK_TAKE
#define CM_SMP_WRITE_LOCK_RELEASE         CM_WRITE_LOCK_RELEASE

#else

typedef struct
{
}UCMSmpLock_t;

#define CM_SMP_LOCK_INIT(Lock)            return_true()
#define CM_SMP_LOCK_DELETE(Lock)                
#define CM_SMP_LOCK_INIT_AND_TAKE(Lock)   return_true()
#define CM_SMP_LOCK_TAKE(Lock)                  
#define CM_SMP_LOCK_TRY(Lock)             return_success()
#define CM_SMP_LOCK_RELEASE(Lock)               

typedef struct
{
}UCMSmpRWLock_t;

#define CM_SMP_RWLOCK_INIT(Lock)          return_true()
#define CM_SMP_RWLOCK_DELETE(Lock)              
#define CM_SMP_READ_LOCK_TAKE(Lock)             
#define CM_SMP_READ_LOCK_RELEASE(Lock)          
#define CM_SMP_WRITE_LOCK_TAKE(Lock)            
#define CM_SMP_WRITE_LOCK_RELEASE(Lock)         
#endif /*CM_CONFIG_SMP*/

#endif /*_UCMOSLCK_H_*/
