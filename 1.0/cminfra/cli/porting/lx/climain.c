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
 * File name:  climain.c
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file is the Entry Point for CLI Engine and calls all the
 *              initialization routines and invokes CLI Engine.
*/

#ifdef CM_CLIENG_SUPPORT
#include "cliincld.h"
#include "dmgif.h"
#include "clicmd.h"
/***************************************************************************
 * Global variables definition section
 ***************************************************************************/
struct cm_cli_session *cli_session_p;
SIGACTION sigact;

#define CM_PAUTHPLUGIN_LIB_PATH "/igateway/ucm/lib/authplugin"

#define CM_CLI_LOGOUT_CMD "logout"
#define CM_CLI_REBOOT_CMD "reboot"
#define CM_CLI_EXIT_CMD "exit"
#define CM_CLI_QUIT_CMD "quit"

#ifdef OF_CM_CNTRL_SUPPORT 
#define CM_CLI_START_TAG "\t\t*  Welcome to VortiQa Open Network (ON) Director  *\n\r"
#endif
#ifdef OF_CM_SWITCH_CBK_SUPPORT
#define CM_CLI_START_TAG "\t\t*  Welcome to VortiQa Open Network (ON) Switch  *\n\r"             
#endif
/***************************************************************************
 * Local function prototype section
 ***************************************************************************/
int got_sig;

#ifdef OF_CM_AUTH_PLUGIN_SUPPORT
int32_t cm_auth_plugin_init(void);
#endif

int32_t cm_cli_setup_signal_handlers (void);
 void sig (int32_t i);

 void sig (int32_t i)
{
  got_sig = i;
}

/**
 \in group CLI API
  This function is entry point for CLI Engine. It calls all initialization routines
  for CLI and invokes CLI Process.

  <b>Input paramameters: </b>\n
  <b><i>argc </i></b> Number of Arguments
  <b><i>argv </i></b> Argument Vector
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */
/******************************************************************************
 * Function Name : main
 * Description   : This function is Entry Point for CLI Engine. Calls all the
 *                 initialization routines and invokes CLI Engine.
 * Input params  : argc - Number of Arguments
 *               : argv - Argument Vector
 * Output params : None
 * Return value  : OF_SUCCESS in Success
 *                 OF_FAILURE in Failure case
 *******************************************************************************/
