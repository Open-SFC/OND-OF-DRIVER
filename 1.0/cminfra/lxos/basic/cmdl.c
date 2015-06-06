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
 * File name: ucmdl.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: 
*/

#include <dlfcn.h>
#include <dirent.h>
#include "cmincld.h"

#define OF_MAX_FNAME_LENGTH (32)
#define OF_MAX_PATH_LENGTH (64) 

int32_t cm_dl_open(char *ppath, char *fname);
void* cm_dl_open_get_fn_ptr(char *ppath, char *fname);

int32_t cm_dl_open(char *ppath, char *fname)
{
  char *handle=NULL, *error=NULL;
  char tmp_path[64]={ };
  int32_t (* pinitfun)(void);

  if((!fname )||(!ppath ))
  {
    printf("%s(): Invalid Input\n\r",__FUNCTION__);
    return OF_FAILURE;
  }

  if((of_strlen(fname) >= OF_MAX_FNAME_LENGTH)||
     (of_strlen(ppath) >= OF_MAX_PATH_LENGTH) )
  {
    printf("%s(): Input exceeds the max limit\n\r",__FUNCTION__);
    return OF_FAILURE;
  }
  
  of_strcpy(tmp_path,ppath);

  handle = dlopen(ppath,RTLD_LAZY);
  if(!handle)
  {
    printf("\n Handle is null ,path: %s \n",ppath);
    if ((error = dlerror()) != NULL)
    {
      fprintf (stderr, "Try again error :%s:%s\n", fname,error);
    }
    dlerror();
    handle = dlopen(tmp_path,RTLD_LAZY);
    if(!handle)
    {
      if ((error = dlerror()) != NULL)
      {
         fprintf (stderr, "%s:%s\n", fname,error);
         printf("dlopen failed for path:%s\n",tmp_path);
         exit(1);
      }
      printf("dlopen failed for path:%s\n",ppath);
      exit(1);
    }
  }
  
  if(of_strcmp(ppath,tmp_path) != 0)
  {
    printf("\n handle: %p path: %s \n",handle,ppath);
    printf("\n  Path is corrupted :%s \n",ppath); 
    dlclose(handle);
    return OF_FAILURE;
  }

  dlerror(); 
  pinitfun = dlsym(handle, fname);
  if ((error = dlerror()) != NULL)
  {
    fprintf (stderr, "%s:%s\n", fname,error);
    printf("dlsym failed for path:%s\n",ppath);
    dlclose(handle);
    dlerror();

    handle = dlopen(tmp_path,RTLD_LAZY);
    if(!handle)
    {
      if ((error = dlerror()) != NULL)
      {
         fprintf (stderr, "%s:%s\n", fname,error);
      }
       printf("dlopen failed for path:%s\n",tmp_path);
       exit(1);
    } 
   
    dlerror(); 
    pinitfun = dlsym(handle,fname);
    if((error = dlerror()) != NULL)
    {
      fprintf (stderr, "%s:%s\n", fname,error);
      printf("dlsym failed for path:%s\n",tmp_path);
      dlclose(handle);
      exit(1);
    } 

  }
  pinitfun();
  return OF_SUCCESS;
}

void* cm_dl_open_get_fn_ptr(char *ppath, char *fname)
{
  char *handle=NULL, *error=NULL;
  void *pFn;
  if((!fname )||(!ppath ))
  {
    printf("%s(): Invalid Input\n\r",__FUNCTION__);
    return NULL;
  }

  if((of_strlen(fname) >= OF_MAX_FNAME_LENGTH)||
     (of_strlen(ppath) >= OF_MAX_PATH_LENGTH) )
  {
    printf("%s(): Input exceeds the max limit\n\r",__FUNCTION__);
    return NULL;
  }

  handle = dlopen(ppath,RTLD_LAZY);
  if(!handle)
  {
    if ((error = dlerror()) != NULL)
    {
      fprintf (stderr, "%s:%s\n", fname,error);
      printf("dlopen failed for path:%s\n",ppath);
      return NULL;
    }
    printf("dlopen failed for path:%s\n",ppath);
    return NULL;
  }

  pFn = dlsym(handle, fname);
  if ((error = dlerror()) != NULL)
  {
    fprintf (stderr, "%s:%s\n", fname,error);
    printf("dlsym failed for path:%s\n",ppath);
    dlclose(handle);
    return NULL;
  }
  return pFn;
}


