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
 * File name: jever.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/05/2013
 * Description: This file contains internal structure declarations 
 *              of config middleware data model
*/

#ifndef JEVER_H
#define JEVER_H

/*****************************************************************************
 * * * * * * * * * * *   M A C R O  D E F I N I T I O N S * * * * * * * * * * *
 *****************************************************************************/
#define   likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#define CM_MAX_CONSOLIDATE_VERSION 100
#define CM_ALERT_MAX_CONSOLIDATE_VERSION (9 *CM_MAX_CONSOLIDATE_VERSION/ 10)
#define CM_MAX_VERSION 100000
#define CM_BASE_VERSION 0

/****************************************************************************
 * * * * * * * * S T R U C T U R E   D E F I N I T I O N S* * * * * * * * * * 
 *****************************************************************************/
/*Each command represents in name value pairs*/
struct cm_vh_command
{
   UCMDllQNode_t list_node;       /*Double linked list node, that
                                   holds previous and next commands */
   //uint32_t uiDMID;              /* Id of the Data model */
   uint32_t command_id;         /* Command operation */
   char *dm_path_p;             /* name of the Data Model */
   struct cm_array_of_nv_pair nv_pair_array;
};

/* version data structure */
struct cm_vh_version
{
   UCMDllQNode_t list_node;
   uint64_t version;     /* major:8, minor:8,revision:16  */
   UCMDllQ_t CommandList;    /* Double linked list head */
};

/*****************************************************************************
 * * * * * * * * * * * * G L O B A L   A P I 's * * * * * * * * * * * * * * *
 *****************************************************************************/

void cm_vh_config_verion_init (void);

int32_t cm_vh_create_new_config_version (uint64_t * pversion);

void cm_vh_consolidate_config_versions (void);

int32_t cm_vh_add_command_to_config_version (uint64_t uiversion,
      struct cm_command * command_p);

int32_t cm_vh_get_delta_version_history_t (uint64_t from_version_ui, uint64_t end_version,
      uint32_t *ver_cnt_ui_p, struct cm_version ** version_list_pp);

int32_t cm_vh_print_config_version_history_t (void );

int32_t cm_vh_get_versions(struct cm_array_of_nv_pair **ppnv_pair_array);
/*****************************************************************************
 * * * * * * * * * * * * L O C A L    A P I 's * * * * * * * * * * * * * * *
 *****************************************************************************/

unsigned char vh_is_version_available(uint64_t version);

int32_t vh_copy_version (struct cm_vh_version * pSrcCfgversion,
      struct cm_version *pDestCfgversion);

int32_t vh_copy_command (struct cm_vh_command * pSrcCommand,
      struct cm_vh_command * pDestCommand);

void cm_vh_free_command_list (struct cm_vh_version * vh_version_p);

void cm_vh_free_command (struct cm_vh_command * command_p);
#endif /* VERSION_LIF_H */