int32_t main (int32_t argc, char * argv[])
{
  /*const*/ char *line_p;
  uint32_t count_ui, start_len_ui = 0, end_len_ui = 0, pos_ui = 0;
  unsigned char log_out_b = FALSE, cmd_exit_b = FALSE , reboot_b = FALSE;
  int32_t return_value;
  unsigned char user_name_a[CM_MAX_ADMINNAME_LEN + 1]={0};
  unsigned char password_a[CM_MAX_ADMINNAME_LEN + 1]={0};
  char *cmd_p = NULL;
  char *temp_p = NULL;

  /* Allocate a free session */
  cli_session_p = (struct cm_cli_session *) of_calloc (1, sizeof (struct cm_cli_session));
  if (!cli_session_p)
  {
    UCMCLIDEBUG ("Unable to allocate CLI session\n");
    exit (1);
  }
  /*Initialize authentication module */
#ifdef OF_CM_AUTH_PLUGIN_SUPPORT
  cm_auth_plugin_init();
#endif

  /*Initialize editline */
  cli_session_p->edit_line_ctx_p = of_el_init (argv[0], stdin, stdout, stderr);
  if (cli_session_p->edit_line_ctx_p == NULL)
  {
    UCMCLIDEBUG ("Edit line initiation failed\n\r");
    of_free (cli_session_p);
    exit (1);
  }

  /* Set default editor */
  of_el_set (cli_session_p->edit_line_ctx_p, EL_EDITOR, "vi");

  /* Set CLI context pointer here. We can get it back any time, 
   * especially while displaying the prompt 
   */
  of_el_set (cli_session_p->edit_line_ctx_p, EL_CLIENTDATA, cli_session_p);

  /* To handle signals gracefully */
  of_el_set (cli_session_p->edit_line_ctx_p, EL_SIGNAL, 1);

  /* Adding user defined function */
  of_el_set (cli_session_p->edit_line_ctx_p, EL_ADDFN, "ed-complete",
            "Complete argument", cm_cli_auto_complete);

  /* Bind TAB to function */
  of_el_set (cli_session_p->edit_line_ctx_p, EL_BIND, "^I", "ed-complete", NULL);

  /* Initialize the history_t */
  cli_session_p->history_p = history_init ();
  if (cli_session_p->history_p == 0)
  {
    UCMCLIDEBUG ("Unable to initialize history_t!!\n\r");
    of_el_end (cli_session_p->edit_line_ctx_p);
    of_free (cli_session_p);
    exit (1);
  }

  /* Set the size of the history_t */
  of_history (cli_session_p->history_p, &(cli_session_p->hist_event), H_SETSIZE,
             CM_CLI_MAX_HISTORY_LINES);

  /* Setup callback routines for history_t */
  of_el_set (cli_session_p->edit_line_ctx_p, EL_HIST, history, cli_session_p->history_p);

  if (cm_cli_setup_signal_handlers () != OF_SUCCESS)
  {
    UCMCLIDEBUG ("CliSetupSignalHandlers : sigaction error");
    of_history_end (cli_session_p->history_p);
    of_el_end (cli_session_p->edit_line_ctx_p);
    of_free (cli_session_p);
    exit (1);
  }

  /* After initialising the CLI session start the Transport connection with JE */
  if ((cli_session_p->tnsprt_channel_p =
       (struct cm_tnsprt_channel *) cm_cli_establish_transport_channel ()) == NULL)
  {
    UCMCLIDEBUG ("\r\nCannot create socket\r\n");
    cm_cli_puts (cli_session_p,"\r\n Unable to connect to the server.\r\n");
    of_history_end (cli_session_p->history_p);
    of_el_end (cli_session_p->edit_line_ctx_p);
    of_free (cli_session_p);
    exit (1);
  }

  cm_cli_mem_init ();

  /*if((return_value = cm_cli_user_login(user_name_a, password_a)) != OF_SUCCESS)
  {
    UCMCLIDEBUG ("\r\nUser Name / Password authentication failed.\r\n");
    of_history_end (cli_session_p->history_p);
    of_el_end (cli_session_p->edit_line_ctx_p);
    of_free (cli_session_p);
    exit (1);
  }*/

  cm_cli_puts (cli_session_p,
              "\n\t\t****************************************************\n\r");
#if defined(CM_MGMT_CARD_SUPPORT) || defined (OF_CM_ITCM_SUPPORT)
  cm_cli_puts (cli_session_p,
              "\t\t*                WELCOME TO CM CLI               *\n\r");
#else
  cm_cli_puts (cli_session_p, CM_CLI_START_TAG); 
#endif
  cm_cli_puts (cli_session_p,
              "\t\t****************************************************\n\r");
  cm_cli_puts (cli_session_p, "\t\t    Use \'help\' command to list commands and usage\n\r");
  cm_cli_puts (cli_session_p,
              "\t\t****************************************************\n\r");

  /*OK, Ready to read and process input commands */
  while (TRUE)
  {
    alarm (CM_CLI_INACTIVITY_TIMEOUT);
    of_el_set (cli_session_p->edit_line_ctx_p, EL_PROMPT, &cm_cli_get_context_prompt);

    /* Read one line from standard input */
    line_p = of_el_gets (cli_session_p->edit_line_ctx_p, &count_ui);

    if ((line_p == NULL) || (count_ui == 0))
    {
      continue;
    }

    /* In order to use our history_t, we have to explicitly add commands to the history_t */
    if (count_ui > 0)
    {
      /* Avoid storing new-line characters in history_t. */
      if (of_strcmp (line_p, "\n"))
      {
        of_history (cli_session_p->history_p, &(cli_session_p->hist_event), H_ENTER, line_p);

  cmd_exit_b = FALSE;
  temp_p = (char*)line_p;
  cmd_p = of_calloc(1, of_strlen(line_p)+1);
  if(cmd_p == NULL)
  {
           if (!(of_strncmp (line_p, CM_CLI_LOGOUT_CMD, of_strlen(CM_CLI_LOGOUT_CMD))) /*|| !(of_strncmp (line_p, CM_CLI_EXIT_CMD, of_strlen (CM_CLI_EXIT_CMD)))*/ || !(of_strncmp (line_p, CM_CLI_QUIT_CMD, of_strlen(CM_CLI_QUIT_CMD))) /* || !(of_strncmp(line_p,CM_CLI_REBOOT_CMD,of_strlen(CM_CLI_REBOOT_CMD)))*/)
    {
       cmd_exit_b = TRUE;
    }
  }
  else
  {
    pos_ui = 0;
    start_len_ui = 0;
    end_len_ui = 0;
          /*if (!(of_strncmp (line_p, CM_CLI_LOGOUT_CMD, of_strlen(CM_CLI_LOGOUT_CMD))) || !(of_strncmp (line_p, CM_CLI_EXIT_CMD, of_strlen (CM_CLI_EXIT_CMD))) || !(of_strncmp (line_p, CM_CLI_QUIT_CMD, of_strlen(CM_CLI_QUIT_CMD))))*/
          if ((of_strstr (line_p, CM_CLI_LOGOUT_CMD)) /*|| (of_strstr(line_p, CM_CLI_EXIT_CMD)) */|| (of_strstr (line_p, CM_CLI_QUIT_CMD)) || of_strstr(line_p,CM_CLI_REBOOT_CMD))
    {
     while(*(temp_p+pos_ui) != '\0')
     {
      if(!isspace(*(temp_p+pos_ui))) //if it is not space and not new line
      {
    start_len_ui = pos_ui;
    break;
      }
      else
      {
    pos_ui++;
      }
    }
    pos_ui = 0;
    pos_ui = of_strlen(temp_p)-1;
    while(*(temp_p + pos_ui) != '\0')
    {
      if( !isspace(*(temp_p + pos_ui)) && (*(temp_p + pos_ui )!= '\n') ) //if it is not space and not new line
      {
    break;
      }
      else
      {
    pos_ui--;
    end_len_ui++;
      }
    }
    of_strncpy(cmd_p, line_p+start_len_ui, of_strlen(line_p)-start_len_ui-end_len_ui);
  
    if (!(of_strncmp (cmd_p, CM_CLI_LOGOUT_CMD,of_strlen(CM_CLI_LOGOUT_CMD))) /*||
            !(of_strcmp (cmd_p, CM_CLI_EXIT_CMD))*/ ||
              !(of_strncmp (cmd_p, CM_CLI_QUIT_CMD,of_strlen(CM_CLI_QUIT_CMD))) ||
		!(of_strncmp(cmd_p,CM_CLI_REBOOT_CMD,of_strlen(CM_CLI_REBOOT_CMD))))
    {
	   cmd_exit_b = TRUE;
         if(!(of_strncmp(cmd_p,CM_CLI_REBOOT_CMD,of_strlen(CM_CLI_REBOOT_CMD))))
	       reboot_b = TRUE;
       #ifndef CONFIGSYNC_SUPPORT
	   if(reboot_b == TRUE)
           {
             reboot_b = FALSE;
             cmd_exit_b = FALSE;
           }
       #endif
    }
   }
  }
  if(cmd_exit_b == TRUE)
  {
          if (cli_session_p->config_session_p != NULL)
          {
            cm_cli_puts (cli_session_p,
                        "Config Session exists. Enter \'commit\' or \'revoke\' to close the Session.\n\r");
            log_out_b = TRUE;
	 
            continue;
          }
          else
          {
            break;
          }
 }
    of_free(cmd_p);
    cmd_p = NULL;

   /* Call cm_cli_interpret_command() */
   cm_cli_interpret_command (cli_session_p, line_p, FALSE);
  if (log_out_b == TRUE || reboot_b == TRUE)
  {
    if (cli_session_p->config_session_p == NULL)
    {
      break;
    }
    else
    {
      cm_cli_puts (cli_session_p,"Enter \'commit\' or \'revoke\' to close the Session.\n\r");
      continue;
    }
  }
      }// \n pressed
    }// Count > 0
  }//while

#ifdef CONFIGSYNC_SUPPORT
  if(reboot_b == TRUE)
    cm_cli_reboot_device_in_config_session(cli_session_p);
#endif  
  if (cli_session_p->config_session_p)
  {
    cli_session_p->config_session_p = NULL;
  }

  cm_cli_terminate_transport_channel (cli_session_p->tnsprt_channel_p);

  /* Clean up our memory */
  if (cli_session_p->current_node_p)
  {
    of_free (cli_session_p->current_node_p);
  }
  of_history_end (cli_session_p->history_p);
  of_el_end (cli_session_p->edit_line_ctx_p);
  of_free (cli_session_p);
  return OF_SUCCESS;
}

