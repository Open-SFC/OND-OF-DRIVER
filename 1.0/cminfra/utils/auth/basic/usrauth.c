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
 * File name: usrauth.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: UCM PAM User Authentication.
 * 
 */

#ifdef CM_SUPPORT
#ifdef CM_AUTH_SUPPORT

#include "authinc.h"

#ifdef CM_PAM_SUPPORT
#include <security/pam_appl.h>
#include <openssl/evp.h>
#include "ucmmd5.h"
#endif

#ifdef CM_AUTH_DEBUG
#define CM_AUTH_DEBUG_PRINT printf("\n%35s(%4d)::", __FUNCTION__, __LINE__),printf
#else
#define CM_AUTH_DEBUG_PRINT(format,args...)
#endif

#define PASSWD_MAX_LEN 33

#ifdef OF_CM_AUTH_PLUGIN_SUPPORT
int32_t cm_user_management(uint32_t uiCmd, struct cm_auth_mgmt *pAuth);
int32_t cm_auth_init(void);
#endif /*OF_CM_AUTH_PLUGIN_SUPPORT*/

char aAuthRole_g[CM_MAX_ADMINROLE_LEN];

#ifdef CM_PAM_SUPPORT
int my_conv (int num_msg, const struct pam_message **msg,
             struct pam_response **resp, void *appdata_ptr);

static struct pam_conv conv = {
  my_conv,
  NULL
};
#endif /* CM_PAM_SUPPORT */

#ifdef OF_CM_AUTH_PLUGIN_SUPPORT
int32_t cm_auth_init()
{
  if(cm_register_auth_function(cm_user_management) != OF_SUCCESS)
  {
    CM_AUTH_DEBUG_PRINT ("Failed to register the function.\n");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

int32_t cm_user_management(uint32_t uiCmd, struct cm_auth_mgmt *pAuth)
{
  int32_t return_value = 0;
  char aPaswd[PASSWD_MAX_LEN];

  of_memset(aPaswd, 0, sizeof(aPaswd));

  switch(uiCmd)
  {
    case CM_AUTH_USR_ADD_CMD:
      break;
    case CM_AUTH_USR_MOD_CMD:
      break;
    case CM_AUTH_USR_DEL_CMD:
      break;
    case CM_AUTH_USR_AUTH_CMD:
#ifdef CM_PAM_SUPPORT
      cm_md5_encrypt_buf(pAuth->pPwd, aPaswd);
      CM_AUTH_DEBUG_PRINT("\n*** MD5 passwd= %s \n", aPaswd);
#else
      of_strcpy(aPaswd, pAuth->pPwd);
#endif
      return_value = 
         cm_pam_authenticate_user(pAuth->pUsrName, aPaswd, &pAuth->role_info_p);
      if(return_value != OF_SUCCESS)
      {
        CM_AUTH_DEBUG_PRINT ("Authentication Failed.\n");
        return OF_FAILURE;
      }
      break;
  }
  return OF_SUCCESS;
}
#endif /*OF_CM_AUTH_PLUGIN_SUPPORT*/

int32_t cm_pam_authenticate_user (char * user_name_p, char * password_p,
                                struct cm_role_info ** role_info_pp)
{
#ifdef CM_PAM_SUPPORT
  pam_handle_t *pPamh = NULL;
  int32_t return_value;
  struct passwd *pPwd;
  struct spwd *pShadowPwd;
  char *pCrypt_password = NULL;
  conv.appdata_ptr = (void *) password_p;
#endif /* CM_PAM_SUPPORT */
//  char aBuffer[50];

  if (*role_info_pp == NULL)
  {
    *role_info_pp = (struct cm_role_info *) of_calloc (1, sizeof (struct cm_role_info));
    if (*role_info_pp == NULL)
    {
      return OF_FAILURE;
    }
  }

#ifdef CM_PAM_SUPPORT
  return_value = pam_start ("ucm", user_name_p, &conv, &pPamh);
  if (return_value == PAM_SUCCESS)
    return_value = pam_authenticate (pPamh, 0);      /* is user really user? */

  if (return_value == PAM_SUCCESS)
  {
    CM_AUTH_DEBUG_PRINT ("PAM Authentication Success \n");
    of_strcpy ((*role_info_pp)->admin_role, aAuthRole_g);
    return OF_SUCCESS;
  }
  else
  {
    CM_AUTH_DEBUG_PRINT ("PAM Authentication Failed \n");
    return OF_FAILURE;
  }
  return OF_FAILURE;
#if 0
  if (return_value == PAM_SUCCESS)
    return_value = pam_acct_mgmt (pPamh, 0); /* permitted access? */

  /* This is where we have been authorized or not. */
  if (return_value == PAM_SUCCESS)
  {
    CM_AUTH_DEBUG_PRINT ("Authenticated Successfully\n");
  }
  else
  {
    CM_AUTH_DEBUG_PRINT ("Not Authenticated\n");
    return OF_FAILURE;
  }
#endif
#else
  if ((of_strcmp (user_name_p, "root") == 0)
      || (of_strcmp (user_name_p, "admin") == 0))
  {
    if ((of_strcmp (password_p, "root") == 0)
        || (of_strcmp (password_p, "admin") == 0))
    {
      of_strcpy ((*role_info_pp)->admin_role, "AdminRole");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "UCM") == 0)
  {
    if (of_strcmp (password_p, "UCM") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "UCMRole");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "guest") == 0)
  {
    if (of_strcmp (password_p, "guest") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "guest");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "user") == 0)
  {
    if (of_strcmp (password_p, "user") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "userRole");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "fwadmin") == 0)
  {
    if (of_strcmp (password_p, "fwadmin") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "fwAdmin");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "fwsadmin") == 0)
  {
    if (of_strcmp (password_p, "fwsadmin") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "fwSAdmin");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "fwuser") == 0)
  {
    if (of_strcmp (password_p, "fwuser") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "fwUser");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "cuser") == 0)
  {
    if (of_strcmp (password_p, "cuser") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "commonUser");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "cadmin") == 0)
  {
    if (of_strcmp (password_p, "cadmin") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "commonAdmin");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "csadmin") == 0)
  {
    if (of_strcmp (password_p, "csadmin") == 0)
    {
      of_strcpy ((*role_info_pp)->admin_role, "commonSAdmin");
      return OF_SUCCESS;
    }
    CM_AUTH_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else
  {
    CM_AUTH_DEBUG_PRINT ("Invalid UserName/Password.");
    return OF_FAILURE;
  }
  return OF_FAILURE;
#endif
}

