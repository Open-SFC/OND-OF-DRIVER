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
 * File name: timer.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: Timer library.
 * 
 */

#ifdef CM_SUPPORT

/********************************************************
 * * * * * *  I n c l u d e   S e c t i o n   * * * * * *
 ********************************************************/
#include "cmincld.h"
#include "cmdefs.h"
#include "cmutil.h"
#include "tmrgdef.h"
#include "timer.h"

/********************************************************
 * * * * * *  G l o b a l   S e c t i o n   * * * * * *
 ********************************************************/

#define CM_ZERO_VALUE                 0

cm_timer_t *pUCMTimerHead[MAX_TIMER_LISTS];
uint32_t uistartup_G;
uint32_t uitimerTickVal_G;
uint32_t UCMtimerProcessInitialized = 0;
 uint32_t uiCurTickValue_g = 0;
#define MAX_STATIC_TIMERS 64
cm_timer_t UCMStaticTimers[MAX_STATIC_TIMERS];
cm_timer_t *pUCMFreeTimerListHead = NULL;
int32_t cm_insert_node (cm_timer_t * pTimer);
timer_statistics_t UCMtimerStatistics;

/********************************************************
 * F u n c t i o n   P r o t o t y p e   S e c t i o n*
 ********************************************************/

int32_t cm_delete_node (cm_timer_t * pTimer);

/********************************************************
 * F u n c t i o n   D e f i n i t i o n   S e c t i o n*
 ********************************************************/

/***********************************************************************
 * Function Name : TimerCreate                                         *
 * Description   : Allocates a timer node                              *
 * Input         : Pointer to return value                             *
 * Output        : Pointer to the newly created node                   *
 * Return value  : 0 or 1 implying O.K. or error respectively          *
 ***********************************************************************/

int32_t cm_timer_create (cm_timer_t ** pTimer)
{
  void *pVoid = NULL;

  if ((pVoid = cm_timer_alloc ()) == NULL)
  {
    UCMtimerStatistics.alloc_failures++;
    CM_DEBUG_PRINT ("returning in TimerCreate as NO_MEMORY");
    return OF_FAILURE;
  }

  *pTimer = pVoid;
  return OF_SUCCESS;
}

/***********************************************************************
 * Function Name : cm_timer_start                                     *
 * Description   : Starts the particular timer                         *
 * Input         : All the Node variables                              *
 * Output        : None                                                *
 * Return value  : 0 or 1 implying O.K. or error respectively          *
 ***********************************************************************/

int32_t cm_timer_start (cm_timer_t * pTimer, VOIDFUNPTR proutine, void *parg,
                         uint8_t timer_type_ui, uint32_t timer_value_ui,
                         uint32_t debug_info_ui)
{

  if (pTimer == NULL)
  {
    UCMtimerStatistics.invalid_timer_id++;
    return OF_FAILURE;
  }
  else if (timer_type_ui > MINS_TMR)
  {
    UCMtimerStatistics.invalid_timer_type++;
    CM_DEBUG_PRINT ("returning in cm_timer_start as ILLEGAL_TYPE");
    return (-1);
  }

  if (timer_value_ui == (uint32_t) NULL)
  {
    return 0;
  }

  if (cm_is_active_timer (pTimer, timer_type_ui) == TRUE)
  {
    UCMtimerStatistics.timer_id_present++;
//    CM_DEBUG_PRINT
   //  ("cm_delete_node called from cm_timer_start callptr %08x and type %d",
    // pTimer->proutine, timer_type_ui);
    if (cm_delete_node (pTimer) != OK)
      return -1;
  }
  pTimer->proutine = proutine;
  pTimer->parg = parg;
  pTimer->timer_type_ui = timer_type_ui;
  pTimer->timer_value_ui = timer_value_ui;
  pTimer->uiactive = FALSE;

  /*
   *Insert the current node.
   */
  if (cm_insert_node (pTimer))
  {
    UCMtimerStatistics.insert_errors++;
    CM_DEBUG_PRINT ("Error in cm_insert_node");
    return (-1);
  }

  return (OK);
}

