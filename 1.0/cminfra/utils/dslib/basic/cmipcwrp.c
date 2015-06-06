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
 * File name: ucmipcwrp.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: 
 * 
 */

#include "cmincld.h"
#include "cmipcwrp.h"

#define CM_IPC_DEBUG(string) printf("%s:%s(%d), errno=%x, %s\n",__FILE__, __FUNCTION__, __LINE__, errno, string);

t_sem_id_t cm_sem_init (uint32_t count_ui, uint32_t flags_ui)
{
  sem_t *sem;

  if ((sem = malloc (sizeof (sem_t))) == NULL)
    return 0;

  if (sem_init (sem, 0, count_ui) == -1)
  {
    free (sem);
    return 0;
  }

  return (t_sem_id_t) sem;
}

int32_t cm_sem_destroy (t_sem_id_t sm_id_ui)
{
  if (sem_destroy ((sem_t *) sm_id_ui) == -1)
    return OF_FAILURE;

  free ((sem_t *) sm_id_ui);
  return OF_SUCCESS;
}

int32_t cm_sem_wait (t_sem_id_t sm_id_ui)
{
  if (sem_wait ((sem_t *) sm_id_ui) == -1)
    return OF_FAILURE;

  return OF_SUCCESS;
}

int32_t cm_sem_post (t_sem_id_t sm_id_ui)
{
  if (sem_post ((sem_t *) sm_id_ui) == -1)
    return OF_FAILURE;

  return OF_SUCCESS;
}

t_sem_id_t cm_mutex_init (uint32_t flags_ui)
{
  pthread_mutex_t *mutex;
#ifdef CM_LINUX_KERNEL_2_6_21
  pthread_mutexattr_t mutex_attr;
  mutex_attr.__align = PTHREAD_MUTEX_RECURSIVE_NP;
#else
  pthread_mutexattr_t mutex_attr = { PTHREAD_MUTEX_RECURSIVE_NP };
#endif /* CM_LINUX_KERNEL_2_6_21 */

  if ((mutex = malloc (sizeof (pthread_mutex_t))) == NULL)
  {
    CM_IPC_DEBUG ("malloc() failed");
    return 0;
  }
  if (pthread_mutex_init (mutex, &mutex_attr) != 0)
  {
    CM_IPC_DEBUG ("pthread_mutex_init() failed");
    free (mutex);
    return 0;
  }
  return (t_sem_id_t) mutex;
}

int32_t cm_mutex_destroy (t_sem_id_t sm_id_ui)
{
  if (pthread_mutex_destroy ((pthread_mutex_t *) sm_id_ui) == -1)
  {
    CM_IPC_DEBUG ("pthread_mutex_destroy() failed");
    return OF_FAILURE;
  }
  free ((pthread_mutex_t *) sm_id_ui);
  return OF_SUCCESS;
}

int32_t cm_mutex_lock (t_sem_id_t sm_id_ui, uint32_t flags_ui)
{
  if (flags_ui == T_SEM_NOWAIT)  /* Dont block if mutex is locked */
  {
    if (pthread_mutex_trylock ((pthread_mutex_t *) sm_id_ui) != 0)
    {
      CM_IPC_DEBUG ("pthread_mutex_trylock() failed");
      return OF_FAILURE;
    }
  }
  else
  {
    if (pthread_mutex_lock ((pthread_mutex_t *) sm_id_ui) != 0)
    {
      CM_IPC_DEBUG ("pthread_mutex_lock() failed");
      return OF_FAILURE;
    }
  }
  return OF_SUCCESS;
}

int32_t cm_mutex_unlock (t_sem_id_t sm_id_ui)
{
  if (pthread_mutex_unlock ((pthread_mutex_t *) sm_id_ui) != 0)
  {
    CM_IPC_DEBUG ("pthread_mutex_unlock() failed");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}
