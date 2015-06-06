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
 * File name: dmldef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/17/2013
 * Description: This file contains internal structure declarations 
 *              of config middle-ware data model.
*/

#ifndef DM_LDEF_H
#define DM_LDEF_H

#define  CM_DM_TRACE_ID 1
#define CM_DM_TRACE_SEVERE  '2'

#ifdef OF_CM_SWITCH_CBK_SUPPORT 
#define  CM_DM_LOG_FILE "/psp/debug/dmdebug"
#endif

#ifdef OF_CM_CNTRL_SUPPORT
#define  CM_DM_LOG_FILE "/usr/local/ond_of_driver/1.0/debug/dmdebug"
#endif

#ifdef CM_DMTRANS_DEBUG
#define CM_DMTRANS_DEBUG_PRINT printf("\n%35s(%4d): ",__FUNCTION__,__LINE__),\
                    printf
#else
#define CM_DMTRANS_DEBUG_PRINT(format,args...)
#endif

#ifdef CM_DM_DEBUG
#define CM_DM_DEBUG_PRINT(format,args...) \
{\
	    cmi_dm_trace( CM_DM_TRACE_ID, CM_DM_TRACE_SEVERE,(char *)__FUNCTION__, __LINE__,format,##args);\
}
#else
#define CM_DM_DEBUG_PRINT(formatstring,args...)
#endif

/*****************************************************************************

/* Parsing utility related marcos */
#define UCMDM_PATH_PARSING_COMPLETED  0
#define UCMDM_PATH_PARSING_REMAINING  1

/* Copy utility macros */
#define CM_COPY_FROM_EXT_STRUCT 0
#define CM_COPY_TO_EXT_STRUCT   1

struct UCMDM_Inode_s;

typedef struct UCMDM_Instance_s
{
  struct cm_dm_instance_map Map;      /* Map Entry */
  UCMDllQ_t ChildInodeList;     /* Child list of UCMDM_Inode_t */
  UCMDllQNode_t list_node;       /* Next Instance */
  UCMDllQ_t role_perm_list;      /* Roles and permission list */
  struct UCMDM_Inode_s *pInode; /* Back pointer to Inode */

  UCMDllQNode_t IDHashlist_node; /* ID Hash list entry */
  UCMDllQNode_t KeyHashlist_node;        /* Key Hash list entry */
} UCMDM_Instance_t;

#define UCMDM_INODE_PARENT_TYPE_INODE     0
#define UCMDM_INODE_PARENT_TYPE_INSTANCE  1

typedef struct UCMDM_Inode_s
{
  /* Some nodes are tables. The children tree of this node
     should be accessed through the instance of the table.
     For each instance of the table, there exists one child tree.

     Some nodes are not tables. Rather they are just anchors.
     Their children can be directly accessed from node itself.
   */
  union
  {
    UCMDllQ_t InstanceList;     /* linked list of Instance_t */
    UCMDllQ_t ChildInodeList;   /* Child list of ucmDM_Inode_t */
  } ChildAccess;

  /* Reference : Point to Data model node */
  struct cm_dm_data_element *pDMNode;

  /* Back pointer to parent */
  uint32_t uiParentType;
  union
  {
    struct UCMDM_Inode_s *parent_inode_p; /* Pointer to Parent Inode */
    UCMDM_Instance_t *parent_instance_p;  /* Pointer to parent Instance */
  } ParentAccess;

  /* House keeping information */
  uint32_t uiNextFreeID;
  unsigned char bIDWrapped;

  UCMDllQNode_t list_node;       /* Holds Child Inode_t list */
} UCMDM_Inode_t;

/* Global hash table */
#define UCMDM_IDMAP_HASH_TABLE_MAX_SIZE 1024
typedef struct UCMDM_InstanceMapHashTables_s
{
  /* ID Hash table, useful for quick getexact */
  UCMDllQ_t IDHashTable[UCMDM_IDMAP_HASH_TABLE_MAX_SIZE];

  /* Key Hash table, useful for quick getexact */
  UCMDllQ_t KeyHashTable[UCMDM_IDMAP_HASH_TABLE_MAX_SIZE];
} UCMDM_InstanceMapHashTables_t;

typedef struct ucmdm_RolePermission_s
{
  char role[UCMDM_ADMIN_ROLE_MAX_LEN + 1];
  uint32_t permissions;
  UCMDllQNode_t list_node;
} ucmdm_RolePermission_t;
#endif /* DM_LDEF_H */
