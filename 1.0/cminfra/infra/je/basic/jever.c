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
 * File name: jever.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/25/2013
 * Description: 
*/

#ifdef CM_SUPPORT
#ifdef CM_VERSION_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jever.h"


/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
UCMDllQ_t cm_vh_version_list_g; /* Holds the config versions list */
extern uint64_t run_time_version_g;
extern uint64_t save_version_g;
uint64_t cnsltd_version_g;

/******************************************************************************
 * * * * * * * * * * * * * * FUNCTION DEFINITION * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**************************************************************************
 *  Function Name :cm_vh_config_verion_init
 *  Input Args    : none
 *  Output Args   : none
 *  Description   : This method is used to Initialize the global version list
 *  Return Values : Successful version list initialization returns OF_SUCCESS,
 *          othewise returns OF_FAILURE.
 ***************************************************************************/

void cm_vh_config_verion_init (void)
{
   /* - Initialize the global version list */
   CM_DLLQ_INIT_LIST (&cm_vh_version_list_g);
   run_time_version_g=CM_BASE_VERSION;
   save_version_g=CM_BASE_VERSION;
   cnsltd_version_g=CM_BASE_VERSION;
   return;
}

/**************************************************************************
 *  Function Name :cm_vh_create_new_config_version
 *  Input Args    :None
 *  Output Args   :pversion : newly created version
 *  Description   :This method is used to create a new version for every
 *         successful configuration session end. This method is
 *         invoked by JE daemon.
 *  Return Values :Successful creation of new version returns OF_SUCCESS,
 *         if any failure occured, returns OF_FAILURE
 **************************************************************************/

int32_t cm_vh_create_new_config_version (uint64_t * pversion)
{
   uint64_t version;
   UCMDllQ_t *vh_version_list = &cm_vh_version_list_g;
   struct cm_vh_version *vh_version_p = NULL;
   uint64_t version_new = 0;

   /* Generate a new version number and fill it */
   version= run_time_version_g;

   /* Generate a new version number and fill it */
   if (version < CM_BASE_VERSION)
      version_new = CM_BASE_VERSION;
   else
      version_new = (version + 1);

   /*
    * See whether we have reached max version numbers.
    * If so, Consolidate them by calling cm_vh_consolidate_config_versions()
    */
   if ((version - cnsltd_version_g) >= (CM_MAX_CONSOLIDATE_VERSION/2)) 
   {
      cm_vh_consolidate_config_versions ();
			cnsltd_version_g=version;
   }

   vh_version_p = (struct cm_vh_version *) of_calloc (1, sizeof (struct cm_vh_version));
   if (unlikely (vh_version_p == NULL))
   {
      CM_JE_DEBUG_PRINT (" Failed to initialize vh_version_p ");
      return OF_FAILURE;
   }

   CM_DLLQ_INIT_LIST (&(vh_version_p->CommandList));
   vh_version_p->version = version_new;
   CM_JE_DEBUG_PRINT ("oldversion=%llx New version= %llx",
         version, vh_version_p->version);
   /* 
    * Prepend the data structure to Global version
    * list “UCMConfigversionHead”
    */
   CM_DLLQ_PREPEND_NODE (vh_version_list, &(vh_version_p->list_node));

   /*Fill the output argument “pversion” with the generated version number */
   *pversion = vh_version_p->version;
   run_time_version_g = vh_version_p->version;
   CM_JE_DEBUG_PRINT ("Created new version number ");
   return OF_SUCCESS;
}

/**************************************************************************
 *  Function Name :cm_vh_consolidate_config_versions
 *  Input Args    :
 *         Configversion: List of configuration versions
 *  Output Args   :none
 *  Description   :This method is used to Consolidate existing versions, if
 *         space is not available for creating a new version.
 *  Return Values :Successful consolidation of versions returns OF_SUCCESS,
 *                 if any failure occured, returns OF_FAILURE
 ***************************************************************************/

