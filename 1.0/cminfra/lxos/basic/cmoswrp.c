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
 * File name: ucmoswrp.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: This file contains wrapper functions for all the Linux OS, and 'C'
 *              library specfic function calls.
*/

#ifndef _OSWRP_C
#define _OSWRP_C

#include "cmincld.h"
#include<unistd.h>
#include<fcntl.h>

char *of_fgets(char *s, int32_t size, FILE *stream);
void *of_gm_time(const time_t *pTime , struct tm *pTm);
uint32_t of_spl_high(void);
uint32_t of_splx(uint32_t ulSpl);
uint32_t get_ticks_2_sec(void);
int32_t get_ticks_per_sec(uint32_t *tickpersec);
time_t of_time(time_t *pTimer);
void protect_enable(unsigned char flag, uint32_t * taskOldMode,
                   uint32_t * intOldMode);
void protect_disable(unsigned char flag,
                    uint32_t taskOldMode,
                    uint32_t intOldMode);
int32_t daemon_init (void);
int32_t cm_get_time_of_day(time_t *pTime, char *pTimeBuff);

/****************************************************************************
* Function definitions:
****************************************************************************/
#ifndef USE_DMALLOC
void *of_malloc (size_t size)
{
  return malloc (size);
}

void *of_realloc (void * ptr, size_t size)
{
  return (void *) (realloc (ptr, size));
}

void of_free (void * ptr)
{
  if(ptr != NULL)
   free (ptr);
}

char *of_fgets(char *s, int32_t size, FILE *stream)
{
	 return( fgets(s,size,stream));
}
void *of_calloc (size_t nmemb, size_t size)
{
  return calloc (nmemb, size);
}

void *of_memcpy (void * dest, const void * source, uint32_t length)
{
  return (void
          *) (memcpy ((void *) dest, (const void *) source,
                      (unsigned int) length));
}

void *of_memset (void * s, int32_t c, int32_t n)
{
  return (void *) (memset ((void *) s, (int) c, (size_t) n));
}

/* String related routines */
int32_t of_strlen (const char * s)
{
  return strlen (s);
}

int32_t of_strcmp (const char * s1, const char * s2)
{
  return (strcmp (s1, s2));
}

void *of_gmtime(const time_t *pTime , struct tm *pTm)
{
  struct tm  *pTemp;
 
  pTemp = gmtime(pTime);
  if(!pTemp) return ;
  of_memcpy(pTm,pTemp,sizeof(struct tm));
 
  return;
}

char *of_strcpy (char *dest, const char *src)
{
  return strcpy (dest, src);
}

char *of_strdup (char *s)
{
  return strdup (s);
}

int32_t of_memcmp (const void * s1, const void * s2, int32_t n)
{
  return (int32_t) memcmp (s1, s2, (size_t) n);
}

int32_t of_strnicmp (char * s1, char * s2, int32_t n)
{
  return strncasecmp (s1, s2, n);
}

int32_t of_stricmp (char * s1, char * s2)
{
  return strcasecmp (s1, s2);
}

char *of_strnchr (char * str_p, unsigned char chr_uc, uint32_t length)
{
  uint32_t uiCurPos;
  for (uiCurPos = 0; (uiCurPos < length) && (*str_p != chr_uc);
       str_p++, uiCurPos++);
  if (uiCurPos == length)
    return NULL;
  return str_p;
}

