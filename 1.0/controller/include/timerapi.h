/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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
**/

/*
 *
 * File name:  timerapi.h
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file contains API functions implementing Timer functionality based on cascaded timer wheels. 
 * API allows Applications to start, stop or restart timers.
 * It supports multiple Timer Containers based on timer granularity required in addition to one default container. 
 * Applications are informed about timer expiries by way of callback function along with two optional arguments they might provide.
 * Highest granularity supported is 1ms.
*/

//! Multi core Timer database API
/*! This module provides Timer resource APIs
 * \addtogroup  Timer_Management
 */

//! Utility modules Error Management
/*! Error Management support for utility modules
 * \addtogroup  Error_Management
 */

#ifndef __TIMERAPI_H
#define __TIMERAPI_H

typedef uint32_t tmr_time_t;

/** \ingroup Timer_Management
  * TIMER_MAX_NO_OF_CONTAINERS
  * - Maximum number of containers supported
  */
#define TIMER_MAX_NO_OF_CONTAINERS 16

/** \ingroup Timer_Management
  * TIMER_DEFAULT_CONTAINER_TIMER_GRANULARITY
  * - Timer granularity for the default container in micro seconds. 
  */
#define TIMER_DEFAULT_CONTAINER_TIMER_GRANULARITY 10000

/** \ingroup Timer_Management
  * TIMER_DEFAULT_CONTAINER_TIMER_RANGE
  * - It specifies the maximum time out value used by timers using the default container.
  */
#define TIMER_DEFAULT_CONTAINER_TIMER_RANGE  2147483647

/** \ingroup Timer_Management
  * TIMER_DEFAULT_CONTAINER_TIMER_ENTRIES_MAX
  * - Max number of timer entries in default container used in memory pool creation.
  */
#define TIMER_DEFAULT_CONTAINER_TIMER_ENTRIES_MAX  1000 

/** \ingroup Timer_Management
  * TIMER_DEFAULT_CONTAINER_TIMER_STATIC_ENTRIES_MAX  
  * - Static timer entries in default container used in memory pool creation.
  */
#define TIMER_DAFAULT_CONTAINER_TIMER_STATIC_ENTRIES_MAX 720

/** \ingroup Error_Management
  * TIMER_SUCCESS
  * - Timer API Functions return this value on successful execution
 */
#define TIMER_SUCCESS 0

/** \ingroup Error_Management
  * TIMER_FAILURE
  * - Tmer API Functions return this value on general failure
 */
#define TIMER_FAILURE -1

/** \ingroup Error_Management
  * TIMER_ERROR_INVALID_CONTAINER
  * - API Functions return this error when container is invalid.
  */
#define TIMER_ERROR_INVALID_CONTAINER -80

/** \ingroup Error_Management
  * TIMER_ERROR_FAILED_TO_DELETE_CONTAINER
  * - API Functions return this value when a container deletion failed.
  */
#define TIMER_ERROR_FAILED_TO_DELETE_CONTAINER -81

/** \ingroup Error_Management
  * TIMER_ERROR_INVALID_CALLBACK_FUNCTION
  * - API Functions return this value when the callback function is invalid.
  */
#define TIMER_ERROR_INVALID_CALLBACK_FUNCTION -82

/** \ingroup Error_Management
  * TIMER_ERROR_INVALID_SAFEREF
  * - API Functions return this value when the saferef magic number is zero.
  */
#define TIMER_ERROR_INVALID_SAFEREF -83

/** \ingroup Timer_Management
  \typedef tmr_cbk_func
  \brief Prototype for the callback function to be used by Applications.\n
  <b>Description</b>\n 
  The Timer module API "tmr_create_and_start()" and "tmr_restart()" expect callback function pointer as an argument. 
  This callback function is called by Timer module upon expiry of timers.
  */
typedef void (*tmr_cbk_func)(void *,void *);

/** \ingroup Timer_Management
  \struct saferef
  \brief This structure holds the safe reference for a timer node or a container node.\n
  <b>Description</b>\n
  Safe reference is used by Timer module to access the timer or container node without employing any search.
  */ 
struct saferef{
  /** This array index is used to access the pointer to timer or container node */ 
  uint32_t index;
  /** magic number used to invalidate the timer or container node */ 
  uint32_t magic;
};

/** \ingroup Timer_Management
  \struct tmr_saferef 
  \brief This structure is passed by Applications while creating a timer node
  */
struct tmr_saferef{
   /** Safe reference of timer node */ 
   struct   saferef saferef;
   /** ID of the Thread is filled by the timer module and is used for internal purpose.*/
   uint32_t thread_id;
};

/** \ingroup Timer_Management
  \brief This function initializes the timer module\n
  <b>Description</b>\n
  This module init function is to be called once by the Applications before using the other timer APIs.
  \return  MEMPOOL_SUCCESS -Upon successful initialization of the module.
  \return  MEMPOOL_FAILURE -Upon unsuccessful initialization of the module.
  */