#ifdef CM_PAM_SUPPORT
#define PAM_AUTH_RESP_ATTRIBUTES  0x1000
int my_conv (int num_msg, const struct pam_message **msg,
             struct pam_response **resp, void *appdata_ptr)
{
  int i;
  char *password = NULL;
  struct pam_response *myresp = NULL;

  //printf("suren_remove: Conversation Callback : Entry. Msg = %s\n",
  //                                                  msg[0]->msg);
  for (i = 0; i < num_msg; i++)
  {
    switch (msg[0]->msg_style)
    {
      case PAM_PROMPT_ECHO_OFF:
      case PAM_PROMPT_ECHO_ON:
        if (!strncmp (msg[0]->msg, "Password", strlen ("Password")))
        {
          //printf("suren_remove: Conversation Callback : Asking for password \n");
          myresp =
            (struct pam_response *) calloc (1, sizeof (struct pam_response));
          if (!myresp)
            return PAM_CRED_ERR;
          password = (char *) calloc (1, strlen ((char *) appdata_ptr) + 1);
          if (!password)
          {
            free (myresp);
            return PAM_CRED_ERR;
          }
          strcpy (password, (char *) appdata_ptr);
          myresp->resp = password;
          myresp->resp_retcode = 0;
          *resp = myresp;
        }
        break;
      case PAM_AUTH_RESP_ATTRIBUTES:
        if (msg[0]->msg)
        {
          // printf("suren_remove: Conversation Callback : Got Radius Attribute: %s\n", msg[0]->msg);
          of_memset (aAuthRole_g, 0, CM_MAX_ADMINROLE_LEN);
          of_strncpy (aAuthRole_g, msg[0]->msg, CM_MAX_ADMINROLE_LEN);
        }

        break;
      case PAM_ERROR_MSG:
      case PAM_TEXT_INFO:
        /* We will add more here */
      default:
        return PAM_CRED_ERR;
    }
  }

  return PAM_SUCCESS;
}

#endif /* CM_PAM_SUPPORT */
#endif /* CM_AUTH_SUPPORT */
#endif /* CM_SUPPORT */