#ifdef OF_CM_AUTH_PLUGIN_SUPPORT

int32_t cm_auth_plugin_init(void)
{
   int32_t l_ret_val=OF_FAILURE;
   DIR *dir_p =NULL;
   struct dirent *dp;
   char  path[64]={'\0'};

   of_strcpy(path,CM_PAUTHPLUGIN_LIB_PATH);
  
   dir_p = opendir(path);
   if(!dir_p)
   {
     UCMCLIDEBUG("%s():diropen for %s failed\n\r",__FUNCTION__,path);
     return OF_FAILURE;
   }

   while((dp = readdir(dir_p))!= NULL)
   { 
     UCMCLIDEBUG("%s: filename=%s\n",__FUNCTION__,dp->d_name);
     if(!(strcmp(dp->d_name,"."))||!(strcmp(dp->d_name,"..")))
     {
       continue;
     }

     sprintf (path,
             "%s/%s",CM_PAUTHPLUGIN_LIB_PATH,dp->d_name);

     UCMCLIDEBUG("%s(): path:%s\n\r",__FUNCTION__,path);
   
     if((l_ret_val =  cm_dl_open(path,"cm_auth_init"))!=OF_SUCCESS)
     {
       UCMCLIDEBUG("%s:failed \n",__FUNCTION__);
       closedir(dir_p);
       exit(1);
     }  
   }
   closedir(dir_p);
   return OF_SUCCESS;
}
#endif /* OF_CM_AUTH_PLUGIN_SUPPORT */

