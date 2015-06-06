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
 * File name: authgdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains definition for  
 *              all possible command types for user management.
 * 
 */

#ifndef UCMAUTH_GDEF_H
#define UCMAUTH_GDEF_H

#define CM_AUTH_USR_ADD_CMD  0x1
#define CM_AUTH_USR_MOD_CMD  0x2
#define CM_AUTH_USR_DEL_CMD  0x3
#define CM_AUTH_USR_AUTH_CMD 0x4

struct cm_auth_mgmt
{
  char* pUsrName;
  char* pPwd;
  char* pNewPwd;
  struct cm_role_info* role_info_p;
};


#endif /* UCMAUTH_GDEF_H */
