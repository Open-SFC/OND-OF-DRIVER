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
 * File name: timerlib.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: Timer library, provides the timer management           
 *              functionality to process/tasks/threads. Currently it is
 *              NOT a thread safe library. But changes can be done to  
 *              make it thread safe. It is a shared library as its APIs
 *              can be invoked simultaneously by multiple threads/tasks
 *              each operating on different library instance(context).
 *
 * Currently this library supports only the seconds timers. But it can be 
 * easily extended to support other timer types. There is no need to change
 * the API interface. All changes that are required are internal to this
 * library module.
 *
 * The timer nodes are opaque to the user. So the user should always
 * allocate the timer node memory using UCMlibTimerCreateTimer API.
 * User module cannot have the timer node structure as a member field in 
 * their own structures.
 * 
 */

#include "cmincld.h"
#include "cmdll.h"
#include "cmmempol.h"
#include "tmrlgdef.h"
#include "tmrlgif.h"
#include "tmrlldef.h"
#include "tmrlblif.h"



/*****************************************************************************/
/* Function        : cm_lib_timer_init                                         */
/* Description     : This API initializes the timer library context.Rach user*/
/*                   module (task/thread/process) should call this API during*/
/*                   their initialization phase. The library context pointer */
/*                   returned by this API should be stored by the user module*/
/*                   as this needs to be passed as an argument to the        */
/*                   remaining APIs of this library.                         */
/* Input Arguments :                                                         */
/*    pInitInfo      -  pointer to library initialization info.              */
/* Output Arguments:                                                         */
/*    ppTimerLibCtxt -  pointer to memory to hold the library context pointer*/
/* Return Value    : OF_SUCCESS - on successful initialization of library     */
/*                               instance. pointer to library context is     */
/*                               returned in output argument ppTimerLibCtxt  */ 
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_init(
                          UCMLibTimerLibInitInfo_t  *pInitInfo,
                          void                    **ppTimerLibCtxt
                        )  
{
  UCMLibTimerCtxt_t  *pCtxt = NULL;  
  int32_t  lRetVal = OF_SUCCESS;
  uint32_t ulStaticTimers;
  uint32_t ulTotalTimers;

  do
  {
   /*
    * Sanity checks
    */
    if( (pInitInfo == NULL) || (ppTimerLibCtxt == NULL) )
    {
      lRetVal = CM_LIB_TIMER_ERR_INVALID_ARGUMENT;
      break;
    }
 
    if( pInitInfo->ulLockType != CM_LIBTMR_NO_LOCK )
    {
      lRetVal = CM_LIB_TIMER_ERR_INVALID_LOCK_TYPE;
      break;
    }

    ulStaticTimers = pInitInfo->ulStaticTimers;
    ulTotalTimers = pInitInfo->ulTotalTimers;

    if( ulTotalTimers == 0 )
    {
      lRetVal = CM_LIB_TIMER_ERR_INVALID_TOTAL_TIMERS;
      break;
    }

    if( ulStaticTimers > ulTotalTimers )
      ulTotalTimers = ulStaticTimers;

   /*
    * allocate memory for library instance
    */
    pCtxt = (UCMLibTimerCtxt_t*) of_calloc( 1, sizeof(UCMLibTimerCtxt_t) );
    if( pCtxt == NULL )
    {
      lRetVal = CM_LIB_TIMER_ERR_MEM_ALLOC_FAIL;
      break;
    }

   /*
    * create memory pool for timer nodes.
    */
    pCtxt->pMemPool = cm_create_mempool(ulStaticTimers,
                                       sizeof(UCMLibTimer_t),
                                       ulTotalTimers,
                                       FALSE);
                                   /*  TODO: next release  "TimerLibPool");*/
    if( pCtxt->pMemPool == NULL )
    {
      lRetVal = CM_LIB_TIMER_ERR_MEM_ALLOC_FAIL;
      break;
    }
       
   /*
    * Initialize the library instance context
    */             
#ifdef CM_LIB_TIMER_DEBUG                   
    CM_LIB_TIMER_INIT_TIMER_CTXT_PATTERN_FIELDS(pCtxt);
#endif
    pCtxt->ulMaxStaticTimers = ulStaticTimers;
    pCtxt->ulMaxTimers = ulTotalTimers;
    pCtxt->ulFlag = 0;

   /*
    * Initialize the active timer node (double linked) list.
    */
    CM_DLLQ_INIT_LIST(&pCtxt->ActiveHead);   

   /*
    * Initialize the passive timer node (double linked) list.
    */
    CM_DLLQ_INIT_LIST(&pCtxt->PassiveHead);   

   /*
    * we store the conversion values so that the system API needn't be
    * invoked each time. Here we are assuming that the number of milli secs per
    * clock tick is also integer value(for eg: Linux 2.4, on intel x86 
    * platforms, one clock tick is 1/100 second(or 10 milli second. From
    * Linux 2.6.13, on Intel x86 platforms, one clock tick is 1/1000 second
    * (or 1 milli second).
    */
    pCtxt->ulClkTicksPerSec = cmi_lib_timer_get_total_clock_ticks_per_sec();
    pCtxt->ulMilliSecsPerClkTick = 
             CM_LIB_TMR_MILLI_SECONDS_PER_SECOND/pCtxt->ulClkTicksPerSec;
  }while (0);

  if( lRetVal != OF_SUCCESS )
  {
    if( pCtxt )
    {
      if( pCtxt->pMemPool )
      {
        cm_delete_mempool(pCtxt->pMemPool);
      }
       
      of_free(pCtxt);
    } 
  }
  else
  {
    *ppTimerLibCtxt = (void *)pCtxt;
  }
 
  return lRetVal; 
}