void cm_vh_consolidate_config_versions (void)
{
   uint32_t count_ui;             /*Total Count of the versions */
   struct cm_vh_version *vh_version_p = NULL;
   UCMDllQ_t *vh_version_list = &cm_vh_version_list_g;
   UCMDllQNode_t *pDllQNode;

   /* Find total no of versions present in the list */
   count_ui = CM_DLLQ_COUNT (vh_version_list);
   CM_JE_DEBUG_PRINT ("Number of versions present in history_t are %d", count_ui);
   if (count_ui == 0)
   {
      return;
   }
   if (count_ui != CM_MAX_CONSOLIDATE_VERSION)
   {
      CM_JE_DEBUG_PRINT ("Not enough versions present in history_t to consolidate");
      return;
   }

   if ((count_ui % 2) == 0 )
      count_ui = count_ui / 2;
   else
      count_ui = count_ui / 2 + 1;

   while (count_ui > 0)
   {
      /* delete older N/2 versions. */
      pDllQNode = CM_DLLQ_LAST (vh_version_list);
      vh_version_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_vh_version *, list_node);
      CM_DLLQ_DELETE_NODE (vh_version_list, pDllQNode);
      cm_vh_free_command_list (vh_version_p);
      of_free (vh_version_p);
      count_ui--;
   }
   CM_JE_DEBUG_PRINT ("Consolidation of commands done");
   return;
}

/**************************************************************************
 * Function Name :cm_vh_add_command_to_config_version
 * Input Args    :
 *                version : version number
 *                command_p: Name value pair of commands to add to the version
 * Output Args   :none
 * Description   :This method is used add commands to  exising version.
 * Return Values :Successful add of commands to version returns OF_SUCCESS,
 *                if any failure occured, returns OF_FAILURE
 ****************************************************************************/

int32_t cm_vh_add_command_to_config_version (uint64_t version,
      struct cm_command * command_p)
{
   UCMDllQ_t *vh_version_list = &cm_vh_version_list_g;
   UCMDllQNode_t *pDllQNode;
   struct cm_vh_version *vh_version_p;
   struct cm_vh_command *vh_command_p;

   CM_JE_DEBUG_PRINT ("Entered");
   if (command_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid Input arguments");
      return OF_FAILURE;
   }

   if ((version < CM_BASE_VERSION) || (version > run_time_version_g))
   {
      CM_JE_DEBUG_PRINT ("Invalid version Number");
      return OF_FAILURE;
   }

   if (!command_p->dm_path_p)
   {
      CM_JE_DEBUG_PRINT ("Invalid DM Path");
      return OF_FAILURE;
   }

   vh_command_p = (struct cm_vh_command *) of_calloc (1, sizeof (struct cm_vh_command));
   if (unlikely (vh_command_p == NULL))
   {
      CM_JE_DEBUG_PRINT ("Failed to initialize vh_command_p");
      return OF_FAILURE;
   }

   vh_command_p->command_id = command_p->command_id;
   vh_command_p->dm_path_p = command_p->dm_path_p;
   vh_command_p->nv_pair_array.count_ui = command_p->nv_pair_array.count_ui;
   vh_command_p->nv_pair_array.nv_pairs = command_p->nv_pair_array.nv_pairs;

   /* Resetting Pointers to NULL */
   command_p->dm_path_p = NULL;
   command_p->nv_pair_array.count_ui = 0;
   command_p->nv_pair_array.nv_pairs = NULL;

   /*
    * Scan through the list “cm_vh_version_list_g” and findout
    *the version data structure by matching the version.
    */

   CM_DLLQ_SCAN (vh_version_list, pDllQNode, UCMDllQNode_t *)
   {
      vh_version_p =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_vh_version *, list_node);
      if (vh_version_p->version == version)
      {
         CM_JE_DEBUG_PRINT
            ("Adding commands to version .... %llx" ,version);
         /*Append the command_p to the command list */
         CM_DLLQ_APPEND_NODE (&(vh_version_p->CommandList),
               &(vh_command_p->list_node));
         CM_JE_PRINT_COMMAND(vh_command_p);
         return OF_SUCCESS;
      }
   }

   CM_JE_DEBUG_PRINT ("version not found");
   cm_vh_free_command (vh_command_p);
   return OF_FAILURE;
}