int32_t tmr_module_init(void);

/** \ingroup Timer_Management
  \brief Creates a timer container\n
  <b>Description:</b>\n
  This function is used to create a timer container.It involves computing
  the number of wheels, number of buckets in each wheel based on the input arguments of granularity and range.
  Application may make use of the default container instead of creating a new one.
  \param[in]  granularity  - Granularity of the timers in micro seconds.
  \param[in]  range        - Maxumum time out value used by timers.  
  \param[out] container_saferef_p -  Safe reference of the container created
  \return TIMER_SUCCESS  - In case of successful creation of container
  \return TIMER_FAILURE  - In case of any error condition
  */
int32_t tmr_init_container(uint32_t granularity,uint32_t range,struct saferef* container_saferef_p);

/** \ingroup Timer_Management
  \brief This function is used to delete a container.

  \param[in] container_saferef_p - Safe reference of the container to be deleted
  \param[in] flag_b - If flag_b =1,All the existing timer nodes are forcefully deleted before deleting the container
  \return TIMER_SUCCESS - In case of successful deletion of the container
  \return TIMER_FAILURE - In case of any error condition
*/ 
int32_t tmr_delete_container(struct saferef* container_saferef_p,uint8_t flag_b);

/** \ingroup Timer_Management
  /brief This function Creates and starts a timer\n
  <b>Description:</b>\n
  This function creates and starts a timer. It validates the container
  to which the timer is intended to be added. It allocates memory for the timer node 
  and puts it in the appropriate timer bucket. The timer node is then added to the 
  safe reference data structure and the index and magic number of the safe reference 
  is then filled in the output parameter.
  
  \param[in]  container_saferef_p - Safe reference of the container to which timer is to be added
  \param[in]  timeout             - Time out of the timer in micro seconds
  \param[in]  periodic_b          - When set to 1,a perodic timer is started instead of a regular timer. 
  \param[in]  func_p              - Callback function to be called after timer expiry
  \param[in]  args1_p             - First Argument to the callback function
  \param[in]  args2_p             - Second Argument to the callback function
  \param[out] tmr_saferef_p       - Safe reference of the timer node created 
  
  \return TIMER_SUCCESS - In case of successful creation of Timer
  \return TIMER_FAILURE - In case of any error condition
 */
int32_t tmr_create_and_start(struct saferef* container_saferef_p, struct tmr_saferef* tmr_saferef_p,uint8_t periodic_b,
                             tmr_time_t timeout, tmr_cbk_func func_p, void *args1_p,void *args2_p);

/** \ingroup Timer_Management
  /brief  This function is used to stop the timer before its expiry\n 
  <b>Description:</b>\n
  This function is used to stop the timer before its expiry. It involves in removing the timer node
  from the bucket and freeing the memory.
 
  \param[in] container_saferef_p - Safe reference of the container to which timer is added
  \param[in] tmr_saferef_p       - Safe reference of the timer node that is created
 
  \return TIMER_SUCCESS  - In case of successful creation of the Timer 
  \return TIMER_FAILURE  - In case of any error condition
*/
int32_t tmr_stop(struct saferef* container_saferef_p, struct tmr_saferef* tmr_saferef_p);                                                                             

/** \ingroup Timer_Management
 \brief This function restarts the timer before its expiry\n
 <b>Description:</b>\n
 This function is used to restart the timer before its expiry. It involves stopping the timer  and creating
 a new timer with new timeout passed.

 \param[in]  container_saferef_p - Safe reference of the container to which timer was added
 \param[in]  timeout             - Timeout of the timer in micro seconds
 \param[in]  periodic_b          - When set to 1,a periodic timer is started instead of a regular timer.
 \param[in]  func_p              - Callback function to be called after the timer expiry
 \param[in]  args1_p             - First Argument to the callback function
 \param[in]  args2_p             - Second Arguments to the callback function
 \param[in]  tmr_saferef_p       - Safe reference of the timer node that was created and it is to be restarted now 
 \param[out] tmr_saferef_p       - Safe reference of the timer node that is created 
 
 \return TIMER_SUCCESS - In case of successful restart of Timer
 \return TIMER_FAILURE - In case of any error condition
 */
int32_t tmr_restart(struct saferef* container_saferef_p, struct tmr_saferef* tmr_saferef_p,tmr_time_t timeout,
                    uint8_t periodic_b,tmr_cbk_func func_p,void *args1_p,void *args2_p);

#ifdef OF_XML_SUPPORT
struct timer_resource_runtime_info {
    int32_t number_of_containers;
    int32_t granularity;
};

#endif

#endif /*__TIMERAPI_H*/