/*****************************************************************************/
/* Function        : cm_lib_timer_de_init                                       */
/* Description     : This API de-initializes the timer library context which */
/*                   was previously initialized using API "cm_lib_timer_init". */
/*                   It is assumed all active timers are stopped before this */
/*                   API is invoked.                                         */
/* Input Arguments :                                                         */
/*    ppTimerLibCtxt  -  pointer to memory which has the pointer to the      */
/*                       library context to be de-initialized.               */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful de-initialization of library  */
/*                               context. pointer to the memory holding the  */
/*                               library context pointer is filled with NULL.*/
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_de_init(
                            void  **ppTimerLibCtxt
                          )
{
  UCMLibTimerCtxt_t * pTempTimerCtxt;
  
 /*
  * Sanity checks
  */
  if( (ppTimerLibCtxt == NULL) || (*ppTimerLibCtxt == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTempTimerCtxt = (UCMLibTimerCtxt_t *)*ppTimerLibCtxt;
#ifdef CM_LIB_TIMER_DEBUG   
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTempTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif

  cm_delete_mempool(pTempTimerCtxt->pMemPool);

  of_free(pTempTimerCtxt);
  *ppTimerLibCtxt = NULL;

  return OF_SUCCESS;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_create_timer                                  */
/* Description     : This API create a timer.                                */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    ppTimer        -  pointer to memory to hold the timer pointer          */
/* Return Value    : OF_SUCCESS - on successful creation of timer. pointer to */
/*                               timer created is returned in output argument*/
/*                                ppTimer.                                   */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_create_timer(
                                 void       *pTimerLibCtxt,
                                 void       **ppTimer
                               )
{
  UCMLibTimer_t  *pTimerNode;
  UCMLibTimerCtxt_t  *pTimerCtxt;
  unsigned char  heap_b = TRUE;
  int32_t   lRetVal; 

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (ppTimer == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;

#ifdef CM_LIB_TIMER_DEBUG   
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif

  heap_b = TRUE;  
  lRetVal = cm_mempool_get_block(pTimerCtxt->pMemPool,
                               (unsigned char **)&pTimerNode, 
                               &heap_b);

  if( lRetVal == OF_SUCCESS )
  {
#ifdef CM_LIB_TIMER_DEBUG
    CM_LIB_TIMER_INIT_TIMER_NODE_PATTERN_FIELDS(pTimerNode);
#endif
    CM_LIB_TIMER_CLEAR_TIMER_FLAGS(pTimerNode);
    CM_LIB_TIMER_SET_TIMER_HEAP(pTimerNode, heap_b);
    *ppTimer = (void *)pTimerNode;
    return OF_SUCCESS;
  }
  else if( lRetVal == CM_MEM_DRAINED_OUT )
  {
    return CM_LIB_TIMER_ERR_TIMERS_EXHAUSTED;
  }
  else
  {
    return CM_LIB_TIMER_ERR_MEM_ALLOC_FAIL;
  }
}

/*****************************************************************************/
/* Function        : cm_lib_timer_delete_timer                                  */
/* Description     : This API deletes a timer. Timer to be deleted shouldn't */
/*                   be active (running)                                     */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    ppTimer        -  pointer to memory which has pointer to the timer to  */
/*                      be deleted.                                          */
/* Return Value    : OF_SUCCESS - on successful deletion of timer. pointer to */
/*                               the memory holding the timer pointer is     */
/*                               filled with NULL.                           */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_delete_timer(
                                 void         *pTimerLibCtxt,
                                 void         **ppTimer
                                )
{
  int32_t   lRetVal;
  UCMLibTimer_t* pTempTimer;
  UCMLibTimerCtxt_t  *pTimerCtxt;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (ppTimer == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTempTimer = *ppTimer;

#ifdef CM_LIB_TIMER_DEBUG   
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTempTimer) )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_NODE;
#endif

  if( CM_LIB_TIMER_IS_TIMER_RUNNING(pTempTimer) )
    return CM_LIB_TIMER_ERR_TIMER_ACTIVE;

  lRetVal = cm_mempool_release_block(pTimerCtxt->pMemPool,
                                   (unsigned char*)pTempTimer, 
                                    CM_LIB_TIMER_GET_TIMER_HEAP(pTempTimer));

  *ppTimer = NULL;
  return OF_SUCCESS;
}
 
/*****************************************************************************/
/* Function        : cm_lib_timer_convert_timer_val                              */
/* Description     : This internal function is used as a utility routine to  */
/*                   convert the timer values from one unit to another unit  */
/* Input Arguments :                                                         */
/*    pTimerCtxt     -  pointer to the library context.                      */
/*    ulTimerType    -  timer type(units) for the timer value.               */
/*    ulTimerVal     -  timer value (units specified by ulTimerType)         */
/*    ulDesTimerType -  timer type(units) to which the timer value has to be */
/*                      converted                                            */ 
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : converted timer value                                   */
/*****************************************************************************/
 inline  uint32_t  cm_lib_timer_convert_timer_val(
                                             UCMLibTimerCtxt_t  *pTimerCtxt,
                                             uint32_t           ulTimerType,
                                             uint32_t           ulTimerVal,
                                             uint32_t           ulDesTimerType
                                            )
{
   uint32_t   ulTmpTimerVal;

   ulTmpTimerVal = 0;

   switch(ulTimerType & CM_LIB_TIMER_TYPE_VALID_BITS_MASK)
   {
     case CM_LIBTIMER_MILLI_SECS_TIMER:
     {
       switch(ulDesTimerType & CM_LIB_TIMER_TYPE_VALID_BITS_MASK)
       {
         case CM_LIBTIMER_SECS_TIMER:
           ulTmpTimerVal = ulTimerVal / CM_LIB_TMR_MILLI_SECONDS_PER_SECOND;
           break;

         case CM_LIBTIMER_CLOCK_TICKS_TIMER:
           ulTmpTimerVal = ulTimerVal / pTimerCtxt->ulMilliSecsPerClkTick;
           break;

         case CM_LIBTIMER_MILLI_SECS_TIMER:
           ulTmpTimerVal = ulTimerVal;
           break;
       } 
     }
     break;

     case CM_LIBTIMER_SECS_TIMER:
     {
       switch(ulDesTimerType & CM_LIB_TIMER_TYPE_VALID_BITS_MASK)
       {
         case CM_LIBTIMER_MILLI_SECS_TIMER:
           ulTmpTimerVal = ulTimerVal * CM_LIB_TMR_MILLI_SECONDS_PER_SECOND;
           break;

         case CM_LIBTIMER_CLOCK_TICKS_TIMER:
           ulTmpTimerVal = ulTimerVal * pTimerCtxt->ulClkTicksPerSec;
           break;

         case CM_LIBTIMER_SECS_TIMER:
           ulTmpTimerVal = ulTimerVal;
           break;
       }      
     }
     break;

     case CM_LIBTIMER_CLOCK_TICKS_TIMER:
     {
       switch(ulDesTimerType & CM_LIB_TIMER_TYPE_VALID_BITS_MASK)
       {
         case CM_LIBTIMER_MILLI_SECS_TIMER:
           ulTmpTimerVal = ulTimerVal * pTimerCtxt->ulMilliSecsPerClkTick;
           break;

         case CM_LIBTIMER_SECS_TIMER:
           ulTmpTimerVal = ulTimerVal / pTimerCtxt->ulClkTicksPerSec;
           break;

         case CM_LIBTIMER_CLOCK_TICKS_TIMER:
           ulTmpTimerVal = ulTimerVal;
           break;
       }      
     }
     break;
   }

   return ulTmpTimerVal;
}
/*****************************************************************************/
/* Function        : cm_lib_timer_is_timer_type_valid                             */
/* Description     : This internal function is used to check the validity of */
/*                   time type                                               */
/* Input Arguments :                                                         */
/*    ulTimerType    -  timer type.                                          */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : TRUE if the timer type is valid. Otherwise FALSE        */
/*****************************************************************************/
 inline  unsigned char   cm_lib_timer_is_timer_type_valid(
                                                          uint32_t ulTimerType
                                                        )
{
  switch (ulTimerType & CM_LIB_TIMER_TYPE_VALID_BITS_MASK)
  {
    case CM_LIBTIMER_SECS_TIMER:
      return TRUE;
    
    default:
      return FALSE;
  }
}

/*****************************************************************************/
/* Function        : cm_lib_timer_start_timer                                   */
/* Description     : This API starts a timer. Timer to be started shouldn't  */
/*                   be active (running)                                     */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer to be started.                  */
/*    pHandler       -  handler to be called when this timer expires.        */
/*    user_data_p      -  user data(opaque) that will be passed to the handler */
/*                      along with timer as arguments.                       */
/*    ulTimerType    -  indicates the timer type.                            */
/*    ulTimerValue   -  indicates the timer value.                           */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful starting of the timer.        */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_start_timer(
                                void                      *pTimerLibCtxt,
                                void                      *pTmr,
                                CM_LIBTIMER_TIMER_HANDLER  pHandler,
                                UCMLibTimerUserData_t       *user_data_p,
                                uint32_t                    ulTimerType,
                                uint32_t                    ulTimerValue
                              )
{
  UCMLibTimerCtxt_t  *pTimerCtxt;
  UCMLibTimer_t* pTimer;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pTmr == NULL) || 
      (pHandler == NULL)/* || (user_data_p == NULL)*/ )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTimer = (UCMLibTimer_t *)pTmr;

 /*
  * currently only seconds timers are supported
  */
  if( cm_lib_timer_is_timer_type_valid(ulTimerType) == FALSE )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_TYPE;

  if( ulTimerValue == 0 )
  {
    pTimerCtxt->stats.ulInvalidTimeoutValues++;     
    return CM_LIB_TIMER_ERR_INVALID_TIMEOUT_VALUE;
  }

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer) )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_NODE;
#endif

  if( CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer) )
  {
    pTimerCtxt->stats.ulActiveTimersStart++;
    return CM_LIB_TIMER_ERR_TIMER_ACTIVE;
  }

  pTimer->pHandler = pHandler;
  if(user_data_p != NULL)
     pTimer->UserData = *user_data_p;
  pTimer->ulTimerType = ulTimerType;
  pTimer->ulTimerVal = ulTimerValue;

  return cm_lib_timer_insert_node( pTimerCtxt, 
                                pTimer,
                                CM_LIB_TIMER_PASSIVE_LIST );
}
 

