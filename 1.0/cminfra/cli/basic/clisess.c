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
 * File name:  clisess.c
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file is used to create a session, update session
 *              details and remove session.
*/

#ifdef CM_CLIENG_SUPPORT
#include "cliincld.h"

/***************************************************************************
 * Global variables definition section
 ***************************************************************************/
struct cm_cli_session* cli_free_session_list_p;


/******************************************************************************
 * Function Name : cm_cli_init_session_pool
 * Input params  : none
 * Output params : none
 * Description   : This API is used to initialize session pool
 * Return value  : none
 *****************************************************************************/
int32_t cm_cli_init_session_pool(void)
{
  int32_t i;
  struct cm_cli_session* cli_session_p;

  for(i=0; i<CM_CLI_MAX_SESSIONS; i++)
  {
    cli_session_p = (struct cm_cli_session*) of_calloc(1, sizeof(struct cm_cli_session));      
    if(cli_session_p)
    {
      cm_cli_free_session(cli_session_p);
    }
    else
    {
      UCMCLIDEBUG("UCMCliInitSessions : Unable to create free sessions\n");
      break;
    }
  }
}

/******************************************************************************
 * Function Name : ucm_cli_alloc_session
 * Input params  : none
 * Output params : none
 * Description   : This API is used to allocate session
 * Return value  : cli_session_p - current Session pointer
 *****************************************************************************/
struct cm_cli_session* ucm_cli_alloc_session(void)
{
  struct cm_cli_session* cli_session_p = NULL;
  if(cli_free_session_list_p)
  {
    cli_session_p = cli_free_session_list_p;
    cli_free_session_list_p = cli_free_session_list_p->next_p;
    of_memset(cli_session_p, 0, sizeof(struct cm_cli_session));
  }
  return cli_session_p;
}

/******************************************************************************
 * Function Name : cm_cli_free_session
 * Input params  : cli_session_p - Session pointer
 * Output params : none
 * Description   : THis API is used to free session pointer
 * Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_free_session(struct cm_cli_session* cli_session_p)
{
  if(cli_session_p)
  {
    cli_session_p->next_p = cli_free_session_list_p;
    cli_free_session_list_p = cli_session_p;
    return OF_SUCCESS;
  }

  return OF_FAILURE;
}

/******************************************************************************
 * Function Name : cm_cli_clean_sessions
 * Input params  : none
 * Output params : none
 * Description   : This API is used to free session pointer allocated memory
 * Return value  : none
 *****************************************************************************/
void cm_cli_clean_sessions(void)
{
  struct cm_cli_session* cli_session_p = NULL;
  while(cli_free_session_list_p)
  {
    cli_session_p = cli_free_session_list_p;
    cli_free_session_list_p = cli_free_session_list_p->next_p;
    of_free(cli_session_p);
  }
}

#endif /*CM_CLIENG_SUPPORT*/
