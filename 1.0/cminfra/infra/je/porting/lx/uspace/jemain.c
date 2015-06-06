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
 * File name: jemain.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"
#include "evntgdef.h"
#include "evntgif.h"
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include "cmgutil.h"
#define UCMJE_SCHEDULING_PRIORITY -19
#define CM_MAX_SECAPPS 99
#define CM_SECAPP_LIB_SEQ_MAX_LEN 3
/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
UCMDllQ_t cm_je_tnsprtchannel_list_g;
UCMDllQ_t cm_je_request_queue_g;
cm_sock_handle je_listen_sock_g;
UCMDllQ_t UCMJEGloTransaction_g;
#ifdef CM_JE_STATS_SUPPORT
int32_t cm_je_global_stats_g[JE_STATS_MAX]={};
int32_t cm_je_session_stats_g[JE_SESSION_STATS_MAX]={};
int32_t cm_je_appl_stats_g[JE_SECAPPL_STATS_MAX]={};
#endif
unsigned char bLdsvInitialized_g=FALSE;
/******************************************************************************
 * * * * * * * * * * * * * * * FUNCTION PROTOTYPES * * * * * * * * * * * * * * 
 *****************************************************************************/
int32_t daemon_init (void);
 int32_t cm_je_init (void);
int32_t cm_sec_appl_init(void);

/******************************************************************************
 * * * * * * * * * * * * * FUNCTION DEFINITIONS * * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**
  \ingroup JEAPI
  This function is entry point for JE. It calls all initilization routines for
  JE and invokes je daemon.

  <b>Input paramameters: </b>\n
  <b><i>argc </i></b> Number of Arguments
  <b><i>argv </i></b> Argument Vector
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : main
 ** Description   : This function is Entry Point for JE. Calls all initialization
 **                  routines and invokes JE Daemon.
 ** Input params  : argc - Number of Arguments
 **                : argv - Argument Vector
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
main (int32_t argc, char * argv[])
{
   int32_t argIndex;
#ifdef CM_LDSV_SUPPORT
   unsigned char bUCMNoLoadCfg=FALSE;
#endif

   signal (SIGPIPE, SIG_IGN);
   if ((argc == 1) || ((argc == 2) && (of_strcmp (argv[1], "-f"))))
   {
      daemon_init ();
   }

   if (nice (UCMJE_SCHEDULING_PRIORITY) != OF_SUCCESS)
   {
      CM_JE_DEBUG_PRINT ("Couldn't set priority");
   }

   cm_je_init ();
   cm_dm_init ();
#ifdef CM_ROLES_PERM_SUPPORT
   cmi_dm_role_perm_init();
#endif
#ifdef CM_EVENT_NOTIFMGR_SUPPORT
   cm_evmgr_register_all_events();
#endif /*CM_EVENT_NOTIFMGR_SUPPORT */


//#ifdef OF_CM_CBKINI_SUPPORT
#if OF_CM_CNTRL_SUPPORT
   UCMCBKInit();
#endif

#ifdef OF_CM_SWITCH_CBK_SUPPORT
   UCMSwitchCBKInit();
#endif


#ifdef OF_UCMGLU_SUPPORT
#ifdef CM_SECAPPL_DLL_SUPPORT
  // IGWUCMRootInit ();         /* Call back Library */
   //cm_sec_appl_init();
#else
  // IGWUCMRootInit ();         /* Call back Library */
#endif
#endif


#ifdef CM_EVENT_NOTIFMGR_SUPPORT
   cm_evmgr_init();
#endif

#ifdef CM_LDSV_SUPPORT
   for (argIndex=1; argIndex < argc; argIndex++)
   {
      if (!of_strcmp (argv[argIndex],CM_JE_NO_LOAD_CONFIGURATION))
      {
         bUCMNoLoadCfg=TRUE;
         break;
      }
   }
   if(bUCMNoLoadCfg==FALSE)
   {
      cm_je_ldsv_init ();
   }