/*****************************************************************************/
/* Function        : cm_lib_timer_stop_timer                                    */
/* Description     : This API stops an active(running) timer.                */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer to be started.                  */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful stopping of the timer.        */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_stop_timer(
                               void        *pTimerLibCtxt,
                               void        *pTmr
                             )  
{
  UCMLibTimerCtxt_t  *pTimerCtxt;
  UCMLibTimer_t* pTimer;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pTmr == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTimer = (UCMLibTimer_t *)pTmr;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer) )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_NODE;
#endif

  if( !CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer) )
  {
    pTimerCtxt->stats.ulInactiveTimersStop++;
    return CM_LIB_TIMER_ERR_TIMER_INACTIVE;
  }

  return cm_lib_timer_remove_node( pTimerCtxt, 
                                pTimer );

}

/*****************************************************************************/
/* Function        : cm_lib_timer_restart_timer                                 */
/* Description     : This API restarts a timer. If timer is active(running), */
/*                   it will be stopped(handler will not be called) before   */
/*                   restarting it.                                          */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer to be started.                  */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful restarting of the timer.      */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_restart_timer(
                                  void        *pTimerLibCtxt,
                                  void        *pTmr
                                )
{
  int32_t lRetVal;
  UCMLibTimerCtxt_t  *pTimerCtxt;
  UCMLibTimer_t* pTimer;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pTmr == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  lRetVal = OF_SUCCESS;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTimer = (UCMLibTimer_t *)pTmr;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer) )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_NODE;
