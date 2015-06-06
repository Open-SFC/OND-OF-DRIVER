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
 * File name: pam_ucm_auth.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: UCM PAM module for Authentication.
 * 
 */

#ifdef CM_PAM_SUPPORT
#include <sys/param.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <security/pam_modules.h>
#include <security/pam_appl.h>
#include "ucmdb.h"

/**************** M A C R O S **********************************/
#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

#ifdef CM_PAM_DEBUG
#define PAM_CM_DEBUG printf
#else
#define PAM_CM_DEBUG(format,args...)
#endif

#define FILE_NAME_LEN 100
#define MAXPWNAM 253
#define PAM_AUTH_RESP_ATTRIBUTES  0x1000

#define PAM_FAIL_CHECK if (retval != PAM_SUCCESS) {	\
	int *pret = malloc( sizeof(int) );		\
	*pret = retval;					\
	pam_set_data( pamh, "ucm_setcred_return"	\
	              , (void *) pret, _int_free );	\
	return retval; }

/***************************************************************/
/**************** G L O B A L  V A R I A B L E S ***************/

static char conf_file[FILE_NAME_LEN]; /* configuration file */
/***************************************************************/

/**************** F U N C T I O N  P R O T O T Y P E S**********/
static int  init_cm_role_db(int argc, const char **argv);
static int cm_converse(pam_handle_t *pamh, int msg_style, char *message, char **password);

/***************************************************************/
/***************************************************************/
/* Callback function used to free the saved return value for pam_setcred. */
void _int_free( pam_handle_t * pamh, void *x, int error_status )
{
    free(x);
}
static void cleanup( pam_handle_t * pamh, void *x, int error_status );

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{
  const char *user;
  char *password = NULL;
  char* ucm_attr_val = NULL;
  int retval = PAM_AUTH_ERR;
  PAM_CM_DEBUG("\n MODULE::pam_sm_authenticate : invoked \n");
  
  /* INIT UCM ROLE DB */
  retval = init_cm_role_db(argc, argv);
  int *tpret = malloc( sizeof(int) );
  *tpret = PAM_USER_UNKNOWN;
  pam_set_data( pamh, "ucm_setcred_return", (void *) tpret, cleanup );

  /* Get the user name */
  retval = pam_get_user(pamh, &user, NULL);
  PAM_FAIL_CHECK;
  PAM_CM_DEBUG("\n MODULE:: Got user %s\n", user);

  /* check that they've entered something, and not too long, either */
  if ((user == NULL) ||
      (strlen(user) > MAXPWNAM)) {
    int *pret = malloc( sizeof(int) );
    *pret = PAM_USER_UNKNOWN;
    pam_set_data( pamh, "ucm_setcred_return", (void *) pret, _int_free );

    PAM_CM_DEBUG("\n MODULE::User name was NULL, or too long\n");
    return PAM_USER_UNKNOWN;
  }

  /* Get the password (if any) from the previous authentication layer */
  retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **) &password);
  PAM_FAIL_CHECK;

  if(password) {
    password = strdup(password);
    PAM_CM_DEBUG("\n MODULE:: Got password %s\n", password);
  }
  else
  {
    cm_converse(pamh, PAM_PROMPT_ECHO_OFF, "Password:", &password);
  }
    PAM_CM_DEBUG("\n MODULE ::Got password %s\n", password);

   /* Fetch from UCM DB */
   ucm_attr_val = get_role_info(user, password);
   PAM_CM_DEBUG("\n MODULE: ucm_attr_val = %s \n", ucm_attr_val);

  if(ucm_attr_val == NULL)
  {
    if(password)
      free(password);
    int *pret = malloc( sizeof(int) );
    *pret = PAM_USER_UNKNOWN;
    pam_set_data( pamh, "ucm_setcred_return", (void *) pret, _int_free );

    PAM_CM_DEBUG("\n MODULE:: Unknown User.\n");
    return PAM_PERM_DENIED;
  }
  /*Send Response to client*/
  cm_converse(pamh, PAM_AUTH_RESP_ATTRIBUTES, ucm_attr_val, NULL);
  //free(ucm_attr_val);
  if(password)
  free(password);

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_setcred(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{
  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);
  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_acct_mgmt(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{
  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_open_session(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{
  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_close_session(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{
  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);

  return (PAM_SUCCESS);
}

PAM_EXTERN int
pam_sm_chauthtok(pam_handle_t *pamh, int flags,
  int argc, const char *argv[])
{

  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);
  return (PAM_SERVICE_ERR);
}

/*****   Local functions     ******/
static int cm_converse(pam_handle_t *pamh, int msg_style, char *message, char **password)
{
  const struct pam_conv *conv;
  struct pam_message resp_msg;
  const struct pam_message *msg[1];
  struct pam_response *resp = NULL;
  int retval;
  
  resp_msg.msg_style = msg_style;
  resp_msg.msg = message;
  msg[0] = &resp_msg;
  
  /* grab the password */
  retval = pam_get_item(pamh, PAM_CONV, (const void **) &conv);
  PAM_FAIL_CHECK;
  
  retval = conv->conv(1, msg, &resp,conv->appdata_ptr);
  PAM_FAIL_CHECK;
  
  if (password) {		/* assume msg.type needs a response */
#ifdef sun
    /* NULL response, fail authentication */
    if ((resp == NULL) || (resp->resp == NULL)) {
      return PAM_SYSTEM_ERR;
    }
#endif
    
    *password = resp->resp;
    free(resp);
  }
  
  return PAM_SUCCESS;
}
static void cleanup(pam_handle_t * pamh, void *x, int error_status )
{
  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);
  free(x);
  clean_role_info();
}

static int  init_cm_role_db(int argc, const char **argv)
{
  int ctrl=0;

  PAM_CM_DEBUG(" \n MODULE:: %s : invoked \n", __FUNCTION__);
  /* step through arguments */
  for (ctrl=0; argc-- > 0; ++argv) {

    /* generic options */
    if (!strncmp(*argv,"conf=",5)) {
      strcpy(conf_file,*argv+5);

    }
  }
  PAM_CM_DEBUG(" \n MODULE:: %s : conf_file = %s\n", __FUNCTION__, conf_file);
  cm_load_user_db(conf_file);
  return 0;
}

#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_ucm_auth");
#endif

#endif /*CM_PAM_SUPPORT*/
