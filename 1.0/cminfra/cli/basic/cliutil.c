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
 * File name:  cliutil.c
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file contains utility APIs which are commonly used in
 *              CLI commands.
*/

#ifdef CM_CLIENG_SUPPORT
#include "cliincld.h"
#include "authgdef.h"
#include "authgif.h"

/****************************************************************************
 * Global Variable Declarations :
 ****************************************************************************/
extern struct cm_cli_session *cli_session_p;
extern struct cm_cli_cmd cm_cli_default_cmd_list[];
char buff_a[512];
#define CM_CLI_MAX_DATA_TYPE 8
#define CM_CLI_MAX_CMD_STRING 32 

char aTypeStr[CM_CLI_MAX_DATA_TYPE][CM_CLI_MAX_NAME_LEN + 1] = {
  "-",
  "String",
  "Integer",
  "Unsigned Integer",
  "Boolean",
  "DateTime",
  "Base64",
  "IPv4 Address"
};

/****************************************************************************
 * Local Prototype Declarations :
 ****************************************************************************/
 int32_t cm_cli_get_dir_path (char * dir_path_p);
 void cm_cli_append_data_element_nodes (struct cm_cli_node * current_node_p,
                                            struct cm_array_of_structs *
                                            data_element_nodes_p, unsigned char add_b);
 int32_t cm_cli_validate_cmd_and_fill_params (int32_t cmd_id_i,
                                                   int32_t count_i,
                                                   char (* tokens_a)[512],
                                                   struct cm_cli_params * params_p,
                                                   uint32_t * param_cnt_ui_p);
 int32_t cm_cli_fill_params( int32_t tkn_cnt_i,
                                     char (*tokens_a)[512],
                                     struct cm_cli_params * params_p,
                                     uint32_t * param_cnt_ui_p);

 void cm_cli_free_array_of_node_info (struct cm_array_of_structs
                                             * array_of_node_info_p);

 void cm_cli_free_dm_node_info (struct cm_dm_node_info * node_info_p,
                                        unsigned char free_b);
 int32_t cm_cli_verify_instance (char * token_p);
 int32_t cm_cli_frame_command_name_list (uint32_t count_ui,
                                struct cm_array_of_structs * node_info_struct_array_p,unsigned char add_b, unsigned char vsg_add_b);

 int32_t cm_cli_get_command_string(uint32_t cmd_id_ui, char *str_p);

 int32_t cm_cli_verify_int_format (const char * intaddr, int32_t * int_val);

int16_t cli_fgets (struct cm_cli_session * cli_session_pionCnxt, char * buff, uint32_t lsize_ui);

int32_t validate_string_with_out_xml_chars(char* name_p, int32_t len_i);
/******************************************************************************
 * Function Name : cm_cli_verify_instance 
 * Description   : This API will verify instances 
 * Input params  : token_p - key token
 * Output params : none
 * Return value  : OF_FAILURE/OF_SUCCESS
 *****************************************************************************/