/**************************************************************************
 *  Function Name :UCMGetConfigversionDiff
 *  Input Args    :
 *                uiFromversion : version number
 *                uiToversion : version Number
 *  Output Args   :
 *                command_list_pp: Name value pair of commands between
 *                uiFromversion and uiToversion
 *  Description   :This method is used to get the configuration differences
 *         between version1 and version2.
 *  Return Values :Successful consolidation of differences between 2 versions
 *         returns OF_SUCCESS,othewise returns OF_FAILURE.
 ***************************************************************************/

int32_t cm_vh_get_delta_version_history_t (uint64_t from_version_ui, uint64_t end_version,
      uint32_t *ver_cnt_ui_p,   struct cm_version ** version_list_pp)
{
   UCMDllQ_t *vh_version_list = &cm_vh_version_list_g;
   struct cm_version *version_list_p=NULL;
   UCMDllQNode_t *pDllQNode, *pDllQNode2;
   uint64_t ulStartversion;
   uint32_t uiversionCnt, uiCnt=0;
   struct cm_vh_version *vh_version_p;

   CM_JE_DEBUG_PRINT ("From version %llx  Toversion %llx  UCMversion %llx", from_version_ui, end_version, run_time_version_g);
 
 
	 if (from_version_ui > end_version)
	 {
      CM_JE_DEBUG_PRINT ("****From version is greater than To version... Invalid Parameters");
      return OF_FAILURE;
	 }
	 if (((from_version_ui < CM_BASE_VERSION)
            || (from_version_ui > run_time_version_g))
         || ((end_version < CM_BASE_VERSION)
            || (end_version > run_time_version_g)))
   {
      CM_JE_DEBUG_PRINT ("Invalid input Parameters");
      return OF_FAILURE;
   }

   ulStartversion = from_version_ui;
   if (vh_is_version_available (ulStartversion) == FALSE)
   {
      CM_JE_DEBUG_PRINT ("version not available");
      return OF_FAILURE;
   }
   if ((vh_is_version_available (end_version)) == FALSE)
   {
      CM_JE_DEBUG_PRINT ("version not available");
      return OF_FAILURE;
   }

   uiversionCnt= end_version-ulStartversion + 1;
   CM_JE_DEBUG_PRINT ("Number of Versons required %d",uiversionCnt);
   version_list_p = (struct cm_version *) of_calloc (uiversionCnt, sizeof (struct cm_version));
   if (!version_list_p)
   {
      CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
      return OF_FAILURE;
   }
   vh_version_p= (struct cm_vh_version *)CM_DLLQ_FIRST(vh_version_list);
   CM_JE_DEBUG_PRINT ("first version in list %llx", vh_version_p->version);
   vh_version_p= (struct cm_vh_version *)CM_DLLQ_LAST(vh_version_list);
   CM_JE_DEBUG_PRINT ("last version in list %llx", vh_version_p->version);
   CM_DLLQ_SCAN (vh_version_list, pDllQNode, UCMDllQNode_t *)
   {
      vh_version_p = CM_DLLQ_LIST_MEMBER
         (pDllQNode, struct cm_vh_version *, list_node);

      CM_JE_DEBUG_PRINT ("version in scan list %llx", vh_version_p->version);
      if (vh_version_p->version == ulStartversion)
      {
         if (vh_copy_version (vh_version_p, &version_list_p[uiCnt++]) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Copy Configuration for a version Failed");
            of_free(version_list_p);
            return OF_FAILURE;
         }
         if(uiversionCnt == 1 )
         {
            break;
         }
         while ((pDllQNode2= CM_DLLQ_PREV(vh_version_list, pDllQNode)) != NULL)
         {
            vh_version_p = CM_DLLQ_LIST_MEMBER
               (pDllQNode2, struct cm_vh_version *, list_node);
            /* Moving towards Left from Startversion
             * Latest versions are present at the beginning of List*/
            if( vh_version_p->version <= end_version)
            {
               if (vh_copy_version (vh_version_p, &version_list_p[uiCnt++]) != OF_SUCCESS)
               {
                  CM_JE_DEBUG_PRINT ("Copy Configuration for a version Failed");
                  return OF_FAILURE;
               }
               pDllQNode=pDllQNode2;
            }
            if (uiversionCnt == uiCnt)
            {
               break;
            }
         }
         /* We got all required versions*/
         break;
      }
   }

   *ver_cnt_ui_p=uiversionCnt;
   *version_list_pp = version_list_p;
   CM_JE_DEBUG_PRINT ("found configuration differences between versions %llx and %llx", ulStartversion, end_version);
   return OF_SUCCESS;
}