#endif

   if( CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer) )
   {
    /*
     * delete the timer node from active list
     */
     lRetVal = cm_lib_timer_remove_node( pTimerCtxt, 
                                      pTimer );
   }
   else
   {
     if( pTimer->pHandler == NULL )
       lRetVal = CM_LIB_TIMER_ERR_TIMER_INACTIVE;
   }

   if( lRetVal == OF_SUCCESS )
   {   
     lRetVal = cm_lib_timer_insert_node( pTimerCtxt, 
                                      pTimer,
                                      CM_LIB_TIMER_PASSIVE_LIST );
   }

   return lRetVal;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_restart_timer_with_new_value                     */
/* Description     : This API restarts a timer with new value. If the timer  */
/*                   is active(running), it will be stopped(handler will not */
/*                   be called) before restarting it.                        */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer to be started.                  */
/*    ulTimerValue   -  indicates the new timer value.                       */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful restarting of the timer.      */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_restart_timer_with_new_value(
                                              void         *pTimerLibCtxt,
                                              void         *pTmr,
                                              uint32_t       ulTimerValue
                                            )
{
  int32_t lRetVal;
  UCMLibTimerCtxt_t  *pTimerCtxt;
  UCMLibTimer_t* pTimer;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pTmr == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  lRetVal = OF_SUCCESS;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTimer = (UCMLibTimer_t *)pTmr;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer) )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_NODE;
#endif

  if( ulTimerValue == 0 )
  {
    pTimerCtxt->stats.ulInvalidTimeoutValues++;     
    return CM_LIB_TIMER_ERR_INVALID_TIMEOUT_VALUE;
  }

  if( CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer) )
   {
    /*
     * delete the timer node from active list
     */
    lRetVal = cm_lib_timer_remove_node( pTimerCtxt, 
                                      pTimer );
   }
   else
   {
     if( pTimer->pHandler == NULL )
       lRetVal = CM_LIB_TIMER_ERR_TIMER_INACTIVE;
   }

   if( lRetVal == OF_SUCCESS )
   {   
     pTimer->ulTimerVal = ulTimerValue;   
     lRetVal = cm_lib_timer_insert_node( pTimerCtxt, 
                                     pTimer,
                                     CM_LIB_TIMER_PASSIVE_LIST );
   }

  return lRetVal;
}