int32_t cm_cli_verify_instance (char * token_p)
{
   char key_a[CM_CLI_MAX_NAME_LEN + 1] = { 0 }, *key_p;
   char dmpath_a[CM_CLI_MAX_PROMPT_LEN + 1] = { 0 };
   struct cm_array_of_nv_pair *key_nv_pairs_p=NULL;
   int32_t return_value1, return_value;
#ifdef CM_DM_INSTANCE_TREE_DEPENDENCY_ENABLED
   unsigned char instance_exists_b = FALSE;
#else
   struct cm_array_of_nv_pair *out_array_of_nv_pair_p = NULL;
#endif

   of_strcpy (key_a, token_p);
   key_p = of_strchr (key_a, '{');

   if (key_p)
   {
      cm_cli_get_dir_path (dmpath_a);
      cm_cli_convert_dir_to_dmpath (dmpath_a, dmpath_a);
      of_strcat (dmpath_a, ".");
      of_strncat (dmpath_a, key_a, (key_p - key_a));
      key_p++;

      return_value1=cm_cli_prepare_keys_array_from_token(dmpath_a, key_p, &key_nv_pairs_p);

      if (return_value1==OF_SUCCESS)
      {
#ifdef CM_DM_INSTANCE_TREE_DEPENDENCY_ENABLED
         instance_exists_b = cm_dm_is_instance_map_entry_using_key_array_and_name_path (dmpath_a,
               cli_session_p->tnsprt_channel_p,
               CM_CLI_MGMT_ENGINE_ID,
               &cli_session_p->role_info,
               key_nv_pairs_p);
         if (instance_exists_b == FALSE)
         {
#else
            return_value = cm_get_exact_record (cli_session_p->tnsprt_channel_p,
                  CM_CLI_MGMT_ENGINE_ID,
                  cli_session_p->role_info.admin_role,
                  dmpath_a,
                  key_nv_pairs_p, &out_array_of_nv_pair_p);
            if (return_value == OF_FAILURE)
            {
               if(out_array_of_nv_pair_p)
                  CM_FREE_PTR_NVPAIR_ARRAY (out_array_of_nv_pair_p,
                        out_array_of_nv_pair_p->count_ui);
#endif
               if(key_nv_pairs_p)
                  CM_FREE_PTR_NVPAIR_ARRAY (key_nv_pairs_p, key_nv_pairs_p->count_ui);
               sprintf (buff_a,
                     "Instance (%s) doesn't exists for the DMPath(%s)\n\r", key_p,
                     dmpath_a);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            if(out_array_of_nv_pair_p)
               CM_FREE_PTR_NVPAIR_ARRAY (out_array_of_nv_pair_p,
                     out_array_of_nv_pair_p->count_ui);
            CM_FREE_PTR_NVPAIR_ARRAY (key_nv_pairs_p, key_nv_pairs_p->count_ui);
         }
         else
         {
            sprintf (buff_a, "Node doesn't exists for DMPath(%s)\n\r", dmpath_a);
            cm_cli_puts (cli_session_p, buff_a);
            return OF_FAILURE;
         }

      }
      return OF_SUCCESS;
}

 /******************************************************************************
 * Function Name : cm_cli_get_dir_path
 * Description   : This API will return the Directory path.
 * Input params  : NONE
 * Output params : dir_path_p - Pointer to hold the Directory path information.
 * Return value  : NONE
 *****************************************************************************/
 int32_t cm_cli_get_dir_path (char * dir_path_p)
{
   int32_t i;
   char token_a[CM_CLI_MAX_NAME_LEN + 1] = { 0 }
   , *p, *q;

   of_strcpy (dir_path_p, cli_session_p->root_c);
   for (i = 1; i < cli_session_p->index_i; i++)
   {
      of_strcat (dir_path_p, "/");
      of_strcpy (token_a, cli_session_p->prompt[i]);
      p = of_strchr (token_a, '#');
      if (p != NULL)
      {
         q = token_a;
         token_a[p - q] = '{';
         of_strncat (token_a, "}", 1);
         of_strcat (dir_path_p, token_a);
         continue;
      }

      of_strcat (dir_path_p, token_a);
   }

   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_interpret_command
 * Description   : This Callback API will take each user entered line as Input 
 *                 and will interprets the input. This will read each word of
 *                 the input, validates the commands and invokes appropriate
 *                 APIs.
 * Input params  : cli_session_p - Cli session pointer
 *                 pline - User enterd command line information
 * Output params : NONE
 * Return value  : OF_SUCCESS or OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_interpret_command (/* volatile */struct cm_cli_session * cli_session_p, /*const*/ volatile char * line_p,volatile unsigned char is_tab_pressed_b)
{
   tokenizer_t *tok_p;
   uint32_t count_i =0, cursor_index_i =0, cursor_offset_i=0, i=0,return_value=0;
   uint32_t param_cnt_ui = 0,tmp2_i=0,tmp1_i=0,tmp_count_i=0;
   uint32_t cmd_cnt_i[CM_CLI_MAX_CMD_STRING] = {};
   uint32_t return_value1=0;
   int32_t  delimiter_cnt_i=0;
   const line_info *line_info_p;
   char **tokens_a = NULL;
   char **table_a = NULL;
   const char **temp_tokens_a = NULL;
   char tmp_tk_a[CM_CLI_MAX_CMD_STRING][128][512] = {} ;
   char tmp_str_a[CM_CLI_MAX_NAME_LEN + 1] = { 0 };
   unsigned char found_b = FALSE;
   struct cm_cli_params params[CM_CLI_MAX_CMDPARAMS] = { 0 };
   struct cm_cli_data_ele_node *data_ele_node_p;
   UCMDllQNode_t *dll_q_node_p;
   //char *table_name_p = NULL;
   char *value_p = NULL;
   /* volatile */struct cm_cli_session *tmp_cli_session_p = cli_session_p;
   //unsigned char can_add_table_b = FALSE;

   if((table_info_arr_p_g != NULL) && (table_info_arr_p_g->count_ui > 0))
   {
      if(table_info_arr_p_g->struct_arr_p != NULL)
         of_free(table_info_arr_p_g->struct_arr_p);
      if(table_info_arr_p_g != NULL)
         of_free(table_info_arr_p_g);
      table_info_arr_p_g = NULL;
   }
   memset(table_name_a_g, 0 , sizeof(table_name_a_g));

   /* Initialize the tokenizer_t */
   tok_p = of_tok_init (NULL);
   line_info_p = of_el_line (cli_session_p->edit_line_ctx_p);

   count_i = cursor_offset_i = cursor_index_i = 0;

   return_value = of_tok_line (tok_p, line_info_p, &count_i,&temp_tokens_a,
         &cursor_index_i, &cursor_offset_i);
   /* return 0 on success */
   if (return_value < 0)
   {
      sprintf (buff_a, "%s :: Internal error\n\r", __FUNCTION__);
      of_tok_end(tok_p);
      cm_cli_puts (cli_session_p, buff_a);
      return OF_FAILURE;
   }

   /* Remove all the delimiters(;) and copy all commands into temporary buffer */
   tmp_count_i = count_i;
   while(tmp_count_i>0)
   {
      tmp_count_i--;
      if(of_strcmp(temp_tokens_a[tmp2_i],";")==0)
      {
         delimiter_cnt_i++;
         tmp1_i = 0;
         ++tmp2_i;
         continue;  
      }
      else
      {
         of_strcpy(tmp_tk_a[delimiter_cnt_i][tmp1_i],temp_tokens_a[tmp2_i]);
         cmd_cnt_i[delimiter_cnt_i] = tmp1_i + 1; 
      }
      tmp1_i++;
      tmp2_i++;
   }

   tmp1_i=0;
   while(delimiter_cnt_i>=0)
   {
      count_i = cmd_cnt_i[tmp1_i];
      tokens_a = (char **)tmp_tk_a[tmp1_i];
      found_b = FALSE;

      if (count_i)
      {
         if (count_i > CM_CLI_MAX_CMDPARAMS)
         {
            sprintf (buff_a,
                  " Exceeds maximum Configuration Parameters count(%d)\n\r",
                  count_i);
            cm_cli_puts (cli_session_p, buff_a);
            of_tok_end(tok_p);
            return OF_FAILURE;
         }

         /* Search for the command in the Default command list and invoke the appropriate
          * callback API  */
         for (i = 0; i < CM_CLI_MAX_CMD; i++)
         {
            if (!of_strcmp ((char*)tokens_a, cm_cli_default_cmd_list[i].command_name_c))
            {
               /* check whether the entered values are valid */
               if(count_i > 1)
               {
                  table_a = (char **)tmp_tk_a[0][1];
                  of_strcpy (tmp_str_a, (char *)table_a);
                  CM_DLLQ_SCAN (&cli_session_p->current_node_p->child_list, dll_q_node_p,UCMDllQNode_t *)
                  {
                     data_ele_node_p = CM_DLLQ_LIST_MEMBER (dll_q_node_p, struct cm_cli_data_ele_node *,list_node);
                     if (of_strcmp (data_ele_node_p->node_name, tmp_str_a) == 0) //table name
                     {
                        if (data_ele_node_p->node_type == CM_DMNODE_TYPE_TABLE)
                        {
                           table_a = (char **)tmp_tk_a[0][2];
                           of_strcpy (tmp_str_a, (char *)table_a);
                           if( (of_strchr(tmp_str_a, '{'))
                                 || (of_strchr(tmp_str_a, '}')) )
                           {
                              cm_cli_puts (cli_session_p, "\n\r Invalid command.\n\r");
                              of_tok_end(tok_p);
                              return OF_FAILURE;
                           }
                        }
                     }
                  }
               }

               /* Extract the parameter values and fill the params T_structure */
               found_b = TRUE;
               return_value = cm_cli_validate_cmd_and_fill_params (i, count_i, (char (*)[512])tokens_a,params, &param_cnt_ui);
               if (return_value != OF_SUCCESS)
               {
                  of_tok_end(tok_p);
                  return OF_FAILURE;
               }
               cli_session_p = tmp_cli_session_p;

               return_value = (*(cm_cli_default_cmd_list[i].call_back_function_p)) (cli_session_p,
                     params,
                     param_cnt_ui);
               break;
            }
         }
         if (found_b == FALSE)
         {
            if(count_i == 1)
            {
               CM_DLLQ_SCAN (&cli_session_p->current_node_p->child_list, dll_q_node_p,UCMDllQNode_t *)
               {
                  data_ele_node_p = CM_DLLQ_LIST_MEMBER (dll_q_node_p, struct cm_cli_data_ele_node *,list_node);

                  if (of_strcmp (data_ele_node_p->node_name, (char *)tokens_a) == 0)
                  {
                     found_b = TRUE;
                     if (data_ele_node_p->node_type == CM_DMNODE_TYPE_TABLE)
                     {
                        if(data_ele_node_p->system_command_b == TRUE)
                        {
                           if((return_value1 = cm_cli_fill_params(count_i,(char (*)[512])tokens_a,params, &param_cnt_ui))!= OF_SUCCESS)
                           {
                              cm_cli_puts(cli_session_p,"\r\n Fill Parameters failed.\r\n");
                              break;
                           }
                           if(is_tab_pressed_b == TRUE)
                              cm_cli_display_instances_and_key_params (data_ele_node_p->node_name,TRUE);
                           else
                              cm_cli_system_command (params, param_cnt_ui);
                           break;
                        }
                        else
                        {
                           /* Display all the Instances and KeyParameter Name for that
                            * Table */
                           cm_cli_display_instances_and_key_params (data_ele_node_p->node_name,FALSE);
                        }
                     }
                     else if (data_ele_node_p->node_type == CM_DMNODE_TYPE_ANCHOR)
                     {
                        cm_cli_validate_and_change_dir(data_ele_node_p->node_name);
                     }
                  }
               }
               if (found_b == FALSE)
               {
                  cm_cli_puts (cli_session_p, "\n\r Invalid command.\n\r");
                  of_tok_end(tok_p);
                  return OF_FAILURE;
               }
            }
            else
            {
               /* May be add Check in the Child List */
               CM_DLLQ_SCAN (&(cli_session_p->current_node_p->child_list), dll_q_node_p,UCMDllQNode_t *)
               {
                  data_ele_node_p = CM_DLLQ_LIST_MEMBER (dll_q_node_p,struct cm_cli_data_ele_node *, list_node);

                  of_strcpy (tmp_str_a, (char *)tokens_a);
                  if ((!of_strcmp (tmp_str_a, data_ele_node_p->node_name)) &&
                        (data_ele_node_p->node_type == CM_DMNODE_TYPE_TABLE))
                  {
                     if(data_ele_node_p->system_command_b == TRUE)
                     {
                        if((return_value1 = cm_cli_fill_params(count_i,(char (*)[512])tokens_a,params, &param_cnt_ui))!= OF_SUCCESS)
                        {
                           cm_cli_puts(cli_session_p,"\r\n Fill Parameters failed.\r\n");
                           break;                
                        }

                        found_b = TRUE;
                        cm_cli_system_command (params, param_cnt_ui);
                        break;
                     }
                     table_a = (char **)tmp_tk_a[0][2];
                     of_strcpy (tmp_str_a, (char *)table_a);
                     if( (of_strchr(tmp_str_a, '{'))
                           || (of_strchr(tmp_str_a, '}')) )
                     { 
                        /* Invalid command entered. Hence ending */
                        break;
                     }
                     else
                     {
                        found_b = TRUE;
                        if(data_ele_node_p->non_config_leaf_node_b == TRUE)
                        {
                           cm_cli_puts(cli_session_p," This command is Read-Only and cannot be configured.\r\n");
                           return OF_FAILURE;
                        }
                        else if( (cli_session_p->no_of_cds == 1 && data_ele_node_p->parent_trans_b == TRUE) || 
                              (cli_session_p->no_of_cds > 1 && data_ele_node_p->parent_trans_b == TRUE && cli_session_p->curr_table_ctx_b == FALSE) ) /* ||
                              (cli_session_p->cd_table_b == TRUE && data_ele_node_p->parent_trans_b == FALSE && cli_session_p->dmpath_a[of_strlen(cli_session_p->dmpath_a)-1] != '}'))*/
                        {
                           cm_cli_puts(cli_session_p," Cannot add table, as context of the parent table is missing.\r\n");
                           return OF_FAILURE;
                        }
                        else if(cli_session_p->cd_table_b == FALSE && cli_session_p->current_node_p->global_trans_b == TRUE && 
                              data_ele_node_p->parent_trans_b == FALSE && cli_session_p->context_b == TRUE)
                        {
                           cm_cli_puts(cli_session_p," Cannot add table as it does not use parent transaction and the parent is transaction based record.\r\n");
                           cm_cli_puts(cli_session_p," Enter into existing parent table using cd table-name{keyparam} and then try to add this table.\r\n");
                           return OF_FAILURE;
                        }
                        /*    else if(cli_session_p->context_b == FALSE && data_ele_node_p->parent_trans_b == TRUE)//Deepthi
                              {
                              cm_cli_puts(cli_session_p,"Cannot add transaction based table as context missing.\r\n");
                              return OF_FAILURE;
                              }*/
                        else
                        {
                           if((return_value1 = cm_cli_fill_params(count_i,(char (*)[512])tokens_a,params, &param_cnt_ui))!= OF_SUCCESS)
                           {
                              cm_cli_puts(cli_session_p,"\r\n Fill Parameters failed.\r\n");
                              break;
                           }
                           if(cm_cli_frame_and_send_add_params_to_je (params, count_i) == OF_SUCCESS)
                           {
                              cli_session_p->curr_table_ctx_b = TRUE;
                              if(cli_session_p->cd_table_b == TRUE)
                              {
                                 cli_session_p->no_of_cds++;
                                 /*  cli_session_p->no_of_cds--;
                                     if(cli_session_p->no_of_cds == 0)
                                     cli_session_p->cd_table_b = FALSE;*/
                                 //    cli_session_p->no_of_cds = 0;
                                 //    cli_session_p->cd_table_b = FALSE;
                              }
                           }
                        }
                        break;
                     }
                  }
               }
            }
         }
         if (found_b == FALSE)
         {
            cm_cli_puts (cli_session_p, "\n\r Invalid command.\n\r");
            of_tok_end(tok_p);
            return OF_FAILURE;
         }

      }
      tmp1_i++; 
      delimiter_cnt_i-- ;
      if(delimiter_cnt_i<0)
         break;
   }
   of_tok_end (tok_p);
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_fill_command_info
 * Description   : This API is used to fill the command structure information.
 * Input params  : cmd_i - Command ID
 *                 dm_path_p - Pointer to Data model path
 *                 count_i - Name-Value pair count
 *                 nv_pair_p - Pointer to NvPair(s)
 * Output params : cmd_p - Pointer to command structure
 * Return value  : NONE
 *****************************************************************************/
void cm_cli_fill_command_info (int32_t cmd_i, char * dm_path_p,
                              int32_t count_i, struct cm_nv_pair * nv_pair_p,
                              struct cm_command * cmd_p)
{
   of_memset (cmd_p, 0, sizeof (struct cm_command));
   cmd_p->command_id = cmd_i;
   cmd_p->dm_path_p = dm_path_p;
   cmd_p->nv_pair_array.count_ui = count_i;
   cmd_p->nv_pair_array.nv_pairs = nv_pair_p;
}

/******************************************************************************
 * Function Name : cm_cli_validate_cmd_and_fill_params
 * Description   : This API is used to validate the Standard CLI command and
 *                 fill the parameters
 * Input params  : cmd_id_i - CommandID
 *                 token_cnt_i -  Tokens count
 *                 tokens_a - array of tokens(strings)
 * Output params : params_p - pointer to parameter structure.
 * Return value  : OF_SUCCESS / OF_FAILURE
 *****************************************************************************/
 int32_t cm_cli_validate_cmd_and_fill_params (int32_t cmd_id_i,
                                                   int32_t token_cnt_i,
                                                   char (*tokens_a)[512],
                                                   struct cm_cli_params * params_p,
                                                   uint32_t * param_cnt_ui_p)
{
   int32_t i, j, tmp_val_i = 0;
   char *tmp_p = NULL;

   /* Verify Parameter count */
   if ((cm_cli_default_cmd_list[cmd_id_i].param_list_p) &&
         ((cm_cli_default_cmd_list[cmd_id_i].param_list_p->param_type == CM_CLI_MANDATORY)
          && (cm_cli_default_cmd_list[cmd_id_i].param_count_us != (token_cnt_i - 1)))
         || ((cm_cli_default_cmd_list[cmd_id_i].param_count_us < (token_cnt_i - 1))))
   {
      cm_cli_puts (cli_session_p, " Invalid number of parameters passed.\n\r");
      sprintf (buff_a, "%s\n\r", cm_cli_default_cmd_list[cmd_id_i].description_p);
      cm_cli_puts (cli_session_p, buff_a);
      return OF_FAILURE;
   }

   /* Verify the Datatype for the entered input */
   for (i = 0, j = 1;i < cm_cli_default_cmd_list[cmd_id_i].param_count_us && tokens_a[j]; i++, j++)
   {
      switch (cm_cli_default_cmd_list[cmd_id_i].param_list_p[i].param_data_type)
      {
         case CM_CLI_CHAR:
            break;
         case CM_CLI_INT:
         case CM_CLI_UINT:
            if(cm_cli_verify_int_format(tokens_a[j], &tmp_val_i) == OF_SUCCESS)
            {
               params_p[i].param_value = tmp_val_i;
               params_p[i].param_data_type = CM_CLI_INT;
            }
            else
            {
               sprintf(buff_a, " Expecting integer value for parameter\n\t");
               cm_cli_puts(cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_CLI_ARRAY:
            if (of_strlen (tokens_a[j]) > cm_cli_default_cmd_list[cmd_id_i].param_list_p->param_size)
            {
               sprintf (buff_a, " Parameter size (%d) exceeds maximum size(%d)\n\r",
                     of_strlen (tokens_a[j]),
                     cm_cli_default_cmd_list[cmd_id_i].param_list_p->param_size);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }

            tmp_p = (char *) of_calloc (1, of_strlen (tokens_a[j]) + 1);
            if (!tmp_p)
            {
#ifdef CM_CLI_DEBUG 
               sprintf (buff_a, "%s :: Memory allocation failed\n\r", __FUNCTION__);
               cm_cli_puts (cli_session_p, buff_a);
#endif
               return OF_FAILURE;
            }
            of_strcpy (tmp_p, tokens_a[j]);
            params_p[i].param_value = (ulong) tmp_p;
            params_p[i].param_data_type = CM_CLI_ARRAY;
            break;
         case CM_CLI_IPADDR:
            break;
         case CM_CLI_MACADDR:
            break;

      }
   }

   params_p[i].param_data_type = CM_CLI_NOMORE;
   params_p[i].param_value = 0;
   *param_cnt_ui_p = token_cnt_i - 1;

   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_fill_params 
 * Description   : This API is used to fill name value pairs of a command
 * Input params  : tkn_cnt_i - token count
                   tokens_a - array of token 
 * Output params : params_p - Array of configured params
                   param_cnt_ui_p - no of configured parameters
 * Return value  : OF_FAILURE/OF_SUCCESS
 *****************************************************************************/
int32_t cm_cli_fill_params(int32_t tkn_cnt_i,char (*tokens_a)[512],struct cm_cli_params * params_p,uint32_t * param_cnt_ui_p)
{
   uint32_t ii,jj;
   uint32_t cnt_i=0;
   char *tmp_p=NULL;


   for(ii=0,jj=0; jj < tkn_cnt_i;ii++,jj++)
   {
      if (of_strlen (tokens_a[jj]) > CM_CLI_DEFAULT_ARRAY_LENGTH )
      {
         sprintf (buff_a, " Parameter size (%d) exceeds maximum size(%d)\n\r",
               of_strlen (tokens_a[jj]),
               CM_CLI_DEFAULT_ARRAY_LENGTH);
         cm_cli_puts (cli_session_p, buff_a);
         return OF_FAILURE;
      }

      tmp_p = (char *) of_calloc (1, of_strlen (tokens_a[jj]) + 1);
      if (!tmp_p)
      {
#ifdef CM_CLI_DEBUG 
         sprintf (buff_a, "%s :: Memory allocation failed\n\r", __FUNCTION__);
         cm_cli_puts (cli_session_p, buff_a);
#endif
         return OF_FAILURE;
      }

      of_strcpy (tmp_p, tokens_a[jj]);
      params_p[ii].param_value = (ulong) tmp_p;
      params_p[ii].param_data_type = CM_CLI_ARRAY;
      cnt_i ++;
   }

   *param_cnt_ui_p = cnt_i;
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_display_result
 * Description   : This API is used to display the Result received from UCM
 *                 JE. 
 * Input params  : result_p - pointer to UCM Result structure
 * Output params : NONE 
 * Return value  : OF_SUCCESS / OF_FAILURE
 *****************************************************************************/
void cm_cli_display_result (struct cm_result * result_p)
{
   char *error_p;
   int32_t error_len_i = 0;

   switch (result_p->result_code)
   {
      case CM_JE_SUCCESS:
#if 0
         switch (result_p->CommandInfo.ucmd_id_i)
         {
            case CM_CMD_COMPARE_PARAM:
               {
                  /* Display Name-Value pairs for element exists in Config Session and 
                   * that exists in Security Application database */
               }
               break;
            case CM_CMD_GET_CONFIG_SESSION:
               {
                  /* Display the DMPath, Command and Name-Value pairs for each DMPath
                  */
               }
               break;
         }
#endif
         cm_cli_puts (cli_session_p, "\n\r");
         if (result_p->result.error.result_string_p)
         {
            cm_cli_puts(cli_session_p,result_p->result.error.result_string_p);
         }
         cm_cli_puts(cli_session_p," Committed the configuration successfully.\n\r");
         cm_cli_puts (cli_session_p, "\n\r");
         if (result_p->result.success.reserved)
         {
            cm_je_get_success_msg (result_p->result.success.reserved, buff_a);
            cm_cli_puts (cli_session_p, buff_a);
         }
         cm_cli_puts (cli_session_p, "\n\r");
         break;
      case CM_JE_FAILURE:
         cm_cli_puts (cli_session_p, "\n\t");
         if (result_p->result.error.result_string_p)
         {
            error_len_i = of_strlen(result_p->result.error.result_string_p);
            error_p = of_calloc(1,error_len_i+20);

            if(!error_p)
            {
               UCMCLIDEBUG ("Memory allocation failed");          
               return;
            }
            sprintf (error_p, " Error :: %s\n\t",
                  result_p->result.error.result_string_p);
            cm_cli_puts (cli_session_p, error_p);
            of_free(error_p);
            error_p = NULL;
            error_len_i = 0;
         }
         else
         {
            sprintf (buff_a, " Error :: \n\t");
            cm_cli_puts (cli_session_p, buff_a);
         }
         /* Display Error Name Value Pair */
         if (result_p->result.error.error_code)
         {
            cm_je_get_err_msg (result_p->result.error.error_code, buff_a);
            cm_cli_puts (cli_session_p, buff_a);
            cm_cli_puts (cli_session_p, "\n\t");

            /* Display DM Path Length  */
            if (result_p->result.error.dm_path_p)
            {
               //sprintf (buff_a, "DMPath :: %s\n\t", result_p->result.error.dm_path_p);
               //cm_cli_puts (cli_session_p, buff_a);
               error_len_i = of_strlen(result_p->result.error.dm_path_p);
               error_p = of_calloc(1, error_len_i+15);
               if(error_p)
               {
                  sprintf (error_p, " DMPath :: %s\n\t", result_p->result.error.dm_path_p);
                  cm_cli_puts (cli_session_p, error_p);
                  of_free(error_p);
                  error_p = NULL;
                  error_len_i = 0;
               }
            }

            if (result_p->result.error.nv_pair.name_p)
            {
               //sprintf (buff_a, "Name :: %s  Value :: %s\n\r",
               //         result_p->result.error.nv_pair.name_p,
               //       (char*)result_p->result.error.nv_pair.value_p);
               //cm_cli_puts (cli_session_p, buff_a);
               error_len_i = of_strlen(result_p->result.error.nv_pair.name_p) +
                  of_strlen( (char*)result_p->result.error.nv_pair.value_p);
               error_p = of_calloc(1, error_len_i+25);
               if(error_p)
               {
                  sprintf (error_p, " Name :: %s  Value :: %s\n\r",
                        result_p->result.error.nv_pair.name_p,
                        (char*)result_p->result.error.nv_pair.value_p);
                  cm_cli_puts (cli_session_p, error_p);
                  of_free(error_p);
                  error_p = NULL;
                  error_len_i = 0;
               }
            }
            cm_cli_puts (cli_session_p, "\n\r");
         }
         break;
      default:
         break;
   }
}

/******************************************************************************
 * Function Name : cm_cli_get_context_prompt
 * Description   : This API is used to get the CLI Prompt.
 * Input params  : edit_line_ctx_p -pointer to Editor line context
 * Output params : NONE
 * Return value  : pointer to character buffer that contains the CLI Prompt
 *****************************************************************************/
char* cm_cli_get_context_prompt (edit_line * edit_line_ctx_p)
{
   struct cm_dm_node_info *node_info_p = NULL;
   struct cm_array_of_uints *id_array_p = NULL;
   uint32_t len_ui = 0;
   int32_t return_value;
   unsigned char is_table_b;

   if (cli_session_p->root_c[0] == '\0')
   {
      /* Getting Root Node */
      return_value = cm_dm_get_first_node_info (NULL, UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM,
            cli_session_p->tnsprt_channel_p,
            CM_CLI_MGMT_ENGINE_ID, &cli_session_p->role_info,
            &node_info_p, &id_array_p, &len_ui);
      if ((return_value == OF_SUCCESS) && (node_info_p))
      {
         of_strcpy (cli_session_p->root_c, node_info_p->name_p);
         of_strcpy (cli_session_p->prompt[cli_session_p->index_i++], cli_session_p->root_c);
         of_strcpy (cli_session_p->current_path, cli_session_p->root_c);
         of_strcpy (cli_session_p->dmpath_a, cli_session_p->root_c);
         of_strcat (cli_session_p->current_path, "/>");
         if (node_info_p)
         {
            cm_cli_free_dm_node_info (node_info_p, TRUE);
         }

         cli_session_p->current_node_p =
            (struct cm_cli_node *) of_calloc (1, sizeof (struct cm_cli_node));
         if (cli_session_p->current_node_p == NULL)
         {
            cm_cli_puts(cli_session_p, " Failed to Login\n\r");
            cm_cli_terminate_transport_channel (cli_session_p->tnsprt_channel_p);
            exit(0);
         }
         of_strcpy (cli_session_p->current_node_p->node_name, cli_session_p->root_c);
         CM_DLLQ_INIT_LIST (&cli_session_p->current_node_p->child_list);
         CM_DLLQ_INIT_LIST (&cli_session_p->current_node_p->data_elements_list);
         cli_session_p->cd_table_b = FALSE;
         cli_session_p->no_of_cds = 0;
         cli_session_p->curr_table_ctx_b = FALSE;
         cm_cli_update_child_list_and_prompt (cli_session_p->dmpath_a, cli_session_p->root_c, FALSE, &is_table_b);
         return cli_session_p->current_path;
      }
      else
      {
         cm_cli_puts (cli_session_p, " Get root node failed.\n\r");
         cm_cli_terminate_transport_channel (cli_session_p->tnsprt_channel_p);
         exit(0);
      }
   }
   else
   {
      return (cli_session_p->current_path);
   }

}

/***************************************************************************
 * Function Name : cm_cli_handle_signal 
 * Description   : This function is called when ever signal is raised in main
 * InputParams   : signum
 * Output Params : NONE
 * Return value  : None
 ***************************************************************************/
void cm_cli_handle_signal (int16_t signal_num_i)
{
   if (signal_num_i == SIGTERM || signal_num_i == SIGHUP)
   {
      printf ("SIGTERM ...\n");
      //    cm_cli_logout ();

      exit (0);
   }

   if (signal_num_i == SIGALRM)
   {
      printf ("\n CLI session timed out due to in-activity.\n");
      cm_cli_cleanup();
      exit (0);
   }
   return;
}

void cm_cli_cleanup(void)
{
   struct cm_result *result_p=NULL;
   if (cli_session_p->config_session_p)
   {
      cm_config_session_end (cli_session_p->config_session_p,
            CM_CMD_CONFIG_SESSION_REVOKE, &result_p);
      if(result_p)
      {
         UCMFreeUCMResult (result_p);
      }
   }

   if (cli_session_p->current_node_p)
   {
      cm_cli_free_child_nodes (cli_session_p->current_node_p);
      of_free(cli_session_p->current_node_p);
   }

   of_history_end (cli_session_p->history_p);
   of_el_end (cli_session_p->edit_line_ctx_p);
   of_free (cli_session_p); 
   return; 
}

int16_t cli_fgets (struct cm_cli_session * cli_session_pionCnxt,
                  char * buff,
                  uint32_t lsize_ui)
{
   char *buff_p = NULL; 

   if(!buff) 
      return OF_FAILURE;

#ifdef OF_CMS_SUPPORT
   if(cli_session_pionCnxt->iSessType == OF_CMS)
   {
      return CliGets(cli_session_pionCnxt, buff);
   }
   else
#endif  /* OF_CMS_SUPPORT */
   {
      if(!of_fgets (buff, lsize_ui, stdin))
      {
         return 0;
      }
      buff_p = buff;
      buff_p += (of_strlen(buff)-1);
      if(*buff_p == '\n')
         *buff_p = '\0';
      return(of_strlen(buff) + 1);
   }
   return OF_FAILURE;
}


/***************************************************************************
 * Function Name : cm_cli_puts 
 * Description   : This function outputs a NULL terminated T_string to CLI.
 *
 * Input         : char *            -- A pointer to the T_string to be
 *                                         output
 *    CliSessionContext_t *cli_session_pContext
 * Output        : None
 * Return value  : Number of bytes written on SUCCESS, -1 on FAILURE 
 ***************************************************************************/
int16_t cm_cli_puts (struct cm_cli_session * cli_session_p, char * buff_p)
{
   if (buff_p)
   {
      if (fputs (buff_p, stdout) == EOF)
      {
         perror ("CliPuts(): Err");
         return OF_FAILURE;
      }
      return of_strlen (buff_p);
   }

   return OF_FAILURE;
}

/***************************************************************************
* Function Name : cm_cli_get_char 
* Description   : This function returns a character from the standerad input
* Input         : CliSessionContext_t* -- A pointer to session context
*                 char *            -- A pointer to the character
*                                         to be inputted
* Output        : A pointer to the character inputted
* Return value  : Number of bytes read on SUCCESS, -1 on FAILURE 
***************************************************************************/
int16_t cm_cli_get_char (struct cm_cli_session * cli_session_p, char * char_p)
{
   unsigned char buff[10] = { '\0' };

   *char_p = fgetc (CM_CLI_STDIN);
   if (*char_p != '\n')
      buff[0] = fgetc (CM_CLI_STDIN);
   return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_cli_update_child_list_and_prompt
 * Description   : This API is used to update child list and prompt under a table
 * Input         : dm_path_p - Data model path
                   prompt_p - cli prompt to navigate
 * Output        : is_table_b - boolean variable to figureout the current node is table or not 
 * Return value  : OF_FAILURE/OF_SUCCESS
 ***************************************************************************/
int32_t cm_cli_update_child_list_and_prompt (char * dm_path_p, char * prompt_p,
                                        unsigned char add_b, unsigned char *is_table_b)
{
   struct cm_dm_node_info *node_info_p = NULL, *tmp_node_info_p;
   struct cm_array_of_structs *child_info_arr_p = NULL;
   struct cm_cli_session *tmp_cli_session_p = cli_session_p;
   int32_t ii;
   uint16_t dmpath_len_ui;

   *is_table_b = FALSE;

   if (!dm_path_p || !prompt_p)
   {
      cm_cli_puts (cli_session_p, " Invalid input.\n\r");
      return OF_FAILURE;
   }

   dmpath_len_ui = of_strlen (dm_path_p);
   if (cm_dm_get_node_and_child_info (tmp_cli_session_p->tnsprt_channel_p,
            dm_path_p, CM_CLI_MGMT_ENGINE_ID,
            &tmp_cli_session_p->role_info,
            &node_info_p, &child_info_arr_p) != OF_SUCCESS)
   {
#ifdef CM_CLI_DEBUG 
      sprintf (buff_a, "%s :: cm_dm_get_node_and_child_info failed\n\r",
            __FUNCTION__);
      cm_cli_puts (cli_session_p, buff_a);
#endif
      return OF_FAILURE;
   }

   if (node_info_p &&
         (node_info_p->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR))
   {
      sprintf (buff_a, "(%s) is not a directory.\n\r", node_info_p->name_p);
      cm_cli_puts (cli_session_p, buff_a);
      return OF_FAILURE;
   }

   tmp_cli_session_p = cli_session_p;

   /* Free the existing childnode infor and dataelement list */
   if (tmp_cli_session_p->current_node_p)
   {
      /* Free old child nodes */
      cm_cli_free_child_nodes (tmp_cli_session_p->current_node_p);

      /* Fill the Data Element node list and append */
      of_strncpy (tmp_cli_session_p->current_node_p->node_name, node_info_p->name_p,
            of_strlen (node_info_p->name_p));
      /****
       * Key Child Info we need to copy. 
       ****/
      if (node_info_p->element_attrib.element_type == CM_DMNODE_TYPE_TABLE)
      {
         if ( (of_strcmp(cli_session_p->current_node_p->node_name, "vsg") == 0) && (dm_path_p[of_strlen(dm_path_p)-1] == '}'))
         {
            *is_table_b = FALSE;
         }
         else
            *is_table_b = TRUE;
         for (ii = 0; ii < child_info_arr_p->count_ui; ii++)
         {
            tmp_node_info_p = (struct cm_dm_node_info *) (child_info_arr_p->struct_arr_p) + ii;
            if (tmp_node_info_p->element_attrib.key_b == TRUE)
            {
               /* Copy KeyName and Key Type */
               cm_cli_copy_data_element_info (&tmp_cli_session_p->current_node_p->key_node_info,
                     tmp_node_info_p);
               break;
            }
         }
      }
      tmp_cli_session_p->current_node_p->node_type_ui = node_info_p->element_attrib.element_type;
      if(node_info_p->element_attrib.element_type == CM_DMNODE_TYPE_ANCHOR)
      {
         cm_cli_append_data_element_nodes (tmp_cli_session_p->current_node_p, child_info_arr_p, TRUE);
      }
      else
      {
         cm_cli_append_data_element_nodes (tmp_cli_session_p->current_node_p, child_info_arr_p, add_b);
      }
      tmp_cli_session_p->current_node_p->parent_trans_b =  node_info_p->element_attrib.parent_trans_b;
      tmp_cli_session_p->current_node_p->global_trans_b =  node_info_p->element_attrib.global_trans_b;
      tmp_cli_session_p->current_node_p->scalar_group_b =  node_info_p->element_attrib.scalar_group_b;
      tmp_cli_session_p->current_node_p->non_config_leaf_node_b =  node_info_p->element_attrib.non_config_leaf_node_b;
   }

   if (!add_b)
      cm_cli_update_prompt (prompt_p);

   if (node_info_p)
   {
      cm_cli_free_dm_node_info (node_info_p, TRUE);
   }

   if (child_info_arr_p)
   {
      cm_cli_free_array_of_node_info (child_info_arr_p);
   }

   return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_cli_free_child_nodes
 * Description   : This API is used to free allocated memory for child nodes
 * Input         : current_node_p - Node list
 * Output        : none 
 * Return value  : none
 ***************************************************************************/
void cm_cli_free_child_nodes (struct cm_cli_node * current_node_p)
{
   UCMDllQNode_t *tmp_p, *node_p;
   struct cm_cli_data_ele_node *data_node_p;

   if (!current_node_p)
   {
      return;
   }

   CM_DLLQ_DYN_SCAN (&current_node_p->child_list, node_p, tmp_p, UCMDllQNode_t *)
   {
      data_node_p = CM_DLLQ_LIST_MEMBER (node_p, struct cm_cli_data_ele_node *, list_node);
      cm_cli_data_element_node_free (data_node_p);
   }
   CM_DLLQ_INIT_LIST (&current_node_p->child_list);

   CM_DLLQ_DYN_SCAN (&current_node_p->data_elements_list, node_p, tmp_p, UCMDllQNode_t *)
   {
      data_node_p = CM_DLLQ_LIST_MEMBER (node_p, struct cm_cli_data_ele_node *, list_node);
      cm_cli_data_element_node_free (data_node_p);
   }
   CM_DLLQ_INIT_LIST (&current_node_p->data_elements_list);

   memset(current_node_p->node_name, 0 , sizeof(current_node_p->node_name));
   memset(current_node_p->install_path, 0 , sizeof(current_node_p->install_path));
   cm_cli_free_array_of_strings ();
}

/***************************************************************************
 * Function Name : cm_cli_append_data_element_nodes
 * Description   : THis api is used to append data model nodes to cli list node
 * Input         : current_node_p - Node list
                   data_element_nodes_p - Array of node lists
 * Output        : none 
 * Return value  : none
 ***************************************************************************/
 void cm_cli_append_data_element_nodes (struct cm_cli_node * current_node_p,
                                 struct cm_array_of_structs * data_element_nodes_p,
         unsigned char add_b)
{
   int32_t ii;
   struct cm_cli_data_ele_node *node_p;
   struct cm_array_of_structs *tmp_p;

   tmp_p = data_element_nodes_p;
   /*Should not add anchors and tables to the array on 
    * igd/>vsg general
    * "igd/vsg general/>fir<tab>"
    */
   if( (of_strcmp(current_node_p->node_name, "vsg") == 0) && (add_b == TRUE))
      cm_cli_frame_command_name_list (tmp_p->count_ui, data_element_nodes_p, TRUE, TRUE);
   else  
      cm_cli_frame_command_name_list (tmp_p->count_ui, data_element_nodes_p, TRUE, FALSE);

   for (ii = 0; ii < tmp_p->count_ui; ii++)
   {
      node_p = (struct cm_cli_data_ele_node *) cm_cli_data_element_node_alloc ();
      if (node_p == NULL)
      {
#ifdef CM_CLI_DEBUG 
         sprintf (buff_a, "%s :: Memory allocation failed\n\r", __FUNCTION__);
         cm_cli_puts (cli_session_p, buff_a);
#endif
         return;
      }
      CM_DLLQ_INIT_NODE (&node_p->list_node);
      cm_cli_copy_data_element_info (node_p, (struct cm_dm_node_info *) tmp_p->struct_arr_p + ii);
      if (node_p->node_type == CM_DMNODE_TYPE_SCALAR_VAR)
      {
         CM_DLLQ_APPEND_NODE (&cli_session_p->current_node_p->data_elements_list,
               &node_p->list_node);
      }
      else
      {
         CM_DLLQ_APPEND_NODE (&cli_session_p->current_node_p->child_list, &node_p->list_node);
      }
   }
}

/***************************************************************************
 * Function Name : cm_cli_frame_command_name_list
 * Description   : This API is used to frame command name strings
 * Input         : node_info_struct_array_p - Array of node structures
 * Output        : none
 * Return value  : OF_FAILURE/OF_SUCCESS
 ***************************************************************************/
 int32_t cm_cli_frame_command_name_list (uint32_t count_ui,
                                struct cm_array_of_structs * node_info_struct_array_p,
        unsigned char add_b, unsigned char vsg_add_b)
{
   struct cm_array_of_strings *string_arr_p;
   int32_t ii, jj;
   uint32_t str_len_ui, cmd_count_ui = 0, node_cnt_ui = 0;
   unsigned char add_b_to_arr = FALSE;

   string_arr_p =
      (struct cm_array_of_strings *) of_calloc (1, sizeof (struct cm_array_of_strings));
   if (string_arr_p == NULL)
   {
#ifdef CM_CLI_DEBUG 
      sprintf (buff_a, "%s :: Memory allocation failed\n\r", __FUNCTION__);
      cm_cli_puts (cli_session_p, buff_a);
#endif
      return OF_FAILURE;
   }

   for(ii = 0; ii < CM_CLI_MAX_CMD; ii ++)
   {
      if ((cm_cli_default_cmd_list[ii].session_b == FALSE) || 
            (((cli_session_p->context_b) || (add_b)) &&
             (cm_cli_default_cmd_list[ii].session_b == TRUE)))
      {       
         cmd_count_ui = cmd_count_ui + 1;
      }       
   }

   if(count_ui && node_info_struct_array_p)
   {
      for (ii = 0; ii< node_info_struct_array_p->count_ui; ii++)
      {
         if((((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.visible_b == TRUE)) 
         {
            if(vsg_add_b == TRUE &&  (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type ==CM_DMNODE_TYPE_SCALAR_VAR) &&  (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.key_b == FALSE) )
            {
               if(((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.non_config_leaf_node_b != TRUE)
               {
                  node_cnt_ui++;
               }
            }
            else if((vsg_add_b == FALSE) && ((add_b == TRUE) ||
                     (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type !=CM_DMNODE_TYPE_SCALAR_VAR)) &&  (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.key_b == FALSE))
            {
               if(((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type != CM_DMNODE_TYPE_SCALAR_VAR)
                  node_cnt_ui++;       
               else //if scalar verify non config leaf node and increment the count
               {
                  if(((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.non_config_leaf_node_b != TRUE)
                  {
                     node_cnt_ui++;
                  }
               }
               /* //VoritQa requirement
                  if((((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR) && (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.mandatory_b == FALSE) && (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.cli_identifier_p) )
                  {
                  node_cnt_ui++;
                  }*/
            }
         }
      }
   }
   string_arr_p->count_ui = node_cnt_ui + cmd_count_ui;

   string_arr_p->string_arr_p = (char **) of_calloc (string_arr_p->count_ui,
         sizeof (unsigned char *));
   if (string_arr_p->string_arr_p == NULL)
   {
#ifdef CM_CLI_DEBUG 
      sprintf (buff_a, "%s :: Memory allocation failed\n\r", __FUNCTION__);
      cm_cli_puts (cli_session_p, buff_a);
#endif
      of_free (string_arr_p);
      return OF_FAILURE;
   }

   /* Copy Default command names to String array */
   for (ii = 0, jj=0; ii < CM_CLI_MAX_CMD; ii++)
   {
      if ((cm_cli_default_cmd_list[ii].session_b == FALSE) ||
            (((cli_session_p->context_b)  || (add_b)) && 
             (cm_cli_default_cmd_list[ii].session_b == TRUE)))
      {
         str_len_ui = of_strlen (cm_cli_default_cmd_list[ii].command_name_c);
         string_arr_p->string_arr_p[jj] = (char *) of_calloc (1, str_len_ui + 1);
         if (string_arr_p->string_arr_p[jj] != NULL)
         {
            of_strncpy (string_arr_p->string_arr_p[jj],cm_cli_default_cmd_list[ii].command_name_c, str_len_ui);
            jj++;
         }
      }      
   }

   if(count_ui && node_info_struct_array_p)
   {
      for (ii = 0; ii< node_info_struct_array_p->count_ui; ii++)
      {
         add_b_to_arr = FALSE;
         if((((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.visible_b == TRUE))       
         {
            if(vsg_add_b == TRUE &&  (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type ==CM_DMNODE_TYPE_SCALAR_VAR) &&  (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.key_b == FALSE))
            {
               if(((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.non_config_leaf_node_b != TRUE)
               {
                  add_b_to_arr = TRUE;
               }
            }
            else if((vsg_add_b == FALSE) && ( (add_b == TRUE) ||
                     (((struct cm_dm_node_info  *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type !=CM_DMNODE_TYPE_SCALAR_VAR)) &&  (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.key_b == FALSE))
            {
               if(((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type != CM_DMNODE_TYPE_SCALAR_VAR)
                  add_b_to_arr = TRUE;
               else //if scalar verify non config leaf node and add the variable
               {
                  if(((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.non_config_leaf_node_b != TRUE)
                  {
                     add_b_to_arr = TRUE;
                  }
               }
            }
            if(add_b_to_arr == TRUE)
            {
               if(((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p+ii)->name_p != NULL)
               {
                  //VoritQa requirement
                  if( (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.mandatory_b == FALSE) && (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.cli_identifier_p) )
                  {
                     str_len_ui = of_strlen (((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p+ii)->name_p);
                     str_len_ui +=  of_strlen (((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p+ii)->element_attrib.cli_identifier_p);
                     str_len_ui += 2; //for "(" and ")" in between name and identifier
                  }
                  else
                  {
                     str_len_ui =   of_strlen (((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p+ii)->name_p);
                  }
                  string_arr_p->string_arr_p[jj] = (char *) of_calloc (1, str_len_ui + 1);
                  if (string_arr_p->string_arr_p[jj] != NULL)
                  {
                     of_strncpy (string_arr_p->string_arr_p[jj],((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p +ii)->name_p, str_len_ui);

                     //VortiQa Requirement
                     if( (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.mandatory_b == FALSE) && (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.cli_identifier_p) )
                     {
                        of_strcat(string_arr_p->string_arr_p[jj], "(");
                        of_strcat(string_arr_p->string_arr_p[jj],((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p +ii)->element_attrib.cli_identifier_p);
                        of_strcat(string_arr_p->string_arr_p[jj], ")");
                     }
                     jj++;
                  }
                  /* //VoritQa requirement
                     if((((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.element_type == CM_DMNODE_TYPE_SCALAR_VAR) && (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.mandatory_b == FALSE) && (((struct cm_dm_node_info *)node_info_struct_array_p->struct_arr_p+ii)->element_attrib.cli_identifier_p) )
                     {
                     str_len_ui = of_strlen (((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p+ii)->element_attrib.cli_identifier_p);
                     string_arr_p->string_arr_p[jj] = (char *) of_calloc (1, str_len_ui + 1);
                     if (string_arr_p->string_arr_p[jj] != NULL)
                     {
                     of_strncpy (string_arr_p->string_arr_p[jj],((struct cm_dm_node_info *) node_info_struct_array_p->struct_arr_p +ii)->element_attrib.cli_identifier_p, str_len_ui);
                     jj++;
                     }
                     }*/
               }
            }
         }
      }
   }
   cli_session_p->string_arr_p = string_arr_p;
   return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_cli_free_array_of_strings
 * Description   : This API is used to free allocated memory for array of command name strings
 * Input         : none
 * Output        : none
 * Return value  : none
 ***************************************************************************/
void cm_cli_free_array_of_strings (void)
{
   int32_t ii;
   struct cm_array_of_strings *string_arr_p = cli_session_p->string_arr_p;

   if (string_arr_p)
   {
      for (ii = 0; ii < string_arr_p->count_ui; ii++)
      {
         of_free (string_arr_p->string_arr_p[ii]);
      }
      of_free (string_arr_p->string_arr_p);
      of_free (string_arr_p);
      cli_session_p->string_arr_p = NULL;
   }
}

/***************************************************************************
 * Function Name : cm_cli_copy_data_element_info
 * Description   : This API is used to copy Data model tree node info into cli element node info
 * Input         : node_info_p - Data Model node info
 * Output        : node_p - CLI Node elements
 * Return value  : none
 ***************************************************************************/
void cm_cli_copy_data_element_info (struct cm_cli_data_ele_node * node_p,
                              struct cm_dm_node_info * node_info_p)
{

   int32_t str_enum_cnt_ui = 0;
   int32_t frd_enum_cnt_ui = 0; 

   /* copy name */
   of_strcpy (node_p->node_name, node_info_p->name_p);
   if (node_info_p->friendly_name_p)
   {
      node_p->friendly_name_p = (char *) of_calloc (1,
            of_strlen
            (node_info_p->friendly_name_p) + 1);
      if (node_p->friendly_name_p)
      {
         of_strcpy (node_p->friendly_name_p, node_info_p->friendly_name_p);
      }
   }

   node_p->node_type = node_info_p->element_attrib.element_type;
   node_p->system_command_b = node_info_p->element_attrib.system_command_b;
   node_p->non_config_leaf_node_b = node_info_p->element_attrib.non_config_leaf_node_b;

   node_p->parent_trans_b = node_info_p->element_attrib.parent_trans_b;
   node_p->visible_b = node_info_p->element_attrib.visible_b;

   if (node_p->node_type == CM_DMNODE_TYPE_SCALAR_VAR)
   {
      node_p->key_b = node_info_p->element_attrib.key_b;
      node_p->mandatory_b = node_info_p->element_attrib.mandatory_b;
      node_p->data_type = node_info_p->data_attrib.data_type;
   }
   if (node_info_p->description_p)
   {
      node_p->description_p = (char *) of_calloc (1,
            of_strlen
            (node_info_p->description_p) + 1);
      if (node_p->description_p)
      {
         of_strcpy (node_p->description_p, node_info_p->description_p);
      }
   }
   if(node_info_p->element_attrib.command_name_p)
   {
      node_p->command_name_p = (char *) of_calloc (1,
            of_strlen
            (node_info_p->element_attrib.command_name_p) + 1);
      if (node_p->command_name_p)
      {
         of_strcpy(node_p->command_name_p, node_info_p->element_attrib.command_name_p);
      }
   }
   if(node_info_p->element_attrib.cli_identifier_p)
   {
      node_p->cli_identifier_p = (char *) of_calloc (1,
            of_strlen(node_info_p->element_attrib.cli_identifier_p) + 1);
      if (node_p->cli_identifier_p)
      {
         of_strcpy(node_p->cli_identifier_p, node_info_p->element_attrib.cli_identifier_p);
      }
   }
   if(node_info_p->element_attrib.cli_idenetifier_help_p)
   {
      node_p->cli_idenetifier_help_p = (char *) of_calloc (1,
            of_strlen(node_info_p->element_attrib.cli_idenetifier_help_p) + 1);
      if (node_p->cli_idenetifier_help_p)
      {
         of_strcpy(node_p->cli_idenetifier_help_p, node_info_p->element_attrib.cli_idenetifier_help_p);
      }
   }
   of_memcpy (&node_p->data_attrib, &node_info_p->data_attrib,
         sizeof (node_p->data_attrib));

   if((node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING || node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)
         && node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
   {
      for(str_enum_cnt_ui = 0 ;str_enum_cnt_ui < node_info_p->data_attrib.attr.string_enum.count_ui; str_enum_cnt_ui++)
      {
         if(node_info_p->data_attrib.attr.string_enum.array[str_enum_cnt_ui])
         {
            node_p->data_attrib.attr.string_enum.array[str_enum_cnt_ui] = (char *) of_calloc (1,
                  of_strlen(node_info_p->data_attrib.attr.string_enum.array[str_enum_cnt_ui]) + 1);

            if(node_p->data_attrib.attr.string_enum.array[str_enum_cnt_ui])
               of_strcpy(node_p->data_attrib.attr.string_enum.array[str_enum_cnt_ui],node_info_p->data_attrib.attr.string_enum.array[str_enum_cnt_ui]);

         }
      }
      for(frd_enum_cnt_ui = 0 ;frd_enum_cnt_ui < node_info_p->data_attrib.attr.string_enum.count_ui; frd_enum_cnt_ui++)
      {
         if(node_info_p->data_attrib.attr.string_enum.aFrdArr[frd_enum_cnt_ui])
         {
            node_p->data_attrib.attr.string_enum.aFrdArr[frd_enum_cnt_ui] = (char *) of_calloc (1,
                  of_strlen(node_info_p->data_attrib.attr.string_enum.aFrdArr[frd_enum_cnt_ui]) + 1);

            if(node_p->data_attrib.attr.string_enum.aFrdArr[frd_enum_cnt_ui])
               of_strcpy(node_p->data_attrib.attr.string_enum.aFrdArr[frd_enum_cnt_ui],node_info_p->data_attrib.attr.string_enum.aFrdArr[frd_enum_cnt_ui]);

         }
      }
   }


}

/***************************************************************************
 * Function Name : cm_cli_update_prompt
 * Description   : This API is used to update cli prompt in cli session
 * Input         : pparam_value - parameter value
 * Output        : none 
 * Return value  : none
 ***************************************************************************/
void cm_cli_update_prompt (char * pparam_value)
{
   if (pparam_value != NULL)
   {
      of_strcpy (cli_session_p->current_path, pparam_value);
   }
   else
   {
      of_strcpy (cli_session_p->current_path, cli_session_p->root_c);
      cli_session_p->index_i = 1;
   }
   of_strcat (cli_session_p->current_path, "/>");
}

/***************************************************************************
 * Function Name : cm_cli_find_directory
 * Description   : THis API is used to find Directory from a directory path
 * Input         : param_p - parameter name
                   dir_path_p - Directory path
 * Output        : none 
 * Return value  : OF_FAILURE/OF_SUCCESS
 ***************************************************************************/
int32_t cm_cli_find_directory (char * param_p, char * dir_path_p,
                             unsigned char find_b, int32_t param_cnt_i)
{
   uint16_t len_i;
   uint8_t index_i = 0;
   int32_t return_value;
   unsigned char found_b = FALSE, key_b = FALSE;
   char *tmp_p, token_a[CM_CLI_DEFAULT_ARRAY_LENGTH + 1] = { 0 };
   char dmpath_a[CM_CLI_MAX_PROMPT_LEN + 1] = { 0 };
   struct cm_dm_node_info *node_info_p=NULL;
   struct cm_array_of_structs *child_info_arr_p = NULL;

   tmp_p = param_p;
   len_i = of_strlen (tmp_p);

   if (!(isalpha (tmp_p[0])) && (tmp_p[0] != '.'))
   {
      cm_cli_puts (cli_session_p, " Invalid directory.\n\r");
      return OF_FAILURE;
   }

   if (find_b == FALSE)
   {
      cli_session_p->index_i = 0;
      while (TRUE)
      {
         return_value = cm_cli_get_token (tmp_p, token_a, &index_i, param_cnt_i);
         of_strcpy (cli_session_p->prompt[cli_session_p->index_i], token_a);
         cli_session_p->index_i++;
         if (return_value == EOLN)
         {
            break;
         }
      }
   }
   else
   {
      while (TRUE)
      {
         return_value = cm_cli_get_token (tmp_p, token_a, &index_i, param_cnt_i);

         if (token_a[0] == '\0')
         {
            break;
         }

         if (tmp_p[index_i] == ' ')
         {
            key_b = TRUE;
         }

         if (of_strcmp (token_a, ".") == 0)
         {
            found_b = TRUE;
            break;
         }
         else if (of_strcmp (token_a, "..") == 0)
         {
            if (cli_session_p->index_i && (cli_session_p->index_i > 1))
               cli_session_p->index_i = cli_session_p->index_i - 1;
         }
         else if ((token_a[0] == '.') && (of_strcmp (token_a, "..") != 0))
         {
            return OF_FAILURE;
         }
         else if (of_strchr (token_a, '{'))
         {
            if (cm_cli_verify_instance (token_a) == OF_FAILURE)
            {
               cm_cli_puts (cli_session_p, " Verification failed.\n\r");
               return OF_FAILURE;
            }
            of_strcpy (cli_session_p->prompt[cli_session_p->index_i], token_a);
            cli_session_p->index_i++;
         }
         else
         {
            cm_cli_get_dir_path (dmpath_a);
            cm_cli_convert_dir_to_dmpath (dmpath_a, dmpath_a);
            of_strcat (dmpath_a, ".");
            of_strncat (dmpath_a, token_a, len_i);
            if (cm_dm_get_node_and_child_info (cli_session_p->tnsprt_channel_p,
                    dmpath_a,
                    CM_CLI_MGMT_ENGINE_ID,
                    &cli_session_p->role_info,
                    &node_info_p, &child_info_arr_p) != OF_SUCCESS)
            {
              cm_cli_puts (cli_session_p, "get node and child info failed.\n\r");
              return OF_FAILURE;
            }
            if ((node_info_p->element_attrib.element_type == CM_DMNODE_TYPE_TABLE) && 
                (node_info_p->element_attrib.global_trans_b != 1) && 
                (of_strchr (token_a, '{') != TRUE))
            {
              cm_cli_free_node_info(node_info_p);
              cm_cli_cleanup_array_of_node_info(child_info_arr_p);
              cm_cli_puts (cli_session_p, "Invalid usage, Enter into table context as cd tablename{key}.\n\r");
              return OF_FAILURE;
            }
           else
           {
             of_strcpy (cli_session_p->prompt[cli_session_p->index_i], token_a);
             cli_session_p->index_i++;
           }
         }
         if (return_value == EOLN)
         {
            break;
         }
      }
   }

   return_value = cm_cli_get_dir_path (dir_path_p);
   if (return_value != OF_SUCCESS)
      return OF_FAILURE;
   return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_cli_get_token
 * Description   : this API is used to parse and tokenize instance path in to tokens
 * Input         : path_string - absolute path
                   index - index value
 * Output        : token - token
 * Return value  : end of tokens or not 
 ***************************************************************************/
enum tok_stat cm_cli_get_token (char * path_string, char * token, uint8_t * index, int32_t param_cnt_i)
{
   int16_t tmp_index_i;
   int16_t str_len_i;
   int16_t ii, jj = 0, space_cnt_i = 0;
   unsigned char brace_found_b = FALSE;

   if ((path_string == NULL) || (index == NULL) || (token == NULL))
   {
      return (EOLN);
   }
   tmp_index_i = *index;
   str_len_i = of_strlen (path_string);
   if (tmp_index_i >= str_len_i)
   {
      token[0] = '\0';
      return EOLN;
   }

   ii = tmp_index_i;

   while (ii < str_len_i)
   {
      if ((path_string[ii] == '{'))
      {
         brace_found_b = TRUE;
      }
      else if((path_string[ii] == '}'))
      {
         brace_found_b = FALSE;
      }
      if ((path_string[ii] == '/'))        /*( || (path_string[ii] == ' ')) */
      {
         if (ii == 0)              /* Root Directory */
            token[jj++] = '/';
         if (brace_found_b == FALSE)
         {
            token[jj] = '\0';
            ii++;
            *index = ii;
            break;
         }
      }

      token[jj++] = path_string[ii];

      if (path_string[ii + 1] == ' ')
      {
         token[jj++] = '#'; 
         ii++;
         space_cnt_i++;
      }
      ii++;
      if((param_cnt_i > 1) && (space_cnt_i == param_cnt_i -1))
      {
         while(ii < str_len_i)  
         {
            token[jj++] = path_string[ii];
            ii++;
         }
      }     

      *index = ii;
   }
   if (*index >= str_len_i)
   {
      token[jj] = '\0';
      return EOLN;
   }
   return MOREINPUT;
}

/***************************************************************************
* Function Name : cm_cli_convert_dir_to_dmpath
* Description   : THis API is used to convert directory into Data model tree path
* Input         : prompt_p - cli prompt
* Output        : dm_path_p - Data model template tree path
* Return value  : none
***************************************************************************/
void cm_cli_convert_dir_to_dmpath (char * prompt_p, char * dm_path_p)
{
   int32_t i, j, len_i;
   unsigned char key_b = FALSE, brace_found_b = FALSE;

   len_i = of_strlen (prompt_p);

   i = 0, j = 0;

   while (i < len_i)
   {
      if (prompt_p[i] == '#') 
      {
         key_b = TRUE;
         brace_found_b = TRUE;
         dm_path_p[j] = '{';
         j++;
         i++;
         continue;
      }
      if (prompt_p[i] == '{')
      {
         brace_found_b = TRUE;
      } 
      else if(prompt_p[i] == '}')
      {
         brace_found_b = FALSE;
      }

      if (prompt_p[i] == '/')
      {
         if (key_b == TRUE)
         {
            dm_path_p[j] = '}';
            key_b = FALSE;
            brace_found_b = FALSE;
            j++;
         }
         if ((prompt_p[i + 1] == '>') || (prompt_p[i + 1] == '\0'))
         {
            if (brace_found_b == FALSE)
            {
               dm_path_p[j] = '\0';
               break;
            }
         }
         if (brace_found_b == FALSE)
         {
            dm_path_p[j++] = '.';
            i++;
            continue;
         }
         if (brace_found_b == TRUE)
         {
            dm_path_p[j++] = prompt_p[i++];
         }
      }
      else
      {
         dm_path_p[j++] = prompt_p[i++];
      }
   }
   dm_path_p[j] = '\0';
}

/***************************************************************************
 * Function Name : cm_cli_restore_prompt
 * Description   : This callback API is used to compare a entered data element's
 *                 value, that exists in the configuration session, with the
 *                 value that exists in the Security Application database.
 * Input         : cli_session_p - Cli Session pointer
 *                 params_p - Input parameters
 * Output        : NONE
 * Return value  : OF_SUCCESS / OF_FAILURE
 ***************************************************************************/
void cm_cli_restore_prompt (void)
{
   struct cm_cli_session *tmp_cli_session_p;
   char token_a[CM_CLI_MAX_NAME_LEN + 1] = { 0 };
   char dir_path_a[CM_CLI_MAX_PROMPT_LEN + 1] = { 0 };
   int32_t index_i = 0;
   uint8_t i = 0;
   unsigned char is_table_b;

   tmp_cli_session_p = cli_session_p;

   while (cm_cli_get_token (tmp_cli_session_p->current_path, token_a, &i, 0) &&
         of_strcmp (token_a, "/>"))
   {
      of_strcpy (tmp_cli_session_p->prompt[index_i], token_a);
      index_i++;
   }

   tmp_cli_session_p->index_i = index_i;
   cm_cli_get_dir_path (dir_path_a);
   cm_cli_convert_dir_to_dmpath (dir_path_a, tmp_cli_session_p->dmpath_a);
   cm_cli_update_prompt (dir_path_a);
   cm_cli_update_child_list_and_prompt (tmp_cli_session_p->dmpath_a, dir_path_a, TRUE, &is_table_b);

}

/***************************************************************************
 * Function Name : cm_cli_validate_attribute 
 * Description   : This API is used to validate name value pairs and attribute ranges
 * Input         : nv_pair_p - Name value pairs
                   dm_path_p - Data Model Path 
 * Output        : none
 * Return value  : OF_FAILURE/OF_SUCCESS
 ***************************************************************************/
int32_t cm_cli_validate_attribute (struct cm_nv_pair * nv_pair_p, char *dm_path_p )
{
   uint32_t cnt_ui = 0;
   uint32_t str_len_ui = 0;
   uint32_t val_num_ui = 0;
   int32_t val_num_i = 0;
   char *name_p = NULL;
   char *value_p = NULL;
   unsigned char match_found_b = FALSE;
   uint32_t ip_ui = 0;
   uint32_t start_ip_ui;
   uint32_t end_ip_ui;
#ifdef CM_IPV6_SUPPORT
   struct in6_addr ip_uiv6;
   struct in6_addr start_ip_uiv6;
   struct in6_addr end_ip_uiv6;
   unsigned char bStart=FALSE;
   unsigned char bEnd=TRUE;
#endif /*CM_IPV6_SUPPORT*/
   int32_t ii = 0, len_i = 0;
   struct cm_cli_data_ele_node *data_ele_node_p;
   struct cm_cli_data_ele_node tem_data_ele_node_p;
   UCMDllQNode_t *dll_q_node_p;
   struct cm_dm_node_info *node_info_p=NULL;
   struct cm_array_of_structs *child_info_arr_p = NULL;

   if (!nv_pair_p)
   {
      cm_cli_puts (cli_session_p, " Invalid Name Value Pair.\n\r");
      return OF_FAILURE;
   }
   name_p = nv_pair_p->name_p;
   if (!name_p || of_strlen(name_p) == 0)
   {
      cm_cli_puts (cli_session_p, " Invalid input. Check the name of the field(s).\n\r");
      return OF_FAILURE;
   }
   value_p = nv_pair_p->value_p;
   if (!value_p)
   {
      cm_cli_puts (cli_session_p, " Invalid input. Check the value of the field(s).\n\r");
      return OF_FAILURE;
   }

   if(dm_path_p != NULL)
   {
      /*
         GET NODE AND CHILD INFO.. */
      if (cm_dm_get_node_and_child_info (cli_session_p->tnsprt_channel_p,
               dm_path_p,
               CM_CLI_MGMT_ENGINE_ID,
               &cli_session_p->role_info,
               &node_info_p, &child_info_arr_p) != OF_SUCCESS)
      {
#ifdef CM_CLI_DEBUG 
         sprintf (buff_a, "%s :: cm_dm_get_node_and_child_info failed\n\r", __FUNCTION__);
         cm_cli_puts (cli_session_p, buff_a);
#endif
         return OF_FAILURE;
      }
      of_memset(&tem_data_ele_node_p,0,sizeof(tem_data_ele_node_p)); 
      cm_cli_copy_data_element_info(&tem_data_ele_node_p, node_info_p);
      data_ele_node_p = &tem_data_ele_node_p;
      match_found_b = TRUE;

      cm_cli_free_node_info(node_info_p);
      cm_cli_cleanup_array_of_node_info(child_info_arr_p);
   }
   else
   {
      CM_DLLQ_SCAN (&cli_session_p->current_node_p->data_elements_list, dll_q_node_p, UCMDllQNode_t *)
      {
         data_ele_node_p = CM_DLLQ_LIST_MEMBER (dll_q_node_p, struct cm_cli_data_ele_node *,
               list_node);
         if (of_strcmp (data_ele_node_p->node_name, name_p) == 0)
         {
            match_found_b = TRUE;
            break;
         }
      }
   }

   if (match_found_b == TRUE)
   {
      match_found_b = FALSE;
      switch (data_ele_node_p->data_attrib.data_type)
      {
         case CM_DATA_TYPE_INT:
            len_i = of_strlen(value_p);
            for (ii  = 0; ii < len_i; ii++)
            {
               if(!(isdigit(value_p[ii])))
                  return OF_FAILURE;
            }
            val_num_i = of_atoi (value_p);
            //value_p = of_calloc(1,sizeof(int32_t));
            //Considering Boundary conditions for copying integer to char buffer
            //Int -2^31 to +2^31, +1 for sign and +1 for null character = 12
            value_p = of_calloc(1,12);
            if(!value_p)
            {
               UCMCLIDEBUG ("Memory allocation failed");
               return OF_FAILURE;
            }
            sprintf(value_p,"%d",val_num_i);
            of_free(nv_pair_p->value_p);
            nv_pair_p->value_p = value_p;

            break;
         case CM_DATA_TYPE_UINT:
            len_i = of_strlen(value_p);
            for (ii  = 0; ii < len_i; ii++)
            {
               if(!(isdigit(value_p[ii])))
                  return OF_FAILURE;
            }
            val_num_ui = of_atoi (value_p);
            // value_p = of_calloc(1,sizeof(uint32_t));
            //Considering Boundary conditions for copying integer to char buffer
            //Int 0 to +2^32 and +1 for null character = 11
            value_p = of_calloc(1,11);
            if(!value_p)
            {
               UCMCLIDEBUG ("Memory allocation failed");
               return OF_FAILURE;
            }           
            sprintf(value_p,"%u",val_num_ui);
            of_free(nv_pair_p->value_p);
            nv_pair_p->value_p = value_p;

            break;
         case CM_DATA_TYPE_INT64:
            len_i = of_strlen(value_p);
            for (ii  = 0; ii < len_i; ii++)
            {
               if(!(isdigit(value_p[ii])))
                  return OF_FAILURE;
            }
            val_num_i = of_atol (value_p);
            //value_p = of_calloc(1,sizeof(int32_t));
            //Considering Boundary conditions for copying integer to char buffer
            //Int -2^63 to +2^63, +1 for sign and +1 for null character = 22
            value_p = of_calloc(1,22);
            if(!value_p)
            {
               UCMCLIDEBUG ("Memory allocation failed");
               return OF_FAILURE;
            }
            sprintf(value_p,"%ld",val_num_i);
            of_free(nv_pair_p->value_p);
            nv_pair_p->value_p = value_p;

            break;
         case CM_DATA_TYPE_UINT64:
            len_i = of_strlen(value_p);
            for (ii  = 0; ii < len_i; ii++)
            {
               if(!(isdigit(value_p[ii])))
               {
                 if(!(of_isxdigit(value_p[ii])))
                   return OF_FAILURE;
	           }
            }
           /* val_num_ui = of_atol (value_p);
            // value_p = of_calloc(1,sizeof(uint32_t));
            //Considering Boundary conditions for copying integer to char buffer
            //Int 0 to +2^64 and +1 for null character = 21
            value_p = of_calloc(1,21);
            if(!value_p)
            {
               UCMCLIDEBUG ("Memory allocation failed");
               return OF_FAILURE;
            }           
            sprintf(value_p,"%lu",val_num_ui);
            of_free(nv_pair_p->value_p);
            nv_pair_p->value_p = value_p;
*/
            break;

         case CM_DATA_TYPE_STRING:
            for(ii=0; ii < of_strlen(value_p); ii++)
            { 
               if( !isalnum (value_p[ii]) &&
                     (value_p[ii] != '-') && (value_p[ii] != ',') && (value_p[ii] != ':') &&
                     (value_p[ii] != '_') && (value_p[ii] != '.') && (value_p[ii] != '=') &&
                     (value_p[ii] != '/') && (value_p[ii] != '"') && (value_p[ii] != '@') &&
                     (value_p[ii] != ',') && (value_p[ii] != ' ') && (value_p[ii] != '%'))
               {    
                  return OF_FAILURE;  /* INVALID NAME*/
               }
               //! # $ ^ & ( ) { } ; ' * ? \ | + ~ [ ] < > are notallowed
            }
            break;  
         case CM_DATA_TYPE_BOOLEAN:  
            val_num_i = of_atoi (value_p);
            if(!(( val_num_i == TRUE) || ( val_num_i == FALSE)))
            {
               sprintf (buff_a, " Invalid boolean value (%s) entered\n\r", value_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_TYPE_DATETIME:
            break;
         case CM_DATA_TYPE_IPADDR:
            if (cm_val_and_conv_aton (value_p, &ip_ui) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid IP Address(%s) entered\n\r", value_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
#ifdef CM_IPV6_SUPPORT
         case CM_DATA_TYPE_IPV6ADDR:
            if (cm_ipv6_presentation_to_num(value_p, &ip_uiv6) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid IPV6 Address(%s) entered\n\r", value_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
#endif /*CM_IPV6_SUPPORT*/

         case CM_DATA_TYPE_EXCLUDING_XMLSTRINGS:
            if (validate_string_with_out_xml_chars(value_p, of_strlen(value_p)) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid String, XML chars are not allowed \'%s\'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;  
         case CM_DATA_TYPE_STRING_SPECIAL_CHARS:
            for(ii=0; ii < of_strlen(value_p); ii++)
            { 
               if( !isalnum (value_p[ii]) &&
                     ( (value_p[ii] == '!') || (value_p[ii] == '#') || (value_p[ii] == '$') ||
                       (value_p[ii] == '&') || (value_p[ii] == '~') || (value_p[ii] == '>') ||
                       (value_p[ii] == '<')) )
               {    
                  return OF_FAILURE;  /* INVALID NAME*/
               }
               //alphanumerics - , : _ . = / " ' @ , space % ^ () {} * ? \ | + [] are allowed
               //! # $ &  ~ < > are notallowed
            }
            break;
         case CM_DATA_TYPE_STRING_PASSWORD:
            break;
         case CM_DATA_TYPE_UNKNOWN:
            break;
         default:
            cm_cli_puts (cli_session_p, " Invalid Value type.\n\r");
            return OF_FAILURE;
      }

      switch (data_ele_node_p->data_attrib.attrib_type)
      {
         case CM_DATA_ATTRIB_INT_RANGE:
            if ((val_num_i < data_ele_node_p->data_attrib.attr.int_range.start_value) ||
                  (val_num_i > data_ele_node_p->data_attrib.attr.int_range.end_value))
            {
               sprintf (buff_a, " Invalid Integer range for \'%s\'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_UINT_RANGE:
            if ((val_num_ui < data_ele_node_p->data_attrib.attr.uint_range.start_value) ||
                  (val_num_ui > data_ele_node_p->data_attrib.attr.uint_range.end_value))
            {
               sprintf (buff_a, " Invalid Integer range for \'%s\'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_INT_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.int_enum.count_ui;
                  cnt_ui++)
            {
               if (val_num_i == data_ele_node_p->data_attrib.attr.int_enum.array[cnt_ui])
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid Integer value entered for '%s'.\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_UINT_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.uint_enum.count_ui;
                  cnt_ui++)
            {
               if (val_num_ui ==
                     data_ele_node_p->data_attrib.attr.uint_enum.array[cnt_ui])
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid Integer value entered for '%s'.\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_INT64_RANGE:
            if ((val_num_i < data_ele_node_p->data_attrib.attr.longint_range.start_value) ||
                  (val_num_i > data_ele_node_p->data_attrib.attr.longint_range.end_value))
            {
               sprintf (buff_a, " Invalid Integer range for \'%s\'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_UINT64_RANGE:
            if ((val_num_ui < data_ele_node_p->data_attrib.attr.ulongint_range.start_value) ||
                  (val_num_ui > data_ele_node_p->data_attrib.attr.ulongint_range.end_value))
            {
               sprintf (buff_a, " Invalid Integer range for \'%s\'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_INT64_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.longint_enum.count_ui;
                  cnt_ui++)
            {
               if (val_num_i == data_ele_node_p->data_attrib.attr.longint_enum.array[cnt_ui])
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid Integer value entered for '%s'.\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_UINT64_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.ulongint_enum.count_ui;
                  cnt_ui++)
            {
               if (val_num_ui ==
                     data_ele_node_p->data_attrib.attr.ulongint_enum.array[cnt_ui])
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid Integer value entered for '%s'.\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_STR_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.string_enum.count_ui;
                  cnt_ui++)
            {
               if (!strcmp
                     (value_p,
                      data_ele_node_p->data_attrib.attr.string_enum.array[cnt_ui]))
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid string value entered for '%s'.\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;

         case CM_DATA_ATTRIB_STR_RANGE:
            str_len_ui = of_strlen (value_p);
            if ((str_len_ui < data_ele_node_p->data_attrib.attr.string_range.min_length) ||
                  (str_len_ui > data_ele_node_p->data_attrib.attr.string_range.max_length))
            {
               sprintf (buff_a,
                     " Invalid string length entered. (Minimum Length=%d; Maximum Length=%d)\n\r",
                     data_ele_node_p->data_attrib.attr.string_range.min_length,
                     data_ele_node_p->data_attrib.attr.string_range.max_length);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_NONE:
            UCMCLIDEBUG ("Attribute is None: Ignoring");
            break;
         case CM_DATA_ATTRIB_IPADDR_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.ip_enum.count_ui;
                  cnt_ui++)
            {
               if (!strcmp
                     (value_p, data_ele_node_p->data_attrib.attr.ip_enum.ip_arr_a[cnt_ui]))
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid IP Address entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_IPADDR_RANGE:
            if (cm_val_and_conv_aton
                  (data_ele_node_p->data_attrib.attr.ip_range.start_ip,
                   &start_ip_ui) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid Start IP Address value entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            if (cm_val_and_conv_aton
                  (data_ele_node_p->data_attrib.attr.ip_range.end_ip,
                   &end_ip_ui) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid End IP Address value entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }

            if ((ip_ui < start_ip_ui) || (ip_ui > end_ip_ui))
            {
               sprintf (buff_a, " Invalid IP Address range entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
#ifdef CM_IPV6_SUPPORT

         case CM_DATA_ATTRIB_IPV6ADDR_ENUM:
            for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.ipv6_enum.count_ui;
                  cnt_ui++)
            {
               if (!strcmp
                     (value_p, data_ele_node_p->data_attrib.attr.ipv6_enum.ip_v6_arr_a[cnt_ui]))
               {
                  match_found_b = TRUE;
               }
            }
            if (match_found_b == FALSE)
            {
               sprintf (buff_a, " Invalid IP Address entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;
         case CM_DATA_ATTRIB_IPV6ADDR_RANGE:
            if (cm_ipv6_presentation_to_num
                  (data_ele_node_p->data_attrib.attr.ipv6_range.start_ip,
                   &start_ip_uiv6) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid Start IP Address value entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            if (cm_ipv6_presentation_to_num
                  (data_ele_node_p->data_attrib.attr.ipv6_range.end_ip,
                   &end_ip_uiv6) != OF_SUCCESS)
            {
               sprintf (buff_a, " Invalid End IP Address value entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            for(ii=0; ii< CM_IPV6_ADDR_LEN ; ii++)
            {
               if(ip_uiv6.s6_addr[ii] < start_ip_uiv6.s6_addr[ii] && bStart == FALSE)
               {
                  bStart = TRUE;
                  if(bEnd == TRUE)
                     break;
               }

               if(ip_uiv6.s6_addr[ii] > start_ip_uiv6.s6_addr[ii] && bEnd == FALSE)
               {
                  bEnd = TRUE;
                  if(bStart == TRUE)
                     break;
               }
            }
            if (bStart == FALSE || bEnd == FALSE)
            {
               sprintf (buff_a, " Invalid IP Address range entered for '%s'\n\r", name_p);
               cm_cli_puts (cli_session_p, buff_a);
               return OF_FAILURE;
            }
            break;

#endif /*CM_IPV6_SUPPORT*/

         default:
            cm_cli_puts (cli_session_p, " Unknown type: default");
            break;
      }
   }
   else
   {
      sprintf (buff_a, " No Match found for (%s)\n\r", name_p);
      cm_cli_puts (cli_session_p, buff_a);
      return OF_FAILURE;
   }
   return OF_SUCCESS;
}

/***************************************************************************
 * Function Name :cm_cli_display_attrib_info 
 * Description   :This APi is used to display attributes info as help
 * Input         : data_ele_node_p - Data model element node info
                   char  - Buffer info 
 * Output        : none
 * Return value  : OF_SUCCESS/OF_FAILURE
 ***************************************************************************/
int32_t cm_cli_display_attrib_info (struct cm_cli_data_ele_node * data_ele_node_p,char (*tmp_sort_buff)[512],int32_t tmp_attrib_cnt_i)
{
   uint32_t cnt_ui = 0;

   memset(buff_a, 0, sizeof(buff_a));
   sprintf (buff_a, "\t: %s", aTypeStr[data_ele_node_p->data_attrib.data_type]);
   of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
   memset(buff_a, 0, sizeof(buff_a));
   switch (data_ele_node_p->data_attrib.attrib_type)
   {
      case CM_DATA_ATTRIB_NONE:
         break;
      case CM_DATA_ATTRIB_INT_RANGE:
         sprintf (buff_a, " [ %d to %d ]",
               data_ele_node_p->data_attrib.attr.int_range.start_value,
               data_ele_node_p->data_attrib.attr.int_range.end_value);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         break;
      case CM_DATA_ATTRIB_UINT_RANGE:
         sprintf (buff_a, " [ %d to %d ]",
               data_ele_node_p->data_attrib.attr.uint_range.start_value,
               data_ele_node_p->data_attrib.attr.uint_range.end_value);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         break;
      case CM_DATA_ATTRIB_INT_ENUM:
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," { ");
         for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.int_enum.count_ui;
               cnt_ui++)
         {
            sprintf (buff_a, "%d",
                  data_ele_node_p->data_attrib.attr.int_enum.array[cnt_ui]);
            if(cnt_ui > 0)
               of_strcat(tmp_sort_buff[tmp_attrib_cnt_i], CM_CLI_ENUM_SEPERATOR);
            of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         }
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," } ");
         break;
      case CM_DATA_ATTRIB_UINT_ENUM:
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," { ");

         for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.uint_enum.count_ui;
               cnt_ui++)
         {
            sprintf (buff_a, "%u",
                  data_ele_node_p->data_attrib.attr.uint_enum.array[cnt_ui]);
            if(cnt_ui > 0)
               of_strcat(tmp_sort_buff[tmp_attrib_cnt_i], CM_CLI_ENUM_SEPERATOR);
            of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         }
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," } ");
         break;
      case CM_DATA_ATTRIB_INT64_RANGE:
         sprintf (buff_a, " [ %ld to %ld ]",
               data_ele_node_p->data_attrib.attr.longint_range.start_value,
               data_ele_node_p->data_attrib.attr.longint_range.end_value);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         break;
      case CM_DATA_ATTRIB_UINT64_RANGE:
         sprintf (buff_a, " [ %lu to %lu ]",
               data_ele_node_p->data_attrib.attr.ulongint_range.start_value,
               data_ele_node_p->data_attrib.attr.ulongint_range.end_value);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         break;
      case CM_DATA_ATTRIB_INT64_ENUM:
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," { ");
         for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.longint_enum.count_ui;
               cnt_ui++)
         {
            sprintf (buff_a, "%ld",
                  data_ele_node_p->data_attrib.attr.longint_enum.array[cnt_ui]);
            if(cnt_ui > 0)
               of_strcat(tmp_sort_buff[tmp_attrib_cnt_i], CM_CLI_ENUM_SEPERATOR);
            of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         }
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," } ");
         break;
      case CM_DATA_ATTRIB_UINT64_ENUM:
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," { ");

         for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.ulongint_enum.count_ui;
               cnt_ui++)
         {
            sprintf (buff_a, "%lu",
                  data_ele_node_p->data_attrib.attr.ulongint_enum.array[cnt_ui]);
            if(cnt_ui > 0)
               of_strcat(tmp_sort_buff[tmp_attrib_cnt_i], CM_CLI_ENUM_SEPERATOR);
            of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         }
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," } ");
         break;
      case CM_DATA_ATTRIB_STR_ENUM:
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," { ");
         for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.string_enum.count_ui;
               cnt_ui++)
         {
            sprintf (buff_a, "%s",
                  data_ele_node_p->data_attrib.attr.string_enum.array[cnt_ui]);
            if(cnt_ui > 0)
               of_strcat(tmp_sort_buff[tmp_attrib_cnt_i], CM_CLI_ENUM_SEPERATOR);
            of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         }
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," } ");
         break;

      case CM_DATA_ATTRIB_STR_RANGE:
         sprintf (buff_a, " [ %d to %d ]",
               data_ele_node_p->data_attrib.attr.string_range.min_length,
               data_ele_node_p->data_attrib.attr.string_range.max_length);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         break;
      case CM_DATA_ATTRIB_IPADDR_ENUM:
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," { ");
         for (cnt_ui = 0; cnt_ui < data_ele_node_p->data_attrib.attr.ip_enum.count_ui;
               cnt_ui++)
         {
            sprintf (buff_a, "%s",
                  data_ele_node_p->data_attrib.attr.ip_enum.ip_arr_a[cnt_ui]);
            if(cnt_ui > 0)
               of_strcat(tmp_sort_buff[tmp_attrib_cnt_i], CM_CLI_ENUM_SEPERATOR);
            of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         }
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i]," } ");
         break;
      case CM_DATA_ATTRIB_IPADDR_RANGE:
         sprintf (buff_a, " StartIP :: (%s) ",
               data_ele_node_p->data_attrib.attr.ip_range.start_ip);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         sprintf (buff_a, " EndIP :: (%s)",
               data_ele_node_p->data_attrib.attr.ip_range.end_ip);
         of_strcat(tmp_sort_buff[tmp_attrib_cnt_i],buff_a);
         break;
      default:
         cm_cli_puts (cli_session_p, " Unknown type: default");
         break;
   }
   // cm_cli_puts (cli_session_p, "\n\t");
   return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_cli_free_dm_node_info 
 * Description   : THis APi is used to free allocated memory
 * Input         : node_info_p - Data model node info
 * Output        : none
 * Return value  :  none
 ***************************************************************************/
void cm_cli_free_dm_node_info (struct cm_dm_node_info * node_info_p,
                             unsigned char free_node_info_b)
{
   if (node_info_p->name_p)
      of_free (node_info_p->name_p);
   if (node_info_p->friendly_name_p)
      of_free (node_info_p->friendly_name_p);
   if (node_info_p->description_p)
      of_free (node_info_p->description_p);
   if (node_info_p->dm_path_p)
      of_free (node_info_p->dm_path_p);
   if(node_info_p->element_attrib.command_name_p)
      of_free(node_info_p->element_attrib.command_name_p);
   if(node_info_p->element_attrib.cli_identifier_p)
      of_free(node_info_p->element_attrib.cli_identifier_p);
   if(node_info_p->element_attrib.cli_idenetifier_help_p)
      of_free(node_info_p->element_attrib.cli_idenetifier_help_p);
   if (free_node_info_b)
      of_free (node_info_p);
}

/***************************************************************************
 * Function Name : cm_cli_free_array_of_node_info 
 * Description   : This API is used to free array of data model tree nodes
 * Input         : array_of_node_info_p - Array of nodes
 * Output        : none
 * Return value  :  none
 ***************************************************************************/
 void cm_cli_free_array_of_node_info (struct cm_array_of_structs *
                                             array_of_node_info_p)
{
   int32_t ii;

   for (ii = 0; ii < array_of_node_info_p->count_ui; ii++)
   {
      cm_cli_free_dm_node_info ((struct cm_dm_node_info *) array_of_node_info_p->struct_arr_p +
            ii, FALSE);
   }
   of_free (array_of_node_info_p->struct_arr_p);
   of_free (array_of_node_info_p);
}

/***************************************************************************
 * Function Name : cm_cli_free_instance_map_list 
 * Description   : THis API is used to free allocated memory
 * Input         : map_p - Instance tree map 
 * Output        : none
 * Return value  : none
 ***************************************************************************/
void cm_cli_free_instance_map_list (struct cm_dm_instance_map * map_p, int32_t count_ui)
{
   int32_t ii;

   for (ii = 0; ii < count_ui; ii++)
   {
      of_free (map_p[ii].key_p);
   }

   of_free (map_p);
}

/***************************************************************************
 * Function Name : cm_cli_display_config_session 
 * Description   : THis API is used to display sesssion information
 * Input         : cmd_array_p - List of commands in the current session
 * Output        : none
 * Return value  : none
 ***************************************************************************/
void cm_cli_display_config_session (struct cm_array_of_commands * cmd_array_p)
{
   int32_t ii;
   struct cm_command *tmp_cmd_p;
   char str_a[CM_CLI_MAX_NAME_LEN + 1] = { 0 };

   for (ii = 0; ii < cmd_array_p->count_ui; ii++)
   {
      tmp_cmd_p = &cmd_array_p->Commands[ii];

      cm_cli_get_command_string(tmp_cmd_p->command_id, str_a);
      sprintf(buff_a, " Command : %s\t", str_a);
      cm_cli_puts(cli_session_p, buff_a);

      /* Get CommandID */
      sprintf (buff_a, " DMPath :: %s\n\t", tmp_cmd_p->dm_path_p);
      cm_cli_puts(cli_session_p, buff_a);
      cm_cli_display_nv_pair (tmp_cmd_p->nv_pair_array.count_ui, 
            &tmp_cmd_p->nv_pair_array,tmp_cmd_p->dm_path_p);
   }
}

 /***************************************************************************
 * Function Name : cm_cli_get_command_string 
 * Description   : THis API is used to get command string in end user understandable format
 * Input         : cmd_id_ui - Command id 
 * Output        : str_p - command string
 * Return value  :  OF_FAILURE/OF_SUCCESS
 ***************************************************************************/
 int32_t cm_cli_get_command_string(uint32_t cmd_id_ui, char *str_p)
{

   switch(cmd_id_ui)   
   {
      case  CM_CMD_ADD_PARAMS:
         of_strcpy(str_p, "CM_CMD_ADD_PARAMS");
         break;
      case CM_CMD_SET_PARAMS:
         of_strcpy(str_p, "CM_CMD_SET_PARAMS");
         break;

      case CM_CMD_DEL_PARAMS:
         of_strcpy(str_p, "CM_CMD_DEL_PARAMS");
         break;
   }
   return OF_SUCCESS;
}

/***************************************************************************
 * *  Function Name: CliVerifyIntFormat
 * *  Description  : This function verifies for the validity of the integer
 * *                 value entered.
 * *  Input        : char* -- A pointer to the int value to be verified  
 * *  Output       : int32_t* -- Interger value if it is a valid one 
 * *  Return Value : int32_t -- OF_SUCCESS if valid integer value is
 * entered 
 * *                 otherwise OF_FAILURE
 * ***************************************************************************/
 int32_t cm_cli_verify_int_format (const char * intaddr, int32_t * int_val)
{
   char * tmp_addr_p = NULL;
   int32_t val_i, len_i = 0, i = 0;

   len_i = of_strlen(intaddr);
   for (i  = 0; i < len_i; i++)
   {
      if(!(isdigit(intaddr[i])))
         return OF_FAILURE;
   }
   if(of_strlen (intaddr) == 0)
      return OF_FAILURE;
   val_i = of_strtol (intaddr, &tmp_addr_p, 0);
   if(*tmp_addr_p == '\0')
   {
      *int_val = val_i;
      return OF_SUCCESS;
   }

   else
      return OF_FAILURE;
}

/******************************************************************************
 * Function Name : cm_cli_user_login
 * Input params  : NONE
 * Output params : NONE
 * Description   : This API is used to read user name, password and authenticate user login process
 * Return value  : OF_SUCCESS / OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_user_login(unsigned char *user_name_p, unsigned char *password_p)
{
   unsigned char user_name_a[CM_MAX_ADMINNAME_LEN + 1];
   char passwd_a[CM_MAX_ADMINNAME_LEN + 1];
   struct cm_role_info* role_info_p = NULL;

   of_memset(user_name_a, 0, sizeof(user_name_a));
   of_memset(passwd_a, 0, sizeof(passwd_a));

#ifdef OF_CM_AUTH_PLUGIN_SUPPORT
   struct cm_auth_mgmt auth;
#endif /*OF_CM_AUTH_PLUGIN_SUPPORT*/

   if(cm_cli_get_user_name(user_name_a)!= OF_SUCCESS)
   {
      cm_cli_puts(cli_session_p, " Get user name failed.\n\r");
      return OF_FAILURE;
   }

   if(cm_cli_get_password(passwd_a)!= OF_SUCCESS)
   {
      cm_cli_puts(cli_session_p, " Get password failed.\n\r");
      return OF_FAILURE;
   }


#ifdef OF_CM_AUTH_PLUGIN_SUPPORT
   UCMCLIDEBUG("\n Using Plugin Authentication Support.\r\n");
   of_memset(&auth,0,sizeof(struct cm_auth_mgmt));
   auth.pUsrName = user_name_a;
   auth.pPwd = passwd_a;
   auth.pNewPwd = NULL;
   auth.role_info_p = role_info_p;
   if(cm_user_mgmt(CM_AUTH_USR_AUTH_CMD, &auth) != OF_SUCCESS)
   {
      cm_cli_puts(cli_session_p, "Authentication failed! Wrong User Name/Password.\n\r");
      if(role_info_p != NULL)
      {
         of_free(role_info_p);
         role_info_p = NULL;
      }
      return OF_FAILURE;
   }
   role_info_p = auth.role_info_p;
#else
   if((cm_pam_authenticate_user(user_name_a, passwd_a, &role_info_p)) != OF_SUCCESS)
   { 
      cm_cli_puts(cli_session_p, "Authentication failed! Wrong User Name/Password.\n\r");
      if(role_info_p != NULL)
      {
         of_free(role_info_p);
         role_info_p = NULL;
      }
      return OF_FAILURE;
   }
#endif /*OF_CM_AUTH_PLUGIN_SUPPORT*/

   of_memcpy(&cli_session_p->role_info, role_info_p, sizeof(struct cm_role_info));
   of_strcpy(cli_session_p->role_info.admin_name, user_name_a);
   if(role_info_p != NULL)
   {
      of_free(role_info_p);
      role_info_p = NULL;
   }
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_get_user_name
 * Input params  : NONE
 * Output params : user_name_p
 * Description   : THis API is used to read user name from command prompt
 * Return value  : OF_SUCCESS?OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_get_user_name(unsigned char* user_name_p)
{
   char *p, *q = user_name_p;

   cm_cli_puts(cli_session_p, "\n\rUser Name: ");
   if(!of_fgets(user_name_p, CM_MAX_ADMINNAME_LEN, CM_CLI_STDIN))
   {
      return OF_FAILURE;
   }

   p = of_strchr(user_name_p, '\n');
   if ( p !=  NULL)
   {
      user_name_p[p-q] = '\0';
   }
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_get_password
 * Input params  : NONE
 * Output params : password_p 
 * Description   : This API is used to read password from command prompt
 * Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_get_password(unsigned char* password_p)
{
   of_strncpy(password_p, getpass("Password: "), CM_MAX_ADMINNAME_LEN);
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_get_server_ip 
 * Input params  : NONE
 * Output params : server_ip_p
 * Description   : This API is used to read device uint32_tess from command prompt
 * Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_get_server_ip(uint32_t *server_ip_p)
{
   unsigned char ip_addr_a[CM_MAX_ADMINNAME_LEN + 1]={0};
   uint32_t ip_address=0;
   char *p, *q;
   cm_cli_puts(cli_session_p, "\n\rDevice IP Address: ");
   if(!of_fgets(ip_addr_a, CM_MAX_ADMINNAME_LEN, CM_CLI_STDIN))
   {
      //return NULL;
      return OF_FAILURE;
   }

   q=&ip_addr_a[0];
   p = of_strchr(ip_addr_a, '\n');
   if ( p !=  NULL)
   {
      ip_addr_a[p-q] = '\0';
   }

   if(cm_val_and_conv_aton(ip_addr_a, &ip_address) == OF_FAILURE )
   {
      if(!of_strcmp(ip_addr_a,""))
      {
         ip_address=0;
      }
      else
      {
         cm_cli_puts(cli_session_p, "\n\rInvalid IP Address\r\n");
         return OF_FAILURE;
      }
   }

   *server_ip_p=ip_address;
   return OF_SUCCESS;
}

/******************************************************************************
 * Function Name : cm_cli_prepare_keys_array_from_token 
 * Input params  : dm_path_p - Data model tree path
                   key_p - key token
 * Output params : out_nv_array_p - Name value pairs
 * Description   : This API is used to prepare instance path using array of keys from array of keys
 * Return value  : OF_SUCCESS/OF_FAILURE
 *****************************************************************************/
int32_t cm_cli_prepare_keys_array_from_token(char *dm_path_p, char *key_p, struct cm_array_of_nv_pair **out_nv_array_p)
{  
   uint32_t cnt_ui=0, key_cnt_ui=0, nv_cnt_ui=0, m, key_index_ui, jj;
   char *key_name_value_p[CM_DM_MAX_KEY_INSTANCES], *key_name_p=NULL;
   unsigned char multiple_keys_b=FALSE, child_in_keys_found_b=FALSE;
   struct cm_dm_node_info *child_node_p = NULL;
   struct cm_array_of_structs *child_info_arr_p = NULL;
   int32_t key_node_i;
   int32_t return_value;
   struct cm_array_of_nv_pair *key_arry_of_nv_pair_p;
   char *q;

   if((!key_p) || (!dm_path_p))
   {
      return OF_FAILURE;
   }

   while ((key_p[cnt_ui] != '}') && (key_p[cnt_ui] != '\0'))
   {
      cnt_ui++;
      if(key_p[cnt_ui] == ',')
      {
         key_cnt_ui++;
      }
   }

   key_cnt_ui++;
   if(key_cnt_ui > 1)
   {
      multiple_keys_b=TRUE;
   }

   return_value=cm_dm_get_key_childs (cli_session_p->tnsprt_channel_p, dm_path_p,
         CM_CLI_MGMT_ENGINE_ID, &cli_session_p->role_info, &child_info_arr_p);
   if( (return_value == OF_FAILURE) || (child_info_arr_p == NULL) )
   {
      cm_cli_puts(cli_session_p,"Get key childs failed\r\n");  
      return OF_FAILURE;
   }

   /* Check for User input key count here itself,return from here if not match*/
   if(child_info_arr_p->count_ui != key_cnt_ui)
   {
      cm_cli_puts(cli_session_p," Key count does not match with application.\r\n");    
      return OF_FAILURE;
   }

   key_arry_of_nv_pair_p=(struct cm_array_of_nv_pair *)of_calloc(1,sizeof(struct cm_array_of_nv_pair));
   if(!key_arry_of_nv_pair_p)
   {
      cm_cli_puts(cli_session_p,"Memory Alloc failure"); /*CID 360*/
      return OF_FAILURE;
   }

   key_arry_of_nv_pair_p->count_ui=key_cnt_ui;
   key_arry_of_nv_pair_p->nv_pairs=(struct cm_nv_pair *) of_calloc(key_arry_of_nv_pair_p->count_ui,
         sizeof(struct cm_nv_pair));
   if(!key_arry_of_nv_pair_p->nv_pairs)
   {
      cm_cli_puts (cli_session_p, "Calloc Failed\n\r");
      return OF_FAILURE;
   }

   if(multiple_keys_b)
   {
      for(cnt_ui = 0; cnt_ui< CM_DM_MAX_KEY_INSTANCES; cnt_ui++)
      {
         key_name_value_p[cnt_ui] = (char*)of_calloc(1, of_strlen(key_p));
         if(key_name_value_p[cnt_ui]== NULL)
         {
            cm_cli_puts(cli_session_p,"Memory allocation failed\r\n");
            for(jj = 0; jj<cnt_ui; jj++)
            {
               of_free(key_name_value_p[jj]);
            }
            of_free(key_arry_of_nv_pair_p->nv_pairs); /*CID 360*/
            of_free(key_arry_of_nv_pair_p);
            return OF_FAILURE;
         }
      }
      for (m=0, key_index_ui=0; m < key_cnt_ui; m++, key_index_ui+=cnt_ui, key_index_ui++)
      {
         for (cnt_ui = 0, jj = 0;
               ((key_p[key_index_ui+cnt_ui] != ',') && (key_p[key_index_ui+cnt_ui] != '}') && (key_p[key_index_ui+cnt_ui] != '\0')); 
               cnt_ui++, jj++)
         {
            key_name_value_p[m][jj] = key_p[key_index_ui+cnt_ui];
         }
      }
   }
   else
   {
      q=strstr(key_p,"}");
      if (q != NULL)
         *q='\0';
   }

   for (cnt_ui = 0; cnt_ui < child_info_arr_p->count_ui; cnt_ui++)
   {
      child_node_p=(struct cm_dm_node_info *)(child_info_arr_p->struct_arr_p) + cnt_ui;
      if (child_node_p->element_attrib.key_b == TRUE)
      {
         child_in_keys_found_b=FALSE;
         for(m=0; m< key_cnt_ui; m++)
         {
            if(multiple_keys_b)
            {
               key_name_p=of_strtok(key_name_value_p[m],"=");
               key_p=of_strtok(NULL,",");//There can be "=" exists in the key value (ex:ocsp - subname record), hence changed "=" to "," as "," is used as token seperator and this cannot be present in the key value.

               //key_p=of_strtok(NULL,"=");
               if (key_name_p)
               {
                  key_node_i=(!of_strcmp(child_node_p->name_p, key_name_p));
                  key_name_value_p[m][of_strlen(key_name_p)]='=';
               }
               else
               {
                  key_node_i=FALSE;
               }
            }
            else
            {
               /* Single Key... No need to give Key Name
                * Hence assuming success */
               key_node_i=TRUE;
            }

            if(key_node_i)
            {
               child_in_keys_found_b=TRUE;
               key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_length = of_strlen (child_node_p->name_p);
               key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_p =
                  (char *) of_calloc (1, key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_length + 1);
               if (key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_p == NULL)
               {
                  cm_cli_puts (cli_session_p, "Calloc Failed\n\r");
                  if (child_info_arr_p)
                  {
                     cm_cli_free_array_of_node_info (child_info_arr_p);
                  }
                  key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_length = 0;
                  CM_FREE_PTR_NVPAIR_ARRAY(key_arry_of_nv_pair_p , key_arry_of_nv_pair_p->count_ui);
                  if(multiple_keys_b)
                  {
                     for(cnt_ui = 0; cnt_ui< CM_DM_MAX_KEY_INSTANCES; cnt_ui++)
                        of_free(key_name_value_p[cnt_ui]);
                  }
                  return OF_FAILURE;
               }
               of_strncpy (key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_p,
                     child_node_p->name_p, key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].name_length);
               key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_type = child_node_p->data_attrib.data_type;
               if(key_p == NULL)
               {
                  sprintf(buff_a, " Value not entered for key %s\r\n", child_node_p->name_p);
                  cm_cli_puts(cli_session_p,buff_a);
                  return OF_FAILURE;
               }
               key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_length = of_strlen ((char *) key_p);
               key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_p = (char *) of_calloc
                  (1, key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_length + 1);
               if (key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_p == NULL)
               {
                  cm_cli_puts (cli_session_p, "Calloc Failed\n\r");
                  if (child_info_arr_p)
                  {
                     cm_cli_free_array_of_node_info (child_info_arr_p);
                  }
                  key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_length = 0;
                  CM_FREE_PTR_NVPAIR_ARRAY(key_arry_of_nv_pair_p, key_arry_of_nv_pair_p->count_ui);
                  if(multiple_keys_b)
                  {
                     for(cnt_ui = 0; cnt_ui< CM_DM_MAX_KEY_INSTANCES; cnt_ui++)
                        of_free(key_name_value_p[cnt_ui]);
                  }
                  return OF_FAILURE;
               }
               of_strncpy (key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_p, (char *) key_p,
                     key_arry_of_nv_pair_p->nv_pairs[nv_cnt_ui].value_length);
               nv_cnt_ui++;
               break;
            }

         }
         if(child_in_keys_found_b == FALSE)
         {
            cm_cli_puts(cli_session_p,"Key Child not entered\r\n");

            if (child_info_arr_p)
            {
               cm_cli_free_array_of_node_info (child_info_arr_p);
            }
            CM_FREE_PTR_NVPAIR_ARRAY(key_arry_of_nv_pair_p, key_arry_of_nv_pair_p->count_ui);
            if(multiple_keys_b)
            {
               for(cnt_ui = 0; cnt_ui< CM_DM_MAX_KEY_INSTANCES; cnt_ui++)
                  of_free(key_name_value_p[cnt_ui]);
            }
            return OF_FAILURE;
         }
      }
   }
   if (child_info_arr_p)
   {
      cm_cli_free_array_of_node_info (child_info_arr_p);
   }
   if(multiple_keys_b)
   {
      for(cnt_ui = 0; cnt_ui< CM_DM_MAX_KEY_INSTANCES; cnt_ui++)
         of_free(key_name_value_p[cnt_ui]);
   }

   if(key_cnt_ui != nv_cnt_ui)
   {
      cm_cli_puts(cli_session_p,"Some input is not a Key");
      if(key_arry_of_nv_pair_p)
         CM_FREE_PTR_NVPAIR_ARRAY(key_arry_of_nv_pair_p, key_arry_of_nv_pair_p->count_ui);
      return OF_FAILURE;
   }
   *out_nv_array_p=key_arry_of_nv_pair_p;
   return OF_SUCCESS;
}
/**************************************************************************
 * Function Name : cm_cli_cleanup_array_of_node_info
 * Input Args    : arr_node_info_structs_p -Pointer to Array of NOde Info structures
 * Output Args   : None
 * Description   : This API used to Free the Array of NodeInfo structure and its members
 * Return Values : None
 * *************************************************************************/
void cm_cli_cleanup_array_of_node_info (struct cm_array_of_structs * arr_node_info_structs_p)
{
   int32_t i, cnt_i;
   struct cm_dm_node_info *node_info_p = NULL;

   if (arr_node_info_structs_p)
   {
      if (arr_node_info_structs_p->struct_arr_p)
      {
         node_info_p = arr_node_info_structs_p->struct_arr_p;
         for (i = 0; i < arr_node_info_structs_p->count_ui; i++)
         {
            if (node_info_p[i].name_p)
               of_free (node_info_p[i].name_p);

            if (node_info_p[i].friendly_name_p)
               of_free (node_info_p[i].friendly_name_p);

            if (node_info_p[i].description_p)
               of_free (node_info_p[i].description_p);

            if (node_info_p[i].dm_path_p)
               of_free (node_info_p[i].dm_path_p);
            /* Atmaram - Memory Leak - Friendly names, command name, cli identifier and enum array not get freed.
             * Huge memory loss.
             * Now Fixed
             */
            if (node_info_p[i].element_attrib.command_name_p)
            {
               of_free(node_info_p[i].element_attrib.command_name_p);
               node_info_p[i].element_attrib.command_name_p=NULL;
            }
            if (node_info_p[i].element_attrib.cli_identifier_p)
            {
               of_free(node_info_p[i].element_attrib.cli_identifier_p);
               node_info_p[i].element_attrib.cli_identifier_p=NULL;
            }
            if (node_info_p[i].element_attrib.cli_idenetifier_help_p)
            {
               of_free(node_info_p[i].element_attrib.cli_idenetifier_help_p);
               node_info_p[i].element_attrib.cli_idenetifier_help_p=NULL;
            }

            if((node_info_p[i].data_attrib.data_type == CM_DATA_TYPE_STRING || 
                     node_info_p[i].data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS) && 
                  node_info_p[i].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
            {
               for( cnt_i =0 ;cnt_i<node_info_p[i].data_attrib.attr.string_enum.count_ui;cnt_i++)
               {
                  if (of_strlen(node_info_p[i].data_attrib.attr.string_enum.array[cnt_i])> 0 )
                     of_free(node_info_p[i].data_attrib.attr.string_enum.array[cnt_i]);
               }
            }
            if((node_info_p[i].data_attrib.data_type == CM_DATA_TYPE_STRING || 
                     node_info_p[i].data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)&& 
                  node_info_p[i].data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
            {
               for( cnt_i =0 ;cnt_i<  node_info_p[i].data_attrib.attr.string_enum.count_ui;cnt_i++)
               {
                  if (of_strlen(node_info_p[i].data_attrib.attr.string_enum.aFrdArr[cnt_i])>0)
                     of_free(node_info_p[i].data_attrib.attr.string_enum.aFrdArr[cnt_i]);
               }
            }

         }
         of_free (arr_node_info_structs_p->struct_arr_p);
      }
      of_free (arr_node_info_structs_p);
   }
}

/**************************************************************************
 * Function Name : UCMCli_FreeNodeInfo
 * Input Args    : node_info_p - Pointer to NodeInfo structure
 * Output Args   : None
 * Description   : This API is used to free NodeInfo structure
 * Return Values : None
 * *************************************************************************/
void cm_cli_free_node_info (struct cm_dm_node_info * node_info_p)
{
   int32_t cnt_i;

   if (node_info_p)
   {
      if (node_info_p->name_p)
         of_free (node_info_p->name_p);

      if (node_info_p->friendly_name_p)
         of_free (node_info_p->friendly_name_p);

      if (node_info_p->description_p)
         of_free (node_info_p->description_p);

      if (node_info_p->dm_path_p)
         of_free (node_info_p->dm_path_p);

      /* Atmaram - Memory Leak - Friendly names, command name, cli identifier and enum array not get freed.
       * Huge memory loss
       * Fixed
       */
      if(node_info_p->element_attrib.command_name_p)
         of_free(node_info_p->element_attrib.command_name_p);

      if(node_info_p->element_attrib.cli_identifier_p)
         of_free(node_info_p->element_attrib.cli_identifier_p);

      if(node_info_p->element_attrib.cli_idenetifier_help_p)
         of_free(node_info_p->element_attrib.cli_idenetifier_help_p);

      if((node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING || 
               node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS) && 
            node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
      {
         for( cnt_i =0 ;cnt_i<node_info_p->data_attrib.attr.string_enum.count_ui;cnt_i++)
         {
            if (of_strlen(node_info_p->data_attrib.attr.string_enum.array[cnt_i])> 0 )
               of_free(node_info_p->data_attrib.attr.string_enum.array[cnt_i]);
         }
      }
      if((node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING || 
               node_info_p->data_attrib.data_type == CM_DATA_TYPE_STRING_SPECIAL_CHARS)&& 
            node_info_p->data_attrib.attrib_type == CM_DATA_ATTRIB_STR_ENUM)
      {
         for( cnt_i =0 ;cnt_i<  node_info_p->data_attrib.attr.string_enum.count_ui;cnt_i++)
         {
            if( of_strlen(node_info_p->data_attrib.attr.string_enum.aFrdArr[cnt_i]) > 0)
               of_free(node_info_p->data_attrib.attr.string_enum.aFrdArr[cnt_i]);
         }
      }

      of_free (node_info_p);
   }
}


#endif /* CM_CLIENG_SUPPORT */