int32_t of_strcasecmp (const char * s1, const char * s2)
{
  char a1;
  char b1;

  if (s1 == NULL || s2 == NULL)
    return -1;
  do
  {
    a1 = *s1;
    b1 = *s2;

    a1 = tolower (a1);
    b1 = tolower (b1);

    if (a1 > b1)
      return 1;

    if (a1 < b1)
      return -1;

    s1++;
    s2++;
  }
  while (a1 && b1);

  if (a1 == '\0' && b1 == '\0')
  {
    return 0;
  }
  if (a1 == '\0')
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

int32_t of_strncmp (const char * s1, const char * s2, size_t n)
{
  return (uint32_t) strncmp (s1, s2, n);
}

char *of_strcat (char * s1, const char * s2)
{
  return (char *) (strcat (s1, s2));
}

int32_t of_strncasecmp (const char * s1, const char * s2, size_t n)
{
  return (int32_t) strncasecmp (s1, s2, n);
}

char *of_strstr (const char * haystack, const char * needle)
{
  return strstr (haystack, needle);
}

/* case insensitive pattern search */
char *of_stristr (const char *String, const char *pattern)
{
  char *pptr, *sptr, *start;

  for (start = (char *) String; *start != 0; start++)
  {
    /* find start of pattern in string */
    for (; ((*start != 0) && (toupper (*start) != toupper (*pattern))); start++)
      ;
    if (0 == *start)
      return NULL;

    pptr = (char *) pattern;
    sptr = (char *) start;

    while (toupper (*sptr) == toupper (*pptr))
    {
      sptr++;
      pptr++;

      /* if end of pattern then pattern was found */

      if (0 == *pptr)
        return (start);
    }
  }
  return NULL;
}

char *of_strchr (const char * p, uint8_t c)
{
  return strchr (p, c);
}

char *of_strtok (char * p, const char * c)
{
  return strtok (p, c);
}

char *of_strrchr (const char * p, uint8_t c)
{
  return strrchr (p, c);
}

char *of_strncpy (char * s1, const char * s2, size_t n)
{
  return strncpy (s1, s2, n);
}

int32_t of_strtol (const char * nptr, char ** endptr, int32_t base)
{
  return (uint32_t) strtol (nptr, endptr, base);
}

uint32_t of_strtoul (const char * nptr, char ** endptr, int32_t base)
{
  return strtoul (nptr, endptr, base);
}

char *of_strncat (char * dest, const char * src, size_t n)
{
  return strncat (dest, src, n);
}

/* Miscellaneous */

int32_t of_isxdigit (int c)
{
  return isxdigit (c);
}

uint32_t of_rand (void)
{
  return rand ();
}

int32_t of_atoi (const char *nptr)
{
  return (atoi (nptr));
}

int32_t of_atol (const char * s)
{
  return ((int32_t) atol (s));
}

int64_t of_atoll (const char * s)
{
  return ((int64_t) atoll (s));
}

uint8_t of_atox(char *cPtr)
{
  uint8_t ucbyte = 0;
  char str[] = "0123456789abcdef";
  unsigned int digit;


  while (*(cPtr))
  {
     for (digit = 0; digit < (strlen(str)); digit++)
     {
        if (str[digit] == *(cPtr))
        {
           if (!(*(cPtr+1)))
              ucbyte += digit;
           else
              ucbyte += 16*digit;

           break;
        }
     }
     cPtr++;
  }

  return ucbyte;
}


/*void exit(int32_t status)
{
  exit(status);
}*/

unsigned char *of_ctime (time_t * t)
{
  return (ctime (t));
}

void of_sleep_ms (int16_t k)
{
  if (k / 1000 == 0)
    sleep (1);
  else
    sleep (k / 1000);
}

uint32_t of_spl_high(void)
{
  return OF_SUCCESS;
}

uint32_t of_splx(uint32_t ulSpl)
{
  return OF_SUCCESS;
}
/***************************************************************************
 * Function Name : get_ticks_2_sec
 * Description   : It returns CM_TICKS2SEC
 * Input         : None
 * Output        : None
 * Return value  : CM_TICKS2SEC
 ***************************************************************************/

uint32_t get_ticks_2_sec(void)
{
   /* this function should return the system clk interrupt frequency.
    * like sysClkRateGet function provided by vx
    */
   uint32_t iHz;
   get_ticks_per_sec(&iHz);
   return iHz;
}
/*
 * This function returns the number of ticks per sec
 */
int32_t get_ticks_per_sec(uint32_t *tickpersec)
{
   *tickpersec = sysconf(_SC_CLK_TCK);
   return OF_SUCCESS; 
}

/*****************************************************************************
 * Function: of_time
 * Input: pointer to a time_t structure
 * Output: 'timer' filled in" time returns the time since the Epoch (00:00:00 UTC, January 1,
 * 1970), measured in seconds." The function TZUpdateZoneInfo() syncs the calling
 * processes idea of timezone with that of iGateway.
 * ******************************************************************************/
time_t of_time(time_t *pTimer)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);	

  if(pTimer)	 
  {
    *pTimer = tv.tv_sec;
  }

  return tv.tv_sec;
}

/***************************************************************************
 * Function Name : protect_enable
 * Description   :
     * Enables the protection of the global variables by increasing the
     * priority of the task and locking the task. Stores the old values
     * in the passed parameters.
 * Input         : unsigned char      -- Flag which indicates whether the process
                                    is interrupt driven or not
 * Output        : uint32_t*     -- Task's old mode
                   uint32_t*     -- Task's old priority
 * Return value  : None
 ***************************************************************************/
void protect_enable(unsigned char flag, uint32_t * taskOldMode,
                   uint32_t * intOldMode)
{
  /* Dummy function for message.c file */
  return;
}

/***************************************************************************
 * Function Name : protect_disable
 * Description   :
     * Disables protection by restoring the old priority and unlocking
     * the task.
 * Input         : unsigned char     -- Process attributes
                   uint32_t     -- Task's old mode
                   uint32_t     -- Task's old priority
 * Output        : None
 * Return value  : None
 ***************************************************************************/
void protect_disable(unsigned char flag,
                    uint32_t taskOldMode,
                    uint32_t intOldMode)
{
  /* Dummy function for message.c file */
   return;
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        : None
 * Return value  : None
 ***************************************************************************/

int32_t daemon_init (void)
{
  int devnullfd, return_value = OF_SUCCESS;
  return_value = chdir ("/");
  switch (fork ())
  {
    case -1:
      printf ("daemon_init: error in fork\r\n");
      exit (1);
    case 0:
           /** Child **/
      break;
    default:
         /** Parent **/
      exit (0);
  }
  printf (" In daemon_init() ");
  if (setsid () == -1)
  {
    printf ("daemon_init: setsid error\r\n");
    exit (1);
  }

  devnullfd = open ("/dev/null", O_RDWR);
  if (devnullfd == -1)
    return (1);

  close (0);
  close (1);
  close (2);

  dup2 (devnullfd, 0);
  dup2 (devnullfd, 1);
  dup2 (devnullfd, 2);
  close (devnullfd);
  return (1);
}

/***************************************************************************
 * Function Name : 
 * Description   :
 * Input         : 
 * Output        : None
 * Return value  : None
 ***************************************************************************/
int32_t cm_get_time_of_day(time_t *pTime, char *pTimeBuff)
{
	 struct timeval time;

	 gettimeofday(&time, NULL);
	 if(pTimeBuff)	
	 {		 
			sprintf(pTimeBuff, "%26ld", time.tv_sec);
			return OF_SUCCESS;
	 }

   return OF_FAILURE;
}

uint64_t num_value(char ch)
{
  if (ch >= '0' && ch <= '9') return ch-'0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a'+10;
  if (ch >= 'A' && ch <= 'F') return ch - 'A'+10;
  return OF_FAILURE;
}
uint64_t char_To64bitNum(char *p)
{
  uint64_t res=0;
  while(*p)
  {
    res<<=4;
    res|= num_value(*p);
    p++;
          }
  return res;
}

#endif /* USE_DMALLOC */
#endif /* _OSWRP_C */