/*****************************************************************************/
/* Function        : cm_lib_timer_get_next_expiry_timer_remval                     */
/* Description     : This API returns the timeout value of the next active   */
/*                   timer(if any) that will be expired in given library     */
/*                   context. Applications can use this API to decide the    */
/*                   timeout value for they can wait for events(eg: poll,    */
/*                   select, epoll). If there are no active timers then this */
/*                   returns OF_SUCCESS and fills the memory pointed by the   */
/*                   output argument "pTimerVal" with zeros. in such cases   */
/*                   applications can chose to wait indefinitely for events. */
/*                   This routine internally invokes another routine to move */
/*                   the timers from passive list to active list.            */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    pTimerVal      -  pointer to the memory where the next expiry timer's  */
/*                      timeout(milli seconds) value is filled.              */
/* Return Value    : OF_SUCCESS - on success.                                 */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_get_next_expiry_timer_remval(
                                               void     *pTimerLibCtxt,
                                               uint32_t   *pTimerVal
                                            )
{
  UCMLibTimer_t  *pTimerNode;
  UCMLibTimerCtxt_t  *pTimerCtxt;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL) || (pTimerVal == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif

 /*
  * first process the passive list, in case the application might be re/starting
  * some timers after calling cmo_lib_timer_process_timers(eg: appl processing the
  * timers before handling fd events).
  */
  cm_lib_timer_process_passive_list(pTimerCtxt);

  pTimerNode = (UCMLibTimer_t *) CM_DLLQ_FIRST(&(pTimerCtxt->ActiveHead));

  if( pTimerNode )
  {
    *pTimerVal =  cm_lib_timer_convert_timer_val(pTimerCtxt,
                                             CM_LIBTIMER_CLOCK_TICKS_TIMER,
                                             pTimerNode->ulRemTimerVal,
                                             CM_LIBTIMER_MILLI_SECS_TIMER);
  }
  else
  {
    *pTimerVal = 0;
  }

  return OF_SUCCESS;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_process_timers                                */
/* Description     : Internal routine called to do the required work for the */
/*                   API cmo_lib_timer_process_timers.                           */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    ulElapsedTime  -  time(clock ticks) elapsed in waiting for the events. */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on success processing of active timers.     */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
 inline int32_t  cm_lib_timer_process_timers(
                                              UCMLibTimerCtxt_t  *pTimerCtxt,
                                              uint32_t           ulElapsedTime  
                                            )
{
  UCMLibTimer_t      *pHeadNode;


  if( ulElapsedTime == 0 ) /* nothing to process */
    return OF_SUCCESS;

  pHeadNode = (UCMLibTimer_t *)CM_DLLQ_FIRST(&(pTimerCtxt->ActiveHead));
  if( pHeadNode == NULL )
    return OF_SUCCESS;

 /*
  * This safety check ensures that if caller(whose is supposed to not wait
  * for events for more than first timer remaining value) passed large
  * elapsed value, then don't end up in timing out all timers in the list. In
  * such cases, the worst effect will be on periodic timers which will be
  * immediately timing out again after restart(in this same function context).
  * BUT THIS CHECKING NEEDS A REVISIT WHEN DIFFERENT TIMER TYPES ARE SUPPORTED.
  * Do we need to have such check ???
  */
  if( ulElapsedTime > pHeadNode->ulRemTimerVal )
    ulElapsedTime = pHeadNode->ulRemTimerVal;  

  do
  {
    if( pHeadNode->ulRemTimerVal )
    {
      if( pHeadNode->ulRemTimerVal > ulElapsedTime )
    {
      pHeadNode->ulRemTimerVal -= ulElapsedTime;
      break; 
    }
      else
    {
      ulElapsedTime -= pHeadNode->ulRemTimerVal;
      pHeadNode->ulRemTimerVal = 0;
    }
    }    

   /*
    * delete the elapsed timer node from the active list
    */
    cm_lib_timer_remove_node(pTimerCtxt, pHeadNode);

   /*
    * call the handler to take appropriate action
    */
    pHeadNode->pHandler(pHeadNode, &(pHeadNode->UserData));

    pHeadNode = (UCMLibTimer_t *) CM_DLLQ_FIRST(&(pTimerCtxt->ActiveHead));
  }while( pHeadNode );


  return OF_SUCCESS;
}

/*****************************************************************************/
/* Function        : cmo_lib_timer_process_timers                                */
/* Description     : This API is called by the application frequently to     */
/*                   process the active timers. Applications waiting for the */
/*                   events(using poll. select, epoll) have to determine the */
/*                   time elapsed in waiting for the events and call this API*/
/*                   with the elapsed time to process the active timers.     */
/*                   This library provides some porting routines for the     */
/*                   applications to estimate the time elapsed in waiting for*/
/*                   events. If any active timer elapsed, it is deleted from */
/*                   the active list and the timer handler is called so that */
/*                   it gets notified about the expiry of the timer and take */
/*                   any appropriate action.                                 */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    ulElapsedTime  -  time(clock ticks) elapsed in waiting for the events. */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on success processing of active timers.     */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cmo_lib_timer_process_timers(
                                   void     *pTimerLibCtxt,
                                   uint32_t   ulElapsedTime  
                                 )
{
  UCMLibTimerCtxt_t  *pTimerCtxt;
  int32_t            lRetVal;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;
 
  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif

  lRetVal = cm_lib_timer_process_timers(pTimerCtxt,
                                     ulElapsedTime);

 /*
  * we are done with processing the timers in active list. So now process the
  * passive list. Its better to do the passive list processing here also as
  * some appl's may not use the routine cm_lib_timer_get_next_expiry_timer_remval.
  */
  cm_lib_timer_process_passive_list(pTimerCtxt);

  return lRetVal;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_pre_wait                                      */
/* Description     : This API is called before the application goes on a     */
/*                   waiting/polling for events. The Application is supposed */
/*                   to use this routine along with its twin routine         */
/*                   cm_lib_timer_post_wait, which is to be called after the    */
/*                   application comes out of the waiting/polling. This      */
/*                   routine returns to the application (through the output  */
/*                   argument) the timeout value of the next active timer(if */
/*                   any) that will be expired in given library context. The */
/*                   applications can use this API to decide the timeout val */
/*                   for which they can wait for events(eg:poll,select,epoll)*/
/*                   If there are no active timers then this returns T_SUCCES*/
/*                   and fills the memory pointed by the output argument     */
/*                   "pTimerVal" with zeros. in such cases applications can  */
/*                   chose to wait indefinitely for events. This routine     */
/*                   internally invokes another routine to move the timers   */
/*                   from passive list to active list. This function also    */
/*                   internally records the timestamp so that the timestamp  */
/*                   taken in the cm_lib_timer_post_wait routine can be used to */
/*                   determine the time(clock ticks) elapsed while the appl  */
/*                   was waiting for the events.                             */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    pTimerVal      -  pointer to the memory where the next expiry timer's  */
/*                      timeout(milli seconds) value is filled.              */
/* Return Value    : OF_SUCCESS - in case of success                          */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_pre_wait(
                             void   *pTimerLibCtxt,
                             uint32_t *pTimerVal  
                           )
{
  UCMLibTimer_t      *pTimerNode;
  UCMLibTimerCtxt_t  *pTimerCtxt;

 /*
  * sanity checks
  */
  if( pTimerLibCtxt == NULL )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif

 /*
  * first process the passive list, in case the application might be re/starting
  * some timers after calling cm_lib_timer_post_wait(eg: appl processing the timers
  * before handling fd events).
  */
  cm_lib_timer_process_passive_list(pTimerCtxt);

 /*
  * record pre wait time stamp
  */
  cm_lib_timer_get_time_stamp(&(pTimerCtxt->ulPreWaitTimeStamp));
  pTimerCtxt->ulFlag |= CM_LIB_TIMER_PRE_WAIT_TIME_STAMP;

  if( pTimerVal )
  {
    pTimerNode = (UCMLibTimer_t *) CM_DLLQ_FIRST(&(pTimerCtxt->ActiveHead));

    if( pTimerNode )
    {
      *pTimerVal =  cm_lib_timer_convert_timer_val(pTimerCtxt,
                                               CM_LIBTIMER_CLOCK_TICKS_TIMER,
                                               pTimerNode->ulRemTimerVal,
                                               CM_LIBTIMER_MILLI_SECS_TIMER);
    }
    else
    {
      *pTimerVal = 0;
    }
  }

  return OF_SUCCESS;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_post_wait                                     */
/* Description     : This API is called after the application finishes       */
/*                   waiting/polling for events. The Application is supposed */
/*                   to use this routine along with its twin routine         */
/*                   cm_lib_timer_pre_wait, which is to be called before the    */
/*                   application goes on waiting/polling for events. This    */
/*                   routine records the timestamp and estimates the time    */
/*                   (clock ticks) elapsed while the application was waiting */
/*                   for events. It then process the active(running) timers. */
/*                   If any active timer elapsed, it is deleted from the     */
/*                   active list and the timer handler is called so that it  */
/*                   gets notified about the expiry of the timer and take any*/
/*                   appropriate action.                                     */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    None                                                                   */
/* Return Value    : OF_SUCCESS - on success processing of active timers.     */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_post_wait(
                              void   *pTimerLibCtxt
                            )
{
  uint32_t           ulElapsedTime;
  int32_t            lRetVal;
  UCMLibTimerCtxt_t  *pTimerCtxt;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;
 
  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif


  if( !(pTimerCtxt->ulFlag & CM_LIB_TIMER_PRE_WAIT_TIME_STAMP) )
    return CM_LIB_TIMER_ERR_TIMER_PRE_WAIT_CALL_NOT_INVOKED;

  cm_lib_timer_get_time_stamp(&(pTimerCtxt->ulPostWaitTimeStamp));  
  pTimerCtxt->ulFlag &= (~CM_LIB_TIMER_PRE_WAIT_TIME_STAMP);

  ulElapsedTime = cm_lib_timer_get_elapsed_time(pTimerCtxt->ulPreWaitTimeStamp,
                                            pTimerCtxt->ulPostWaitTimeStamp);
   
  lRetVal =  cm_lib_timer_process_timers(pTimerLibCtxt,
                                      ulElapsedTime);

 /*
  * we are done with processing the timers in active list. So now process the
  * passive list. Its better to do the passive list processing here also as
  * some appl's may not use the routine cm_lib_timer_get_next_expiry_timer_remval.
  */
  cm_lib_timer_process_passive_list(pTimerCtxt);

  return lRetVal;
}


/*****************************************************************************/
/* Function        : cm_lib_timer_is_timer_active                                */
/* Description     : This API is used to know whether a given timer is active*/
/*                   (running) or not.                                       */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer.                                */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : TRUE - if the timer is active.                          */
/*                   FALSE - if the timer is inactive or any other error.    */
/*****************************************************************************/
unsigned char  cm_lib_timer_is_timer_active(
                                   void   *pTimerLibCtxt,
                                   void   *pTmr  
                                 )
{
  UCMLibTimerCtxt_t  *pTimerCtxt;
  UCMLibTimer_t* pTimer;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pTmr == NULL) )
    return FALSE;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTimer = (UCMLibTimer_t *)pTmr;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return FALSE;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer) )
    return FALSE;