/**
 \ingroup CLIAPI
  This API is used to set-up the signal handlers for various signals received
  by CLI engine. 
 
  <b>Input paramameters: </b> None\n
  <b>Output Parameters: </b> None\n
  <b>Return Value:</b> OF_SUCCESS in Success
    OF_FAILURE in Failure case.\n
 */

/******************************************************************************
 * Function Name : cm_cli_setup_signal_handlers
 * Input params  : NONE
 * Output params : NONE
 * Description   : This API is used to set-up signal handlers for various
 *               : signals received by CLI engine.
 * Return value  : OF_SUCCESS / OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_setup_signal_handlers (void)
{
  sigact.sa_handler = (void (*)(int32_t)) cm_cli_handle_signal;

  signal (SIGINT, sig);
  sigaddset (&sigact.sa_mask, SIGTERM);
  sigaddset (&sigact.sa_mask, SIGQUIT);
  sigaddset (&sigact.sa_mask, SIGHUP);
  sigaddset (&sigact.sa_mask, SIGALRM);

  sigact.sa_flags = 0;
  sigact.sa_restorer = NULL;

  if (sigaction (SIGQUIT, &sigact, NULL) < 0 ||
      sigaction (SIGTERM, &sigact, NULL) < 0 ||
      sigaction (SIGHUP, &sigact, NULL) < 0 ||
      sigaction (SIGALRM, &sigact, NULL) < 0)
  {
    UCMCLIDEBUG ("%s :: SigAction returned failure\n\r", __FUNCTION__);
    return OF_FAILURE;
  }

  return OF_SUCCESS;
}

#endif