int32_t vh_copy_version (struct cm_vh_version * pSrcCfgversion,
      struct cm_version * pUCMversion)
{
   uint32_t count_ui=0;
   struct cm_array_of_commands *command_list_p;
   struct cm_command *commands_p;
   struct cm_vh_command *command_p;
   UCMDllQNode_t *pDllQNode;

   CM_JE_DEBUG_PRINT ("Entered");
   if ((pSrcCfgversion == NULL) || (pUCMversion == NULL))
   {
      CM_JE_DEBUG_PRINT ("Invalid inputs");
      return OF_FAILURE;
   }

   CM_JE_DEBUG_PRINT ("Copying version %llx ",  pSrcCfgversion->version);
   pUCMversion->version = pSrcCfgversion->version;
   command_list_p=(struct cm_array_of_commands *) of_calloc(1, sizeof(struct cm_array_of_commands));
   command_list_p->count_ui=CM_DLLQ_COUNT(&pSrcCfgversion->CommandList); 
   commands_p=(struct cm_command *) of_calloc(command_list_p->count_ui,sizeof(struct cm_command));

   CM_DLLQ_SCAN (&pSrcCfgversion->CommandList, pDllQNode, UCMDllQNode_t *)
   {
      command_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_vh_command *, list_node);
      commands_p[count_ui].command_id=command_p->command_id;
      commands_p[count_ui].dm_path_p=command_p->dm_path_p;
      commands_p[count_ui].nv_pair_array.count_ui = command_p->nv_pair_array.count_ui;
      commands_p[count_ui].nv_pair_array.nv_pairs = command_p->nv_pair_array.nv_pairs;
#if 0
			/* setting to NULL... to avoid double free*/
      command_p->dm_path_p=NULL;
      command_p->nv_pair_array.count_ui=0;
      command_p->nv_pair_array.nv_pairs=NULL;
#endif
      count_ui++;
   }
   command_list_p->Commands=commands_p;
   pUCMversion->command_list_p=command_list_p;
   CM_JE_PRINT_ARRAY_OF_COMMANDS(pUCMversion->command_list_p);
   CM_JE_DEBUG_PRINT("Succesfully copied all commands for the version %llx", pUCMversion->version);
   return OF_SUCCESS;
}

int32_t vh_copy_command (struct cm_vh_command * pSrcCommand,
      struct cm_vh_command * pDestCommand)
{
   uint32_t uiNvCnt = 0;

   if ((pSrcCommand == NULL) || (pDestCommand == NULL))
   {
      CM_JE_DEBUG_PRINT ("Invalid inputs");
      return OF_FAILURE;
   }