/***********************************************************************
 * Function Name : cm_insert_node                                          *
 * Description   : Inserts a node in the  list                         *
 * Input         : Pointer to the node to be inserted                  *
 * Output        : None                                                *
 * Return value  : Returns OK. Error conditions are not checked here   *
 ***********************************************************************/

int32_t cm_insert_node (cm_timer_t * pTimer)
{
  cm_timer_t *pCurrentTimer = (cm_timer_t *) NULL;
  uint32_t uicumuTime = CM_ZERO_VALUE;
  uint8_t type;

  /*
   *Get the type of timer you are dealing with
   *and use it as a key for pointer manipulations on all
   *the four types of linked lists.
   */

  type = pTimer->timer_type_ui;

  /*
   *Lock the data structure here.
   */

  pTimer->uiactive = TRUE;

  /*
   *If this happens to be the first node.
   */

  if (pUCMTimerHead[type] == NULL)
  {
    pUCMTimerHead[type] = pTimer;
    pUCMTimerHead[type]->pnext = NULL;
    pUCMTimerHead[type]->pprev = NULL;
    pTimer->cur_tmr_count_ui = pTimer->timer_value_ui;

    //CM_DEBUG_PRINT ("Added the timer");
    return (OK);
  }
  else
  {
    pCurrentTimer = pUCMTimerHead[type];

    while (TRUE)
    {
      /*
       *If the total time is less than or equal to the cumulative time
       *then insert the node before the current node.
       */

      uicumuTime = uicumuTime + pCurrentTimer->cur_tmr_count_ui;
      if ((pTimer->timer_value_ui) <= uicumuTime)
      {
        pTimer->pnext = pCurrentTimer;
        pTimer->pprev = pCurrentTimer->pprev;
        if (pCurrentTimer->pprev)
        {
          pCurrentTimer->pprev->pnext = pTimer;
        }
        else
        {
          pUCMTimerHead[type] = pTimer;
        }
        pCurrentTimer->pprev = pTimer;
        pTimer->cur_tmr_count_ui = pTimer->timer_value_ui -
          (uicumuTime - (pCurrentTimer->cur_tmr_count_ui));
        pCurrentTimer->cur_tmr_count_ui = pCurrentTimer->cur_tmr_count_ui -
          pTimer->cur_tmr_count_ui;

        break;
      }

      /*
       *Put the node as the last one, if the current node is the
       *last one and the cumulative count is still less than the
       *total count.
       */

      if (pCurrentTimer->pnext == NULL)
      {
        pCurrentTimer->pnext = pTimer;
        pTimer->pnext = NULL;
        pTimer->pprev = pCurrentTimer;
        pTimer->cur_tmr_count_ui = pTimer->timer_value_ui - uicumuTime;
        break;
      }
      pCurrentTimer = pCurrentTimer->pnext;
    }

    return (OK);
  }
}

/***********************************************************************
 * Function Name : cm_delete_node                                          *
 * Description   : Deletes a node from the  list                       *
 * Input         : Pointer to the node to be deleted                   *
 * Output        : None                                                *
 * Return value  : Returns OK. Error conditions are not checked here   *
 ***********************************************************************/

