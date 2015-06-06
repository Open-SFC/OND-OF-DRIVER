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
 * File name: ucmdb.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: 
 * 
 */

#ifndef _CM_PAM_ROLEDB_DEFS_H_
#define _CM_PAM_ROLEDB_DEFS_H_

struct cm_role_db {
  char * user_name_p;
  char * pPwdSum;
  char * role_name_p;
  struct UCMRoleDb_s *next_p;
};

int cm_load_user_db(char *file_name_p);
char * get_role_info(const char *pUser, char *pMd5Sum);
void clean_role_info();

#ifdef CM_ROLEDB_DEBUG
#define UCMROLEDBDEBUG printf
#else
#define UCMROLEDBDEBUG(format,args...)
#endif

#endif /*_CM_PAM_ROLEDB_DEFS_H_*/