#endif

  return CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer);
}


/*****************************************************************************/
/* Function        : cm_lib_timer_get_timer_expiry_val                            */
/* Description     : This API is used to get the timer expiry(remaining)     */
/*                   timeout value                                           */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer.                                */
/* Output Arguments:                                                         */
/*    pulRemVal      - pointer to memory to hold remaining timeout value.    */
/*                     remaining timeout value will be in same units as timer*/
/* Return Value    : OF_SUCCESS - on success.                                 */
/*                   Appropriate Error - is there is some error.             */
/*****************************************************************************/
int32_t  cm_lib_timer_get_timer_expiry_val(
                                       void   *pTimerLibCtxt,
                                       void   *pTmr,
                                       uint32_t *pulRemVal 
                                     )
{
  UCMLibTimerCtxt_t  *pTimerCtxt;
  UCMLibTimer_t      *pTimer;
  UCMLibTimer_t      *pTempNode;
  uint32_t           ulRemTime;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pTmr == NULL) || (pulRemVal == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;
  pTimer = (UCMLibTimer_t *)pTmr;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;

  if( !CM_LIB_TIMER_ARE_TIMER_NODE_PATTERN_FIELDS_VALID(pTimer) )
    return CM_LIB_TIMER_ERR_INVALID_TIMER_NODE;
#endif

  if( !CM_LIB_TIMER_IS_TIMER_RUNNING(pTimer) )
    return CM_LIB_TIMER_ERR_TIMER_INACTIVE;

  if( CM_LIB_TIMER_IS_TIMER_ACTIVE(pTimer) )
  {
 /*
  * Now scan through the list to find out the correct position for this  
  * timer node.
  */
  ulRemTime = 0;
  CM_DLLQ_SCAN(&(pTimerCtxt->ActiveHead),
                    pTempNode,
                    UCMLibTimer_t *)
  {
    ulRemTime += pTempNode->ulRemTimerVal;

    if( pTempNode == pTimer )
      break;
  }

    ulRemTime = cm_lib_timer_convert_timer_val(pTimerCtxt,
                                           CM_LIBTIMER_CLOCK_TICKS_TIMER,
                                           ulRemTime,
                                           pTimer->ulTimerType);
  }
  else  if( CM_LIB_TIMER_IS_TIMER_PASSIVE(pTimer) )
  {
    ulRemTime = pTimer->ulTimerVal;
  }
  else
    return CM_LIB_TIMER_ERR_TIMER_INACTIVE;

  *pulRemVal = ulRemTime;
  return OF_SUCCESS;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_get_total_clock_ticks_per_sec                     */
/* Description     : This API returns the number of clock ticks in a second  */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : number of clock ticks per second.                       */
/*****************************************************************************/
uint32_t  cm_lib_timer_get_total_clock_ticks_per_sec(
                                               void     *pTimerLibCtxt
                                             )
{
  if( pTimerLibCtxt == NULL )
    return cmi_lib_timer_get_total_clock_ticks_per_sec();

  return ((UCMLibTimerCtxt_t *)pTimerLibCtxt)->ulClkTicksPerSec;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_get_stats                                     */
/* Description     : This API is used to get the statistics of the library   */
/*                   module for the given context.                           */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    pStats         -  pointer to the memory where the statistic info has to*/
/*                      be filled.                                           */
/* Return Value    : OF_SUCCESS - on success.                                 */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
int32_t  cm_lib_timer_get_stats(
                              void               *pTimerLibCtxt,
                              UCMLibTimerStats_t   *pStats     
                            )
{
  struct cm_mempool_stats mempoolstats;
  UCMLibTimerStats_t *pTempStats;
  UCMLibTimerCtxt_t  *pTimerCtxt;

 /*
  * sanity checks
  */
  if( (pTimerLibCtxt == NULL)  || (pStats == NULL) )
    return CM_LIB_TIMER_ERR_INVALID_ARGUMENT;

  pTimerCtxt = (UCMLibTimerCtxt_t *)pTimerLibCtxt;

#ifdef CM_LIB_TIMER_DEBUG
  if( !CM_LIB_TIMER_ARE_TIMER_CTXT_PATTERN_FIELDS_VALID(pTimerCtxt) )
    return CM_LIB_TIMER_ERR_INVALID_LIBRARY_CONTEXT;
#endif

  cm_mempool_get_stats(pTimerCtxt->pMemPool, &mempoolstats);
  pTempStats = &(pTimerCtxt->stats);

  pStats->ulMaxStaticTimers = mempoolstats.ulBlocksCnt;
  pStats->ulMaxHeapTimers = mempoolstats.ulHeapAlwdBlocks;
  pStats->ulAvailStaticTimers = mempoolstats.ulFreeCnt;
  pStats->ulAvailHeapTimers = mempoolstats.ulHeapFreeCnt;
  pStats->ulAllocHits = mempoolstats.ulAllocCnt + mempoolstats.ulAllocFailCnt +
          mempoolstats.ulHeapAllockBlocks + mempoolstats.ulHeapAllocFailCnt;
  pStats->ulReleaseHits = mempoolstats.ulReleaseCnt + 
          mempoolstats.ulReleaseFailCnt + mempoolstats.ulHeapReleaseCnt;
  pStats->ulAllocFails = mempoolstats.ulAllocFailCnt + 
          mempoolstats.ulHeapAllocFailCnt;
  pStats->ulReleaseFails = mempoolstats.ulReleaseFailCnt;
  pStats->ulActiveTimersStart = pTempStats->ulActiveTimersStart;  
  pStats->ulInactiveTimersStop = pTempStats->ulInactiveTimersStop;
  pStats->ulInactiveTimersRestart = pTempStats->ulInactiveTimersRestart;
  pStats->ulInvalidTimerTypes = pTempStats->ulInvalidTimerTypes;
  pStats->ulInvalidTimeoutValues = pTempStats->ulInvalidTimeoutValues;

  return OF_SUCCESS;
}

/*****************************************************************************/
/* Function        : cm_lib_timer_insert_node                                   */
/* Description     : This internal function is used to insert a timer in the */
/*                   active/passive list. If the node has to be inserted in  */
/*                   active list it will be inserted at appropriate position */
/*                   based on its expiry value. The timer node flags are     */
/*                   appropriately adjusted.                                 */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer that has to be inserted.        */
/*    ulListType     -  indicates the list type                              */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful insertion of the timer.       */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
  int32_t  cm_lib_timer_insert_node(
                                            UCMLibTimerCtxt_t   *pTimerCtxt,
                                            UCMLibTimer_t       *pTimer,
                                            uint32_t            ulListType
                                          )
{
  UCMLibTimer_t*  pCurrNode;
  UCMLibTimer_t*  pTempNode;
  UCMLibTimer_t*  pPrevNode;
  UCMLibTimer_t*  next_node_p;
  uint32_t        ulCurrTime; 

  if( ulListType == CM_LIB_TIMER_ACTIVE_LIST )
  { 
 /*
    * Now scan through the active list to find out the correct position for 
    * this timer node.
  */
    pTimer->ulTimerValInClkTicks = 
          cm_lib_timer_convert_timer_val(pTimerCtxt,
                                     pTimer->ulTimerType,
                                     pTimer->ulTimerVal,
                                     CM_LIBTIMER_CLOCK_TICKS_TIMER);
    pTimer->ulRemTimerVal = 0;
  ulCurrTime = 0;
  pPrevNode = NULL;
  CM_DLLQ_DYN_SCAN(&(pTimerCtxt->ActiveHead),
                    pCurrNode, pTempNode,
                    UCMLibTimer_t *)
  {
#ifdef CM_LIB_TIMER_DEBUG
    if( pCurrNode == pTimer )
    {
      (UCMLibTimerCtxt_t *)pTimerCtxt->stats.ulActiveTimersStart++;
      return CM_LIB_TIMER_ERR_TIMER_ACTIVE;
    }
#endif
    ulCurrTime += pCurrNode->ulRemTimerVal;

      if( pTimer->ulTimerValInClkTicks <= ulCurrTime )
    {
      ulCurrTime -= pCurrNode->ulRemTimerVal;
      break;
    }
    else 
    {    
      pPrevNode = pCurrNode;
    }
  }
  
  next_node_p = pCurrNode;

 /*
  * Now next_node_p(if not NULL) points to the node in the list before which
  * the new timer node should be inserted. And pPrevNode(if not NULL) points
  * to the node in the list after which the new timer node should be inserted.
  */
  if( pPrevNode == NULL ) 
  {
    /* insert at the begining of the list */
      pTimer->ulRemTimerVal = pTimer->ulTimerValInClkTicks;
    if( next_node_p ) 
    {
      /* there is a next node, so adjust its remaining timeout value */
      next_node_p->ulRemTimerVal = (next_node_p->ulRemTimerVal - \
                                  pTimer->ulRemTimerVal);
    }
  }
  else
  {
    if( next_node_p )
    {
      /* insert in the middle of the list */
        pTimer->ulRemTimerVal = pTimer->ulTimerValInClkTicks - ulCurrTime;    

      next_node_p->ulRemTimerVal = (next_node_p->ulRemTimerVal - \
                                  pTimer->ulRemTimerVal);
    }
    else
    {
      /* insert at the end of the list */
        pTimer->ulRemTimerVal = pTimer->ulTimerValInClkTicks - ulCurrTime; 
    }
  }

  /* insert the node */
  CM_DLLQ_INSERT_NODE(&(pTimerCtxt->ActiveHead), \
                       (UCMDllQNode_t*)pPrevNode, \
                       (UCMDllQNode_t*)pTimer); 

    CM_LIB_TIMER_CLEAR_TIMER_PASSIVE(pTimer);
    CM_LIB_TIMER_SET_TIMER_ACTIVE(pTimer);
  }
  else
  {
   /*
    * passive list is not an ordered list. so insert at the end
    */
    pTimer->ulRemTimerVal = 0;
    CM_DLLQ_APPEND_NODE(&(pTimerCtxt->PassiveHead), \
                         (UCMDllQNode_t*)pTimer);
    CM_LIB_TIMER_SET_TIMER_PASSIVE(pTimer);
    CM_LIB_TIMER_CLEAR_TIMER_ACTIVE(pTimer);    
  }

  CM_LIB_TIMER_SET_TIMER_RUNNING(pTimer);

  return OF_SUCCESS;
}
   

/*****************************************************************************/
/* Function        : cm_lib_timer_remove_node                                   */
/* Description     : This internal function is used to remove a timer from   */
/*                   the active/passive list. Timer node flags are used to   */
/*                   determine the list(active/passive) from which this node */
/*                   has to be removed                                       */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/*    pTmr           -  pointer to the timer that has to be removed.         */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : OF_SUCCESS - on successful removal of the timer.         */
/*                   Appropriate Error - on failure.                         */
/*****************************************************************************/
  int32_t  cm_lib_timer_remove_node(
                                            UCMLibTimerCtxt_t  *pTimerCtxt,
                                            UCMLibTimer_t      *pTimer
                                          )
{
  UCMLibTimer_t*  next_node_p; 

  if( CM_LIB_TIMER_IS_TIMER_ACTIVE(pTimer) )
  {
  next_node_p = (UCMLibTimer_t *)CM_DLLQ_NEXT( &(pTimerCtxt->ActiveHead), \
                                              (UCMDllQNode_t*)pTimer);
  if( next_node_p != NULL )
  {
    /* 
     * since this timer node is deleted, adjust the next timer node's
     * remaining timeout value
     */
     next_node_p->ulRemTimerVal += pTimer->ulRemTimerVal;
  }

  CM_DLLQ_DELETE_NODE(&(pTimerCtxt->ActiveHead), (UCMDllQNode_t*)pTimer);
    CM_LIB_TIMER_CLEAR_TIMER_ACTIVE(pTimer);
  }

  if( CM_LIB_TIMER_IS_TIMER_PASSIVE(pTimer) )
  {
    CM_DLLQ_DELETE_NODE(&(pTimerCtxt->PassiveHead), (UCMDllQNode_t*)pTimer);
    CM_LIB_TIMER_CLEAR_TIMER_PASSIVE(pTimer);
  }

  CM_LIB_TIMER_CLEAR_TIMER_RUNNING(pTimer);

  return OF_SUCCESS;
}


/*****************************************************************************/
/* Function        : cm_lib_timer_process_passive_list                           */
/* Description     : This internal function is used to move the timers from  */
/*                   the passive list to the active list                     */
/* Input Arguments :                                                         */
/*    pTimerLibCtxt  -  pointer to timer library context.                    */
/* Output Arguments:                                                         */
/*    NONE                                                                   */
/* Return Value    : None                                                    */
/*****************************************************************************/
  void  cm_lib_timer_process_passive_list(
                                             UCMLibTimerCtxt_t  *pTimerCtxt 
                                            )
{
  UCMLibTimer_t   *pTimerNode;

  while( (pTimerNode = (UCMLibTimer_t *)CM_DLLQ_FIRST(&(pTimerCtxt->PassiveHead))) )
  {
    cm_lib_timer_remove_node(pTimerCtxt,
                          pTimerNode);
    
    cm_lib_timer_insert_node(pTimerCtxt,
                          pTimerNode,
                          CM_LIB_TIMER_ACTIVE_LIST);
  }

  return;
}


/**************************************************************************/
/*                                                                        */
/*                        REVISION  HISTORY                               */
/*                        =================                               */
/*                                                                        */
/*  version      Date        Author         Change Description            */
/*  -------    --------      ------        ----------------------         */
/*  1.0        05-25-2005   Sridhar M       Initial implementation        */
/*  1.1        31-01-2006   Sridhar M       added function headers        */
/*  1.2        11-06-2006   Sridhar M       Introduced macro's            */
/*  1.3        12-06-2006   Sridhar M       passive list(BR 27876)        */
/*  1.4        13-06-2006   Sridhar M       timer latency fixes(BR 27877) */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


