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
 * File name:   ldsvmain.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: LDSV APIs.
*/

#ifdef CM_LDSV_SUPPORT

#include "ldsvinc.h"
#include "ldsvldef.h"
#include "ldsvlif.h"
#include "cmtransgdf.h"
#include <signal.h>

struct cm_poll_info cmLDSVFDList[CM_LDSV_FDPOLL_MAXFDS];
#define CMLDSV_SCHEDULING_PRIORITY -19

cm_sock_handle ulLDSVListenSock_g;
int32_t daemon_init (void);

main (int32_t argc, char * argv[])
{
  CM_LDSV_DEBUG_PRINT ("Entered in main method ");
  signal (SIGPIPE, SIG_IGN);
  if ((argc == 1) || ((argc == 2) && (strcmp (argv[1], "-f"))))
  {
    daemon_init ();
  }

  if (nice (CMLDSV_SCHEDULING_PRIORITY) != OF_SUCCESS)
  {
    //return;
  }

  cm_ldsv_init ();
  cm_ldsv_daemon ();
}

int32_t cm_ldsv_init (void)
{
  CM_LDSV_DEBUG_PRINT ("Entered");
  ulLDSVListenSock_g = cm_ldsv_create_listen_socket ();
  if (ulLDSVListenSock_g == 0)
  {
    CM_LDSV_DEBUG_PRINT ("Creating Listen Socket Failed");
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

#endif /* CM_LDSV_SUPPORT */
