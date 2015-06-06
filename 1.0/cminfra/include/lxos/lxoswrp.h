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
 * File name: lxoswrp.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains 'C' language specific wrapper
 *              types, Wrapper function declarations for 'C' 
 *              library function calls, and Linux OS specific 
 *              function calls.
 * 
 */

#ifndef LXOS_WRAP_H
#define LXOS_WRAP_H


typedef ulong t_sem_id_t;
typedef uint32_t semaphore_t;

#ifndef NULL
#define NULL  ((void*)0)
#endif

/*Access Permissions */
#define T_S_IRWXU  0x01c0   /* read/write/execute permission, owner */
#define T_S_IRWXG  0x0038   /* read/write/execute permission, group */
#define T_S_IRWXO  0x0007   /* read/write/execute permission, other */

/* 
   Fixme : The following function name mappings should removed.
   We have to have LXOS wrapper function declaration 
*/

#if !defined(__KERNEL__) && defined(USE_DMALLOC)
#define of_malloc malloc
#define of_free free
#define of_realloc realloc
#define of_calloc calloc
#define of_memset memset
#define of_strlen strlen
#define of_strcmp strcmp
#define t_str_i_cmp stricmp
#define of_strncmp strncmp
#define of_strcpy strcpy
#define t_mem_cmp memcmp
#define of_memcpy memcpy
#define of_strncpy strncpy
#define of_strcat strcat
#define of_strncat strncat
#define t_str_case_cmp strcasecmp
#define t_str_ncase_cmp strncasecmp
#define of_strchr strchr
#define of_strtok strtok
#define of_strrchr strrchr
#define of_strdup strdup
#define of_isxdigit isxdigit
#define t_rand rand
#define of_atol atol	
#define of_atoll atoll
#define t_ctime ctime
#define of_fgets fgets
#define T_gmtime gmtime_r
#define of_sleep_ms(k) (k/1000 == 0)? sleep(1) : sleep(k/1000)

#else

void of_sleep_ms (int16_t k);
unsigned char *t_ctime (time_t * t);
int32_t of_atol (const char * s);
int64_t of_atoll (const char *nptr);
uint8_t of_atox(char *cPtr);
uint32_t t_rand (void);
char *of_strncat (char * dest, const char * src, size_t n);
uint32_t t_strtoul (const char * nptr, char ** endptr, int32_t base);
int32_t of_strtol (const char * nptr, char ** endptr, int32_t base);
char *of_strncpy (char * s1, const char * s2, size_t n);
char *of_strrchr (const char * p, uint8_t c);
char *of_strchr (const char * p, uint8_t c);
char *of_strtok (char * p, const char * c);
char *t_str_istr (const char *String, const char *pattern);
char *of_strstr (const char * haystack, const char * needle);
int32_t t_str_ncase_cmp (const char * s1, const char * s2, size_t n);
char *of_strcat (char * s1, const char * s2);
int32_t of_strncmp (const char * s1, const char * s2, size_t n);
int32_t t_str_case_cmp (const char * s1, const char * s2);
char *t_str_nchr (char * str_p, unsigned char chr_uc, uint32_t length);
int32_t t_str_i_cmp (char * s1, char * s2);
int32_t t_str_ni_cmp (char * s1, char * s2, int32_t n);
int32_t t_mem_cmp (const void * s1, const void * s2, int32_t n);
char *of_strcpy (char *dest, const char *src);
int32_t of_strcmp (const char * s1, const char * s2);
int32_t of_strlen (const char * s);
void *of_memset (void * s, int32_t c, int32_t n);
void *of_malloc (size_t size);
void of_free (void * ptr);
void *of_realloc (void * ptr, size_t size);
void *of_calloc (size_t nmemb, size_t size);
int32_t of_atoi (const char *nptr);
void *of_memcpy (void * dest, const void * source, uint32_t length);
char *of_strdup (char *s);
int32_t of_isxdigit (int c);
uint64_t char_To64bitNum(char *p);
//void *T_gmtime(const time_t* const timep, struct tm* mytm);
#endif

#define signal signal
#define sigaddset sigaddset
#define sigaction sigaction
#define sigfillset sigfillset
//#define sigsetjmp sigsetjmp
#define alarm alarm

#define NULLCHAR  ((char *)0)

#define T_SEM_FIFO      0x01
#define T_SEM_PRIORITY  0x02
#define T_SEM_WAIT      0x04
#define T_SEM_NOWAIT    0x08

#define T_SEM_FULL      0
#define T_SEM_EMPTY   0
#define T_SEM_LOCAL   0
#define T_SEM_GLOBAL  0

#define T_pthread_mutex_t pthread_mutex_t
#define T_PTHREAD_MUTEX_INITIALIZER  PTHREAD_MUTEX_INITIALIZER
#define T_pthread_mutex_lock pthread_mutex_lock
#define T_pthread_mutex_unlock pthread_mutex_unlock
#define T_pthread_mutex_init  pthread_mutex_init
#define T_pthread_rwlock_init(x,y)
#define T_pthread_rwlock_t pthread_rwlock_t

#endif /* LXOS_WRAP_H */
