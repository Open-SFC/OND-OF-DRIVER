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
 * File name:   ldsvldef.h
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: This header file contains LDSV engine specific 
 *              local structures and definitions.
/*                                                                        */

#ifndef CM_LDSV_LDEF_H
#define CM_LDSV_LDEF_H

#define  CM_LDSV_TRACE_ID 1
#define CM_LDSV_TRACE_SEVERE  '2'
//#define CM_LDSV_DEBUG
#ifdef CM_LDSV_DEBUG
//#define CM_LDSV_DEBUG_PRINT printf("\n%35s(%4d)::", __FUNCTION__, __LINE__),printf
#define CM_LDSVWRAP_DEBUG_PRINT printf("\n%35s(%4d)::", __FUNCTION__, __LINE__),printf
//#define CM_LDSV_DEBUG_PRINT(format,args...) printf("\n%35s(%4d): "format,__FUNCTION__,__LINE__,##args);
#define CM_LDSV_DEBUG_PRINT(format,args...)\
   cm_ldsv_trace( CM_LDSV_TRACE_ID, CM_LDSV_TRACE_SEVERE, (char *)__FUNCTION__, __LINE__,format,##args)
#else
#define CM_LDSV_DEBUG_PRINT(format,args...)
#define CM_LDSVWRAP_DEBUG_PRINT(format,args...)
#endif

#define  CM_LDSV_ENG_ADMIN_ROLE   "AdminRole"
#define CM_LDSV_FDPOLL_READ 0x01
#define CM_LDSV_FDPOLL_WRITE 0x02
#define CM_LDSV_FDPOLL_EXCEPTION 0x04
#define CM_LDSV_FDPOLL_URGENT 0x08

#define CM_LDSV_FDPOLL_MAXFDS 4
#define CM_LDSV_FDPOLL_LEAST_POLL_TIME ((3) * 1000)

#define LDSV_MAX_BUFFER_SIZE (2 * 1024)
#define LDSV_BUFFER_SIZE (1024)
#define LDSV_DMPATH_SIZE 512
#define LDSV_COPY_FILE_BUFFER_SIZE 512 
#define LDSV_XML_READ_LINE 512 
#define LDSV_XML_WRITE_BUFFER 25 

#define unlikely(x) __builtin_expect((x), 0)
//#define PERSISTENT_TRANSFERLIST_DIR "/ucmconfig/"
//#define PERSISTENT_TRANSFERLIST_DEFAULT_DIR "/VortiQa/ucm/config-default/"
#define MAX_HASH_SIZE 500
#define CM_LDSV_DM_PATH_PARSING_COMPLETED  0
#define CM_LDSV_DM_PATH_PARSING_REMAINING  1
#define DIRMODE (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)

#define CM_LDSV_CREATE_COMPLETE_DIR_PATH(aFullPath,aDirPath, pVersionNo) \
   memset(aFullPath,0,sizeof(aFullPath));\
   strncat(aFullPath, PERSISTENT_TRANSFERLIST_DIR, strlen(PERSISTENT_TRANSFERLIST_DIR));\
   strncat(aFullPath, pVersionNo, strlen(pVersionNo));\
   strncat(aFullPath, "/", 1);\
   strncat(aFullPath, aDirPath, strlen(aDirPath));\

#define CM_LDSV_CREATE_COMPLETE_FILE_PATH(aFullPath, aDirPath, aFile, iVersionNo) \
   memset(aFullPath,0,sizeof(aFullPath));\
   strncat(aFullPath, PERSISTENT_TRANSFERLIST_DIR, strlen(PERSISTENT_TRANSFERLIST_DIR));\
   strncat(aFullPath, iVersionNo, strlen(iVersionNo));\
   strncat(aFullPath, "/", 1);\
   strncat(aFullPath, aDirPath, strlen(aDirPath));\
   strncat(aFullPath, aFile, strlen(aFile));

#define CM_LDSV_CREATE_COMPLETE_DEFAULT_FILE_PATH(aFullPath,aDirPath, aFile) \
   memset(aFullPath,0,sizeof(aFullPath));\
   strncat(aFullPath, PERSISTENT_TRANSFERLIST_DEFAULT_DIR, strlen(PERSISTENT_TRANSFERLIST_DEFAULT_DIR));\
   strncat(aFullPath, aDirPath, strlen(aDirPath));\
   strncat(aFullPath, aFile, strlen(aFile));

typedef struct cm_ldsv_table_stack_node_s
{
  /* Current node path */
  char *dm_path_p;
  char *pElementName;
  struct cm_dm_node_info *pDMNodeAttrib;
  /* Info useful to get next dm nodes */
  void *pOpaqueInfo;
  uint32_t uiOpaqueInfoLen;

  //struct cm_nv_pair ActiveInstKey;
  struct cm_array_of_nv_pair ActiveInstKeys;
  /* Stack maintenance */
  UCMDllQNode_t ListNode;
} cm_ldsv_table_stack_node_t;


typedef struct cm_ldsv_config_request_s
{
  uint32_t command_id;           /*(LDSV_COMMAND) */
  uint32_t sub_command_id;            /* load/save/factorydefault */
  char *dm_path_p;            /*Data Model Path string */
  int32_t iDMPathLength;        /*(Data Model path string length) */
} cm_ldsv_config_request_t;


#endif /* CM_LDSV_LDEF_H */