int32_t cm_delete_node (cm_timer_t * pTimer)
{
  uint8_t type;

  if (pTimer == NULL)
    return OF_FAILURE;

  type = pTimer->timer_type_ui;

  pTimer->uiactive = FALSE;

  if (pTimer->pnext)
  {
    pTimer->pnext->cur_tmr_count_ui =
      pTimer->pnext->cur_tmr_count_ui + pTimer->cur_tmr_count_ui;
    pTimer->pnext->pprev = pTimer->pprev;
  }
  if (pTimer->pprev)
  {
    pTimer->pprev->pnext = pTimer->pnext;
  }
  else
  {
    pUCMTimerHead[type] = pTimer->pnext;
  }

  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_stop                                           *
 * Description   : Stops an active timer in the  list                  *
 * Input         : Pointer to the node to be stopped                   *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_timer_stop (cm_timer_t * pTimer)
{

  //CM_DEBUG_PRINT ("cm_timer_stop entry");

  if (cm_is_active_timer (pTimer, pTimer->timer_type_ui))
  {
    if (cm_delete_node (pTimer))
    {
      CM_DEBUG_PRINT ("returning in cm_timer_stop as TMR_STP_ERR");
      return OF_FAILURE;
    }
  }
  else
  {
    UCMtimerStatistics.not_present_error++;
  }
  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_delete                                         *
 * Description   : Stops an active timer and frees the memory          *
 * Input         : Pointer to the node to be deleted                   *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_timer_delete (cm_timer_t * pTimer)
{
  if (pTimer->uiactive == FALSE)
  {
    /** of_free(pTimer); **/
    cm_timer_free (pTimer);
    return OF_FAILURE;
  }

  if (cm_timer_stop (pTimer))
  {
    /* of_free(pTimer); */
    cm_timer_free (pTimer);
    return OF_FAILURE;
  }
  /*
   *Freeing the memory segment which has been acquired earlier.
   */

  /* of_free(pTimer); */
  cm_timer_free (pTimer);
  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_restart                                        *
 * Description   : Stops an active timer and starts it again           *
 * Input         : Pointer to the node to be restarted                 *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_timer_restart (cm_timer_t * pTimer)
{
  int32_t istatus;

  istatus = cm_timer_start (pTimer, pTimer->proutine, pTimer->parg,
                             pTimer->timer_type_ui, pTimer->timer_value_ui,
                             pTimer->debug_info_ui);
  if (istatus != OK)
  {
    return OF_FAILURE;
  }
  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_restart_with_new_value                            *
 * Description   : Restarts a timer with new value                     *
 * Input         : Pointer to the node to be restarted                 *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_timer_restart_with_new_value (cm_timer_t * pTimer, uint32_t uinewValue)
{
  int32_t istatus;

  if ((cm_is_active_timer (pTimer, pTimer->timer_type_ui)) &&
      (cm_timer_stop (pTimer)))
  {
    return OF_FAILURE;
  }

  /*
   *  Initialize the duration of the timer to the new value passed
   *  and re-start the timer
   */
  pTimer->timer_value_ui = uinewValue;
  istatus = cm_timer_start (pTimer, pTimer->proutine, pTimer->parg,
                             pTimer->timer_type_ui, pTimer->timer_value_ui,
                             pTimer->debug_info_ui);
  if (istatus != OK)
  {
    return OF_FAILURE;
  }
  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_alloc 
 * Description   : if pUCMFreeTimerListHead list is NULL then it allocates
 *        dynamically, otherwise it will get the first node from
 *        pUCMFreeTimerListHead. 
 * Input         : none. 
 * Output        :
 * Return value  : pointer to newly allocated node on success , otherwise NULL.
 ***********************************************************************/

cm_timer_t *cm_timer_alloc (void)
{
  cm_timer_t *pTimer = NULL;

  if (pUCMFreeTimerListHead == NULL)
  {
    UCMtimerStatistics.dynamic_alloc_hits_ul++;
    pTimer = (cm_timer_t *) of_calloc (1, sizeof (cm_timer_t));
    if (pTimer == NULL)
    {
      UCMtimerStatistics.dynamic_alloc_fails_ul++;
      return (NULL);
    }
    pTimer->b_dynamic = TRUE;
  }
  else
  {
    UCMtimerStatistics.static_alloc_hits_ul++;
    pTimer = pUCMFreeTimerListHead;
    pUCMFreeTimerListHead = pTimer->pnext;
    memset ((char *) pTimer, 0, sizeof (cm_timer_t));
    pTimer->b_dynamic = FALSE;
  }
  return (pTimer);
}

/***********************************************************************
 * Function Name : cm_timer_free
 * Description   : If the node allocated dynamically it will free the 
 *        memory occupied by it, otherwise it will keep it in 
 *        pUCMFreeTimerListHead list. 
 * Input         : pTimer -- pointer to cm_timer_t.                       
 * Output        : 
 * Return value  : none.
 ***********************************************************************/

void cm_timer_free (cm_timer_t * pTimer)
{

  if (pTimer == NULL)
    return;
  if (pTimer->b_dynamic == TRUE)
  {
    UCMtimerStatistics.dynamic_alloc_frees_ul++;
    free (pTimer);
  }
  else
  {
    UCMtimerStatistics.static_alloc_frees_ul++;
    pTimer->pnext = pUCMFreeTimerListHead;
    pUCMFreeTimerListHead = pTimer;
  }
}

/***********************************************************************
 * Function Name : TimerInit                                           *
 * Description   : Initializes data structures and brings up timer task*
 * Input         : None                                                *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_timer_init (void)
{
  uint32_t uitmr = CM_ZERO_VALUE;
  int32_t ii = CM_ZERO_VALUE;

  uistartup_G = CM_ZERO_VALUE;
  uitimerTickVal_G = CM_ZERO_VALUE;

  for (ii = MAX_STATIC_TIMERS - 1; ii >= CM_ZERO_VALUE; ii--)
  {
    UCMStaticTimers[ii].b_dynamic = FALSE;
    cm_timer_free (&UCMStaticTimers[ii]);
  }
  memset ((char *) & UCMtimerStatistics, 0, sizeof (timer_statistics_t));

  /*
   *Initialize the linked lists catering to different types
   *of timers.
   */

  for (uitmr = CM_ZERO_VALUE; uitmr < MAX_TIMER_LISTS; uitmr++)
  {
    pUCMTimerHead[uitmr] = NULL;
  }

  UCMtimerProcessInitialized = 1;

  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_get_remaining_time                               *
 * Description   : Gets the remaining time to expire for the timer Id  *

 * Input         :                                                     *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_timer_get_remaining_time (cm_timer_t * pTimer,
                                    uint32_t * pgetRemngTime)
{
  cm_timer_t *pCurrentTimer = NULL;
  uint32_t uicumuTime = CM_ZERO_VALUE;

  /* for inactive node prev ptr might be pointing to garbage  -pravin */
  if (!pTimer->uiactive)
  {
    *pgetRemngTime = 0;
    return TMR_INACTIVE;
  }

  pCurrentTimer = pTimer;

  while (TRUE)
  {
    uicumuTime = uicumuTime + pCurrentTimer->cur_tmr_count_ui;
    if (pCurrentTimer->pprev == NULL)
    {
      *pgetRemngTime = uicumuTime;
      break;
    }
    pCurrentTimer = pCurrentTimer->pprev;
  }
  return (OK);
}

/***************************************************************************
 * Function Name : cm_is_active_timer
 * Description   : Returns TRUE if the timer is active; FALSE otherwise
 * Input         : cm_timer_t* -- pointer to the timer object
                   uint8_t  -- timer type
 * Output        : None
 * Return value  : TRUE if timer is active; FALSE otherwise
 ***************************************************************************/

unsigned char cm_is_active_timer (cm_timer_t * pTimer, uint8_t timer_type_ui)
{
  cm_timer_t *pCurrentTimer = pUCMTimerHead[timer_type_ui];

  if (pTimer == NULL)
    return FALSE;

  while (pCurrentTimer != NULL)
  {
    if (pCurrentTimer == pTimer)
      break;

    pCurrentTimer = pCurrentTimer->pnext;  /** SRINI - SEP 19 **/
  }

  if (pCurrentTimer == NULL)
    return FALSE;
  else
    return TRUE;
}

/***************************************************************************
 * Function Name : cm_timer_duration
 * Description   : returns the duration of the timer
 * Input         : cm_timer_t* -- pointer to the timer object
 * Output        : None
 * Return value  : uint32_t -- Duration of the timer
 ***************************************************************************/

uint32_t cm_timer_duration (cm_timer_t * pTimer)
{
  if (pTimer == NULL)
    return OF_FAILURE;
  return pTimer->timer_value_ui;
}

/***************************************************************************
 * Function Name : cm_timer_get_callback_fn
 * Description   : returns the callback function pointer of the timer
 * Input         : cm_timer_t* -- pointer to the timer object
 * Output        : None
 * Return value  : VOIDFUNPTR -- Callback function of the timer
 ***************************************************************************/

VOIDFUNPTR cm_timer_get_callback_fn (cm_timer_t * pTimer)
{
  if (pTimer == NULL)
    return NULL;
  return pTimer->proutine;
}

/***************************************************************************
 * Function Name : cm_timer_get_callback_arg
 * Description   : returns the argument to callback function of the timer
 * Input         : cm_timer_t* -- pointer to the timer object
 * Output        : None
 * Return value  : void* -- argument to Callback function of the timer
 ***************************************************************************/

void *cm_timer_get_callback_arg (cm_timer_t * pTimer)
{
  if (pTimer == NULL)
    return NULL;
  return pTimer->parg;
}

/***************************************************************************
 * Function Name : cm_timer_set_callback_arg
 * Description   : sets the argument to callback function of the timer
 * Input         : cm_timer_t* -- pointer to the timer object
                   void*    -- argument
 * Output        : None
 * Return value  : None
 ***************************************************************************/

void cm_timer_set_callback_arg (cm_timer_t * pTimer, void * parg)
{
  pTimer->parg = parg;
}

/***********************************************************************
 * Function Name : cm_time_out_action_handler                           *
 * Description   : Calls the required routine after the time out       *
 * Input         :                                                     *
 * Output        : None                                                *
 * Return value  : Returns Error code or OK (PASS)                     *
 ***********************************************************************/

int32_t cm_time_out_action_handler (cm_timer_t * pTimer, void * arg)
{
  /*
   *The required function pointer had already been initialized
   *at the time of starting the timer.
   */

  if (pTimer->proutine == NULL)
  {
    return OF_FAILURE;
  }
  pTimer->proutine (arg);
  return (OK);
}

/***********************************************************************
 * Function Name : cm_timer_process
 * Description   : if timer process initialized, it calls cm_timer_node_update. 
 * Input         :                                                     
 * Output        :
 * Return value  :
 ***********************************************************************/

void cm_timer_process (uint32_t units)
{
  if (UCMtimerProcessInitialized == 0)
    return;
  cm_timer_node_update (pUCMTimerHead[SECS_TMR], units);
}

/***********************************************************************
 * Function Name : cm_timer_process_nodes                            *
 * Description   : Waits for an event from ISR and processes it        *
 * Input         : Elapsed time in Milli Secs                          *
 * Output        : None                                                *
 * Return value  : None                                                *
 ***********************************************************************/
int32_t cm_timer_process_nodes(uint32_t ulTimeOutPassedInMsec)
{
  static uint32_t ulTimeMilliSecs = 0; 
//  static uint32_t ulTimeSecs = 0;
  cm_timer_t *pcurrent;
//  cm_timer_t *headPtr = NULL;
//  cm_timer_t *tailPtr = NULL;

  ulTimeMilliSecs += ulTimeOutPassedInMsec;
  uiCurTickValue_g += (ulTimeOutPassedInMsec * get_ticks_2_sec())/1000;
  pcurrent = pUCMTimerHead[MILLI_SECS_TMR];
  cm_timer_node_update(pcurrent, ulTimeOutPassedInMsec);    
#if 0
  /*
   *Update the Seconds Timer List here.
   */

  while (ulTimeMilliSecs >= ONE_THOUSAND_MILLI)
  {
    ulTimeSecs = ulTimeSecs + 1;
    ulTimeMilliSecs = ulTimeMilliSecs - ONE_THOUSAND_MILLI;
    pcurrent = pUCMTimerHead [SECS_TMR];
    TimerNodeUpdate(pcurrent, 1);    

    pcurrent = pUCMTimerHead [PERIODIC_TMR];
    if (pcurrent)
    {
      pcurrent->cur_tmr_count_ui = pcurrent->cur_tmr_count_ui - 1;
      while (TRUE)
      {
        if (pcurrent->cur_tmr_count_ui == 0) 
        {
          if (headPtr == NULL)
          {
            headPtr = pcurrent;
            headPtr->pprev = NULL;
            tailPtr = headPtr;
          }
          else
          {
            tailPtr->pnext = pcurrent;
            pcurrent->pprev = tailPtr;
            tailPtr = pcurrent;
          }
        }
        else
        { 
          break;
        }
        if (pcurrent->pnext == NULL) 
        {
           break;
        }
        pcurrent = pcurrent->pnext;
      }

      /*
       *Start invoking the functions from the list.
       */
      while (TRUE)
      {
        if (headPtr == NULL)
        {
          break;
        }
        else
        {
          pcurrent = headPtr;
          headPtr->proutine (headPtr->parg);
          headPtr = headPtr->pnext;
          cm_timer_restart(pcurrent);
        }
      }
    }
  }

  /*
   *Update the Minutes Timer List here.
   */
  while (ulTimeSecs >= SIXTY_SECS)
  {
    ulTimeSecs -= SIXTY_SECS;
    pcurrent = pUCMTimerHead [MINS_TMR];
    cm_timer_node_update(pcurrent, 1);    
  }
#endif	
  return OF_SUCCESS;
}
/***********************************************************************
 * Function Name : cm_timer_node_update
 * Description   : If given timerid current timer count is grater than 
 *        given actual time , then sutract actual time from 
 *        timerid current timer count, otherwise sutract given 
 *        current timer count from all the nodes current timer 
 *        count as long as on sutraction their current timer count
 *        becomes zero and deleate all such nodes.
 * Input         : pTimer -- pointer to cm_timer_t.
 *        uiactualTime -- actual time with which we want to update.
 * Output        :
 * Return value  : none.
 ***********************************************************************/

void cm_timer_node_update (cm_timer_t * pTimer, uint32_t uiactualTime)
{
  cm_timer_t *pTimerHead = NULL;
  uint32_t timer_type_ui = CM_ZERO_VALUE, ulRemTime = CM_ZERO_VALUE;

  pTimerHead = pTimer;

  if (!pTimerHead)
    return;

  timer_type_ui = pTimer->timer_type_ui;

  if (pTimerHead->cur_tmr_count_ui > uiactualTime)
  {
    pTimerHead->cur_tmr_count_ui -= uiactualTime;
    return;
  }
  else
  {
    ulRemTime = MIN (uiactualTime, pTimerHead->cur_tmr_count_ui);
    pTimerHead->cur_tmr_count_ui -= ulRemTime;
    uiactualTime -= ulRemTime;
  }

  while (pTimerHead->cur_tmr_count_ui == 0)
  {
    cm_delete_node (pTimerHead);
    CM_DEBUG_PRINT ("the call back called\r\n");
    pTimerHead->proutine (pTimerHead->parg);
    pTimerHead = pUCMTimerHead[timer_type_ui];
    if (!pTimerHead)
      break;
    ulRemTime = MIN (uiactualTime, pTimerHead->cur_tmr_count_ui);
    pTimerHead->cur_tmr_count_ui -= ulRemTime;
    uiactualTime -= ulRemTime;
  }
}

/***********************************************************************
 * Function Name : cm_get_first_timer_timeout_val
 * Description   : From timer nodes gets the first node value.
 * Input         : none                                           
 * Output        : 
 * Return value  : If there exists a node its value otherwise -1
 ***********************************************************************/

int32_t cm_get_first_timer_timeout_val ()
{
  if (pUCMTimerHead[SECS_TMR])
    return pUCMTimerHead[SECS_TMR]->cur_tmr_count_ui;
  else
    return -1;
}

/***********************************************************************
 * Function Name : GetCurrentTickValue                                 *
 * Description   : To get the ticks value.                             *
 * Input         : NONE                                                *
 * Output        : None                                                *
 * Return value  : uicurrTickVal                                       *
 ***********************************************************************/
uint32_t cm_get_current_tick_value()
{
  uint32_t uiospl;
  uint32_t uicurrTickVal;

  uiospl = of_spl_high();
  uicurrTickVal = uiCurTickValue_g; 
  of_splx(uiospl);
  return (uicurrTickVal);
}

/* i don't want to access timer internals thru' arp for server monitoring */
void cm_timer_call_out (cm_timer_t * pTimer)
{
  pTimer->proutine (pTimer->parg);
}

#endif /* CM_SUPPORT */
