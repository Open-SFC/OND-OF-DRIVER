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
 * File name: authgif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains API declarations provided by
 *              UCM AUTH module.
 * 
 */

#ifndef CM_AUTH_GIF_H
#define CM_AUTH_GIF_H

/* API to Authenticate user */

typedef int32_t (*UCMManageUser) (uint32_t uiCmd, struct cm_auth_mgmt *pAuth);
int32_t cm_register_auth_function(UCMManageUser pAuthFunPtr);
int32_t cm_user_mgmt(uint32_t uiCmd, struct cm_auth_mgmt *pAuth);

int32_t cm_pam_authenticate_user(char* pUsrName, char* pPwd, 
                               struct cm_role_info** role_info_pp);


#endif /* CM_AUTH_GIF_H */