   pDestCommand->command_id = pSrcCommand->command_id;
   if (pSrcCommand->dm_path_p)
   {
      pDestCommand->dm_path_p =
         (char *) of_calloc (1, of_strlen (pSrcCommand->dm_path_p) + 1);
      if (!pDestCommand->dm_path_p)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
         return OF_FAILURE;
      }
      of_strcpy (pDestCommand->dm_path_p, pSrcCommand->dm_path_p);
      pDestCommand->nv_pair_array.count_ui = pSrcCommand->nv_pair_array.count_ui;
      pDestCommand->nv_pair_array.nv_pairs =
         (struct cm_nv_pair *) of_calloc (pDestCommand->nv_pair_array.count_ui,
               sizeof (struct cm_nv_pair));
      if (!pDestCommand->nv_pair_array.nv_pairs)
      {
         CM_JE_DEBUG_PRINT ("Memory Allocation Failed");
         of_free (pDestCommand->dm_path_p);
         return OF_FAILURE;
      }
      for (uiNvCnt = 0; uiNvCnt < pDestCommand->nv_pair_array.count_ui; uiNvCnt++)
      {
         if (je_copy_nvpair_to_nvpair (&pSrcCommand->nv_pair_array.nv_pairs[uiNvCnt],
                  &pDestCommand->
                  nv_pair_array.nv_pairs[uiNvCnt]) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Copy Nv Pair To Nv Pair Failed");
            of_free (pDestCommand->dm_path_p);
            CM_FREE_NVPAIR_ARRAY (pDestCommand->nv_pair_array, pDestCommand->nv_pair_array.count_ui);
            return OF_FAILURE;
         }
      }
   }
   return OF_SUCCESS;
}

/**************************************************************************
 *  Function Name :cm_vh_free_command_list
 *  Input Args    :
 *                 pDllHead: List of configuration commands
 *  Output Args   :none
 *  Description   :This method is used to delete exising commands in
 *         from each version.
 *  Return Values :Successful deletion of commands returns OF_SUCCESS,
 *                 if any failure occured, returns OF_FAILURE
 ****************************************************************************/
void cm_vh_free_command_list (struct cm_vh_version * vh_version_p)
{
   struct cm_vh_command *command_p;
   UCMDllQNode_t *pDllQNode, *pTmpNode;

   if (vh_version_p == NULL)
   {
      CM_JE_DEBUG_PRINT ("Invalid input arguments");
      return;
   }

   CM_JE_DEBUG_PRINT ("Deleting Verion %llx", vh_version_p->version);
   CM_DLLQ_DYN_SCAN(&(vh_version_p->CommandList), pDllQNode, pTmpNode, UCMDllQNode_t *)
   {
      command_p = CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_vh_command *, list_node);
      CM_DLLQ_DELETE_NODE(&(vh_version_p->CommandList), pDllQNode);
      cm_vh_free_command (command_p);
   }
   return;
}

void cm_vh_free_command (struct cm_vh_command * command_p)
{
   if (!command_p)
   {
      CM_JE_DEBUG_PRINT ("Input is NULL");
      return;
   }
   if (command_p->dm_path_p)
      of_free (command_p->dm_path_p);

   CM_FREE_NVPAIR_ARRAY (command_p->nv_pair_array, command_p->nv_pair_array.count_ui);
   of_free (command_p);
   return;
}

unsigned char vh_is_version_available(uint64_t version)
{
   UCMDllQ_t *vh_version_list = &cm_vh_version_list_g;
   UCMDllQNode_t *pDllQNode;
   struct cm_vh_version *vh_version_p;

   CM_DLLQ_SCAN (vh_version_list, pDllQNode, UCMDllQNode_t *)
   {
      vh_version_p =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_vh_version *, list_node);
      if(vh_version_p->version == version)
      {
         return TRUE;
      }
   }
   return FALSE;
}

int32_t cm_vh_print_config_version_history_t (  )
{
#ifdef CM_JE_STATS_SUPPORT
   UCMDllQ_t *vh_version_list = &cm_vh_version_list_g;
   UCMDllQNode_t *pDllQNode, *pDllQNode2;
   struct cm_vh_version *vh_version_p;
   struct cm_vh_command *command_p;
   struct cm_nv_pair KeyNvPair;

   CM_JE_DEBUG_PRINT ("Printing Configuration history_t");
   CM_DLLQ_SCAN (vh_version_list, pDllQNode, UCMDllQNode_t *)
   {
      vh_version_p =
         CM_DLLQ_LIST_MEMBER (pDllQNode, struct cm_vh_version *, list_node);
      CM_JE_DEBUG_PRINT("version =%llx", vh_version_p->version);
      CM_DLLQ_SCAN (&vh_version_p->CommandList, pDllQNode2, UCMDllQNode_t *)
      {
         command_p = CM_DLLQ_LIST_MEMBER (pDllQNode2, struct cm_vh_command *, list_node);
         if(je_get_key_nvpair(command_p->dm_path_p,
                  &command_p->nv_pair_array,&KeyNvPair) != OF_SUCCESS)
         {
            CM_JE_DEBUG_PRINT ("Get KeyNvPair Failed");
            return OF_FAILURE;
         }
         CM_JE_DEBUG_PRINT("DMPath=%s Command=%d Key=%s",
               command_p->dm_path_p, command_p->command_id,  (char *)KeyNvPair.value_p);
         je_free_nvpair(KeyNvPair);
      }
   }
#endif
   return OF_SUCCESS;
}
#define CM_JE_RUNTIME_CONFIG_VERSION_NAME "Runtime Configuration Version"
#define CM_JE_SAVE_CONFIG_VERSION_NAME "Saved Configuration Version"


