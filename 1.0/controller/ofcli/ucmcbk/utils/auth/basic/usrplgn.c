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
 * File name: usrplgn.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifdef OF_CM_SUPPORT
#ifdef CM_AUTH_SUPPORT

#include "cbcmninc.h"
#include "dmgif.h"
#include "cmgutil.h"
#include "authgdef.h"
#include "authgif.h"
#include "jegif.h"

#define CM_SNET_GENERAL 0

int32_t cm_auth_userInit(void);

int32_t UCMAuthenticateUserwithUserDB (unsigned char * user_name_p, unsigned char * password_p,
                                        struct cm_role_info *role_info_p);
int32_t UCMRegisterAuthUserFunction(cm_auth_user pAuthFunPtr);

int32_t cm_auth_userInit()
{
  if(UCMRegisterAuthUserFunction(UCMAuthenticateUserwithUserDB) != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Failed to register the function.\n");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}


int32_t UCMAuthenticateUserwithUserDB (unsigned char * user_name_p, unsigned char * password_p,
                                          struct cm_role_info *role_info_p)
{

/*
  int32_t         return_value = OF_FAILURE;
  uint32_t        ulSnetId;
  int32_t         InactivityTimer = 0;
  uint8_t         Privileges      = 0;
#define CM_CHALLENGE_LEN 64
  char         aChallenge[CM_CHALLENGE_LEN+1] = {};
  UserDbTemp_t    UserInfo;

  of_memset(&UserInfo, 0, sizeof UserInfo);
*/

  if (role_info_p == NULL)
  {
    CM_CBK_DEBUG_PRINT ("role_info_p is NULL");
    return OF_FAILURE;
  }

  if ((of_strcmp (user_name_p, "root") == 0) || (of_strcmp (user_name_p, "admin") == 0))
  {
    if ((of_strcmp (password_p, "root") == 0) || (of_strcmp (password_p, "admin") == 0))
    {
      of_strcpy (role_info_p->admin_role, "AdminRole");
      of_strcpy (role_info_p->admin_name,user_name_p);
      return OF_SUCCESS;
    }
    CM_CBK_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "UCM") == 0)
  {
    if (of_strcmp (password_p, "UCM") == 0)
    {
      of_strcpy (role_info_p->admin_role, "UCMRole");
      of_strcpy (role_info_p->admin_name,user_name_p);
      return OF_SUCCESS;
    }
    CM_CBK_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }
  else if (of_strcmp (user_name_p, "guest") == 0)
  {
    if (of_strcmp (password_p, "guest") == 0)
    {
      of_strcpy (role_info_p->admin_role, "GuestRole");
      of_strcpy (role_info_p->admin_name,user_name_p);
      return OF_SUCCESS;
    }
    CM_CBK_DEBUG_PRINT ("Invalid Password");
    return OF_FAILURE;
  }

  /*GetExact User from the data base*/
 
/* 
  ulSnetId = CM_SNET_GENERAL; //UCM has to pass SNet Name for authentication
  return_value =  GetExactUsrDbRec(ulSnetId, user_name_p, &UserInfo);

  if (return_value == OF_SUCCESS)
  {
    //Verify the username and password using  UserEncryAuthenticate
  
    return_value = UserEncryAuthenticate(ulSnetId, user_name_p, aChallenge, password_p, &Privileges, &InactivityTimer);
  
    if(return_value == OF_SUCCESS)
    {
      of_strcpy (role_info_p->admin_role,UserInfo.aUcmRoleName);
      of_strcpy (role_info_p->admin_name,user_name_p);
      CM_CBK_DEBUG_PRINT("username=%s password=%s admin role=%s",
                            user_name_p,password_p,role_info_p->admin_role);
      return OF_SUCCESS;
    }
    else
    {
      CM_CBK_DEBUG_PRINT ("Invalid Password");
      return OF_FAILURE;
    }
  }
*/
  return OF_SUCCESS;
}

#endif /* CM_AUTH_SUPPORT */
#endif /* OF_CM_SUPPORT */
