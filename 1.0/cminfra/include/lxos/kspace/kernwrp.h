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
 * File name: kernwrp.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

#ifndef __KERNWRP_H
#define __KERNWRP_H


#define KERN_GFP_ATOMIC	GFP_ATOMIC
#define KERN_GFP_KERNEL	GFP_KERNEL

#define of_strlen(str)     KernStrLen(str)
#define of_strcmp(str1,str2)   KernStrCmp(str1,str2)

#define of_strncmp(str1,str2,nbytes) KernStrNCmp(str1,str2,nbytes)
#define of_strcat(str1,str2)   KernStrCat(str1,str2)
#define of_strncat(str1,str2,nbytes) KernStrNCat(str1,str2,nbytes)
#define of_strstr(haystack,needle) KernStrStr(haystack,needle)
#define of_strdup(str)     KernStrDup(str)

#define of_memset(ptr,byte,size)   KernMemSet(ptr,byte,size)
#define T_memzero(ptr,size)   KernMemSet(ptr,0,size)
#define of_memcpy(dst,src,len)   KernMemCpy(dst,src,len)
#define t_mem_cmp(mem1,mem2,size)  KernMemCmp(mem1,mem2,size)

#define KernPrint	printk
#define KernAlert	printk
#define KernCritical	printk

#define KernStartBhAtomic()
#define KernEndBhAtomic()
/*
 * Semaphore related macro
 */

#define CM_DECLARE_MUTEX(name)     DECLARE_MUTEX(name) 

#define KernDown(x)                 down(x)
#define KernUp(x)                   up(x)


/*
 * Memory Allocation/Free Macros
 */
#ifdef DYN_ALLOC_HEAP_DBG
/*
 * Map all allocation/frees to of_malloc/of_free() 
 */
 #define kmalloc(x,y) of_malloc(x)
#define kfree of_free
#else
#define KernFree(ptr)     kfree(ptr)
#endif
/*
 * User Space Interface
 */

#define KernCopyToUser(userptr,kernptr,size)	copy_to_user(userptr,kernptr,size)
#define KernCopyFromUser(kernptr,userptr,size)	copy_from_user(kernptr,userptr,size)
#define KernGetCurPid()                         (current->pid)

uint32_t KernGetEbps(uint32_t *ebpinfo);
uint32_t KernGetEsp(void);

/*
 * Kernel Locking Macros
 */
typedef spinlock_t KernBHLock;

#define KernBHLock_Declare(x)     KernBHLock x=SPIN_LOCK_UNLOCKED
#define KernLockBH(x)             spin_lock_bh(x)
#define KernUnLockBH(x)           spin_unlock_bh(x)
#define KernIsBHLocked(x)         spin_is_locked(x)



/*
 * Memory Related Macros
 */

#define KernMemCpy(dst,src,len)		memcpy(dst,src,len)
#define KernMemSet(ptr,byte,size)	memset(ptr,byte,(size_t)(size))
#define KernMemCmp(mem1,mem2,size)	memcmp(mem1,mem2,size)

/*
 * String Related Wrappers
 */

#define KernStrLen(str)			strlen(str)
#define KernStrCmp(str1,str2)		strcmp(str1,str2)
#define KernStrCpy(dst,src)		strcpy(dst,src)
#define KernStrNCmp(str1,str2,nbytes)	strncmp(str1,str2,nbytes)
#define KernStrCat(str1,str2)		strcat(str1,str2)
#define KernStrNCat(str1,str2,nbytes)	strncat(str1,str2,nbytes)
#define KernStrStr(haystack,needle)	strstr(haystack,needle)
#define KernStrDup(str)                 kstrdup(str, GFP_KERNEL)


/*
 *  Misc. Wrappers
 */

#ifndef LINUX_2_4_18_14
   #define KernDumpStack()                    do{                              \
                                                   dump_stack();               \
					        } while(0)
												   
#else
   #define KernDumpStack()                    do{                               \
				                 struct task_struct *task;      \
		                   	         task = current;                \
					 show_stack(&(task->thread.esp));\
				               } while(0)  		
#endif					       



#define KernPanic(str)			panic(str)

#define KernToLower(c)			tolower(c)

/*
 * Prototypes
 */

void*    Kerngetfreepages(size_t size);
void    Kernfreepages(void *ptr, size_t size);

struct tm* KernLocalTime(time_t *timep);

int32_t  KernTmGet(uint32_t *pdate, uint32_t *ptime, uint32_t *pticks);
time_t   KernTime(time_t *ptimer);
char* KernCTimeR(time_t * timer1, char *TimeBuf, int32_t *plen);

#endif