int32_t cm_vh_get_versions(struct cm_array_of_nv_pair **ppnv_pair_array)
{
	 struct cm_array_of_nv_pair *pnv_pair_array;
	 char aversion[32];

	 CM_JE_DEBUG_PRINT("Entered");

	 pnv_pair_array=(struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
	 if(pnv_pair_array==NULL)
	 {
			CM_JE_DEBUG_PRINT("Memory allocation failed");
			return OF_FAILURE;
	 }
	 pnv_pair_array->count_ui=2;
	 pnv_pair_array->nv_pairs=(struct cm_nv_pair *)of_calloc(pnv_pair_array->count_ui,sizeof(struct cm_nv_pair));

	 /* Run Time Config version*/
	 pnv_pair_array->nv_pairs[0].name_length=of_strlen(CM_JE_RUNTIME_CONFIG_VERSION_NAME);
	 pnv_pair_array->nv_pairs[0].name_p=(char *)of_calloc(1,pnv_pair_array->nv_pairs[0].name_length+1);
	 of_strncpy(pnv_pair_array->nv_pairs[0].name_p,CM_JE_RUNTIME_CONFIG_VERSION_NAME,pnv_pair_array->nv_pairs[0].name_length);
	 pnv_pair_array->nv_pairs[0].value_type=CM_DATA_TYPE_STRING;
	 CM_READ_VERSION(run_time_version_g,aversion);
	 pnv_pair_array->nv_pairs[0].value_length=of_strlen(aversion);
	 pnv_pair_array->nv_pairs[0].value_p=(char *)of_calloc(1,pnv_pair_array->nv_pairs[0].value_length+1);
	 of_strncpy(pnv_pair_array->nv_pairs[0].value_p,aversion,pnv_pair_array->nv_pairs[0].value_length);

	 /* Save Config version*/
	 pnv_pair_array->nv_pairs[1].name_length=of_strlen(CM_JE_SAVE_CONFIG_VERSION_NAME);
	 pnv_pair_array->nv_pairs[1].name_p=(char *)of_calloc(1,pnv_pair_array->nv_pairs[1].name_length+1);
	 of_strncpy(pnv_pair_array->nv_pairs[1].name_p,CM_JE_SAVE_CONFIG_VERSION_NAME,pnv_pair_array->nv_pairs[1].name_length);
	 pnv_pair_array->nv_pairs[1].value_type=CM_DATA_TYPE_STRING;
	 CM_READ_VERSION(save_version_g,aversion);
	 pnv_pair_array->nv_pairs[1].value_length=of_strlen(aversion);
	 pnv_pair_array->nv_pairs[1].value_p=(char *)of_calloc(1,pnv_pair_array->nv_pairs[1].value_length+1);
	 of_strncpy(pnv_pair_array->nv_pairs[1].value_p,aversion,pnv_pair_array->nv_pairs[1].value_length);

	 CM_JE_PRINT_NVPAIR_ARRAY(pnv_pair_array);
	 *ppnv_pair_array=pnv_pair_array;
	 return OF_SUCCESS;
}

#endif /* CM_VERSION_SUPPORT */
#endif /* CM_SUPPORT */
