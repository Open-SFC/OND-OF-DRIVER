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
 * File name: mesgreg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: 
*/

#ifdef CM_MESGCLNT_SUPPORT
#include "cmincld.h"
#include <dirent.h>

#define CM_MSGCLNT_LIB_PATH "/ucm/lib/mesgclnt"

int32_t UCMMsgSysLogModulesInit(void)
{
#if 0
	 printf("%s :: Entry\n\r", __FUNCTION__);
UCMMsgClntReg_Init();
	 printf("%s :: Exit\n\r", __FUNCTION__);
#endif
   int32_t lRetVal=OF_FAILURE;
   DIR *pDir =NULL;
   struct dirent *dp;
   char  path[64]={'\0'};

   of_strcpy(path,CM_MSGCLNT_LIB_PATH);
  
   pDir = opendir(path);
   if(!pDir)
   {
     printf("%s():diropen for %s failed\n\r",__FUNCTION__,path);
     return OF_FAILURE;
   }

   while((dp = readdir(pDir))!= NULL)
   { 
     printf("%s: filename=%s\n",__FUNCTION__,dp->d_name);
     if(!(strcmp(dp->d_name,"."))||!(strcmp(dp->d_name,"..")))
     {
       continue;
     }

     sprintf (path,
             "%s/%s",CM_MSGCLNT_LIB_PATH,dp->d_name);

     printf("%s(): path:%s\n\r",__FUNCTION__,path);
   
     if((lRetVal =  cm_dl_open(path,"UCMMsgClntReg_Init"))!=OF_SUCCESS)
     {
       printf("%s:failed \n",__FUNCTION__);
       closedir(pDir);
       exit(1);
     }  
   }
   closedir(pDir);
   return OF_SUCCESS;
}
#endif

