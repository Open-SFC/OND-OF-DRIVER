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
 * File name: usrmgmt.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: UCM PAM User Management.
 * 
 */

#ifdef CM_SUPPORT
#ifdef CM_AUTH_SUPPORT
#ifdef OF_CM_AUTH_PLUGIN_SUPPORT

#include "authinc.h"

UCMManageUser pRegFunPtr;

int32_t cm_register_auth_function(UCMManageUser pAuthFunPtr)
{
  pRegFunPtr = pAuthFunPtr;
  return OF_SUCCESS;
}

int32_t cm_user_mgmt(uint32_t uiCmd, struct cm_auth_mgmt *pAuth)
{
  int32_t return_value = OF_FAILURE;
  if(pRegFunPtr != NULL)
  {
    return_value = (pRegFunPtr)(uiCmd, pAuth);
  }
  return return_value;
}

#endif /* OF_CM_AUTH_PLUGIN_SUPPORT */
#endif /* CM_AUTH_SUPPORT */
#endif /* CM_SUPPORT */