#endif
   cm_je_daemon ();
}

/**
  \ingroup JEAPI
  This function invokes all JE initialization routines. It initializes 
  Mempool,ID Generator, Timer, je config version.

  <b>Input paramameters: </b> None\n
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
  OF_FAILURE in Failure case.\n
  */
/******************************************************************************
 ** Function Name : cm_je_init 
 ** Description   : This function invokes all JE initialization routines. 
 **                  It initializes Mempool,ID Generator, Timer, je config version.
 ** Input params  : None
 ** Output params : None
 ** Return value  : OF_SUCCESS in Success
 **                  OF_FAILURE in Failure case
 *******************************************************************************/
int32_t cm_je_init (void)
{
   CM_JE_DEBUG_PRINT ("Entered");
   je_listen_sock_g = cm_je_create_listen_socket ();
   if (je_listen_sock_g == 0)
   {
      CM_JE_DEBUG_PRINT ("Creating Listen Socket Failed");
      return OF_FAILURE;
   }
   CM_DLLQ_INIT_LIST (&cm_je_tnsprtchannel_list_g);
   CM_DLLQ_INIT_LIST (&cm_je_request_queue_g);
   CM_DLLQ_INIT_LIST (&UCMJEGloTransaction_g);

   cm_je_create_cfgsession_mempool ();
   cm_je_create_cfgcache_mempool ();
   cm_je_create_cfgrequest_mempool ();
   cm_je_create_tnsprtchannel_mempool ();
   je_instanceid_generator_init ();
   cm_timer_init ();
   CM_INIT_JE_STATS;
   CM_INIT_JESESSION_STATS;
   CM_INIT_JESECAPPL_STATS;
#ifdef CM_VERSION_SUPPORT
   cm_vh_config_verion_init ();
#endif
   return OF_SUCCESS;
}


#ifdef CM_SECAPPL_DLL_SUPPORT
int32_t cm_sec_appl_init(void)
{
   int32_t lRetVal=OF_FAILURE;
   DIR *pDir =NULL;
   struct dirent *dp;
   char  path[CM_JE_FILE_MAX_LEN+1];
   uint32_t uiCnt;
   char *pFile=NULL;
   char aLibSequence[CM_SECAPP_LIB_SEQ_MAX_LEN];


   for(uiCnt=0; uiCnt < CM_MAX_SECAPPS; uiCnt++)
   {
      of_memset(path, CM_JE_FILE_MAX_LEN, 0);
      of_strcpy(path,CM_SECAPP_LIB_PATH);
      pDir = opendir(path);
      if(!pDir)
      {
         CM_JE_DEBUG_PRINT("diropen for path %s failed", path);
         return OF_FAILURE;
      }
      while((dp = readdir(pDir))!= NULL)
      { 
         if(!(strcmp(dp->d_name,"."))||!(strcmp(dp->d_name,"..")))
         {
            continue;
         }
         of_memset(aLibSequence,0, CM_SECAPP_LIB_SEQ_MAX_LEN);
         sprintf(aLibSequence,"%02d",uiCnt);
         pFile=strstr( dp->d_name,aLibSequence);
         if(!pFile)
         {
            continue;
         }
         if(pFile)
         {
            CM_JE_DEBUG_PRINT("filename=%s",dp->d_name);

            sprintf(path,
                  "%s/%s",CM_SECAPP_LIB_PATH,dp->d_name);

            CM_JE_DEBUG_PRINT("path:%s",path);

            if((lRetVal =  cm_dl_open(path,"cm_init"))!=OF_SUCCESS)
            {
               CM_JE_DEBUG_PRINT("dll open failed for %s", path);
               closedir(pDir);
               exit(1);
            }
         }   
      }
      closedir(pDir);
   }
   return OF_SUCCESS;
}
#endif /* CM_SECAPPL_DLL_SUPPORT */
#endif /* CM_JE_SUPPORT */
#endif /* CM_SUPPORT */
