/**
 * Copyright (c) 2012, 2013  Freescale.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

/* File  :      crm_attribute.c
 * Author:      Varun Kumar Reddy<b36461@freescale.com>
 * Description: 
 */

#include "crm_system_dependencies.h"
#include "crm_tenant_api.h"

extern void *crm_attributes_mempool_g;

int32_t crm_add_attribute(of_list_t *attributes_p, struct crm_attribute_name_value_pair* attribute_info_p)
{
  struct  crm_resource_attribute_entry *attribute_entry_p=NULL,*attribute_entry_scan_p=NULL;
  uint8_t heap_b = 0;
  int32_t retval;


  OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "entered");
  if(attribute_info_p == NULL)
    return CRM_FAILURE;

  if(attribute_info_p->name_string == NULL)
    return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(attribute_info_p->value_string == NULL)
    return CRM_ERROR_ATTRIBUTE_VALUE_NULL;

  if(strlen(attribute_info_p->name_string) > CRM_MAX_ATTRIBUTE_NAME_LEN)
    return CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;

  if(strlen(attribute_info_p->value_string) > CRM_MAX_ATTRIBUTE_VALUE_LEN)
    return CRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
  
  OF_LIST_SCAN(*attributes_p, attribute_entry_scan_p, struct crm_resource_attribute_entry*, CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
  {
    if(!strcmp(attribute_entry_scan_p->name, attribute_info_p->name_string))
    {
      OF_LOG_MSG(OF_LOG_CRM, OF_LOG_DEBUG, "Modifying attribute value");
      strcpy(attribute_entry_scan_p->value, attribute_info_p->value_string);
      return CRM_SUCCESS;   
    }
  }
  retval = mempool_get_mem_block(crm_attributes_mempool_g,(uchar8_t**)&attribute_entry_p,&heap_b);
  if(retval != MEMPOOL_SUCCESS)
    return  CRM_ERROR_NO_MEM;

  attribute_entry_p->heap_b = heap_b;
  strcpy(attribute_entry_p->name, attribute_info_p->name_string);
  strcpy(attribute_entry_p->value, attribute_info_p->value_string);

  /* Add attribute to the list of  attributes */
  OF_APPEND_NODE_TO_LIST((*attributes_p), attribute_entry_p, CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET);
  return CRM_SUCCESS;
}
/*******************************************************************************************************/
int32_t crm_get_first_attribute( of_list_t *attributes_p,
                                    struct crm_attribute_name_value_output_info  *attribute_output_p)
{ 
  struct   crm_resource_attribute_entry       *attribute_entry_p=NULL;
  uint8_t  entry_found_b = 0;

  if(attributes_p == NULL)
    return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  OF_LIST_SCAN(*attributes_p, attribute_entry_p, struct crm_resource_attribute_entry*, CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
   {
         entry_found_b = 1;
         break;
   }

   if(entry_found_b == 0 )
      return CRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(attribute_output_p->name_string == NULL)
      return CRM_ERROR_ATTRIBUTE_NAME_NULL;

   if(strlen(attribute_entry_p->name) > (attribute_output_p->name_length))
   {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Debug");  
      attribute_output_p->name_length  = strlen(attribute_entry_p->name)  +1;
      return CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;
   }

   if(attribute_output_p->value_string == NULL)
   {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Debug");  
      return CRM_ERROR_ATTRIBUTE_VALUE_NULL;
   }
   if(strlen(attribute_entry_p->value) > (attribute_output_p->value_length))
   {
      OF_LOG_MSG(OF_LOG_CRM,OF_LOG_DEBUG,"Debug");  
      attribute_output_p->value_length  = strlen(attribute_entry_p->value)  +1;
      return CRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
   }


   strcpy(attribute_output_p->name_string, attribute_entry_p->name);
   attribute_output_p->name_length  = strlen(attribute_entry_p->name)  +1;
   strcpy(attribute_output_p->value_string, attribute_entry_p->value);
   attribute_output_p->value_length  = strlen(attribute_entry_p->value)  +1;
   return CRM_SUCCESS;
}

/*******************************************************************************************************/
int32_t crm_get_next_attribute(of_list_t *attributes,char *current_attribute_name,
                                struct crm_attribute_name_value_output_info *attribute_output)
{
  uint8_t current_entry_found_b = 0;
  struct  crm_resource_attribute_entry *attribute_entry_p;
  int32_t retval;

  if((attribute_output == NULL) || (current_attribute_name == NULL))
    return CRM_FAILURE;

  if(attribute_output->name_string == NULL)
    return CRM_ERROR_ATTRIBUTE_NAME_NULL;

  if(attribute_output->value_string == NULL)
    return CRM_ERROR_ATTRIBUTE_VALUE_NULL;

  retval = CRM_ERROR_NO_MORE_ENTRIES;

  OF_LIST_SCAN(*attributes,attribute_entry_p,struct crm_resource_attribute_entry*, CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET )
  {
    if(current_entry_found_b == 0)
    {
      if(!strcmp(attribute_entry_p->name,current_attribute_name))
      {
        current_entry_found_b = 1;
        continue;
      }
      else
       continue;
    }
    else
    {
    /* skip the first matching attribute name */
      if(strlen(attribute_entry_p->name) > (attribute_output->name_length))
      {
        attribute_output->name_length  = strlen(attribute_entry_p->name) +1;
        retval = CRM_ERROR_INVALID_ATTRIBUTE_NAME_LEN;
        break;
      }

      if(strlen(attribute_entry_p->value) > (attribute_output->value_length))
      {
        attribute_output->value_length  = strlen(attribute_entry_p->value)  +1;
        return CRM_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
      }
      strcpy(attribute_output->name_string,attribute_entry_p->name);
      attribute_output->name_length  = strlen(attribute_entry_p->name) +1;
      strcpy(attribute_output->value_string, attribute_entry_p->value);
      attribute_output->value_length  = strlen(attribute_entry_p->value)  +1;   
      retval = CRM_SUCCESS;
      break;
    }
  }
  if(current_entry_found_b == 0)
    retval = CRM_ERROR_INVALID_NAME;

  return retval;

}
/*****************************************************************************************************/
int32_t crm_get_exact_attribute(of_list_t *attributes_p,
      char  *attribute_name_string,
      struct crm_attribute_name_value_output_info *attribute_output)
{
   struct   crm_resource_attribute_entry *attribute_entry_scan_p=NULL;
   uchar8_t lstoffset = CRM_RESOURCE_ATTRIBUTE_ENTRY_LISTNODE_OFFSET;

   OF_LIST_SCAN(*attributes_p, attribute_entry_scan_p, struct crm_resource_attribute_entry*, lstoffset )
   {
      if(!strcmp(attribute_entry_scan_p->name, attribute_name_string))
      {
         OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ALL, "attribute name found ");
         strcpy(attribute_output->name_string,attribute_entry_scan_p->name);
         attribute_output->name_length  = strlen(attribute_entry_scan_p->name) +1;
         strcpy(attribute_output->value_string, attribute_entry_scan_p->value);
         attribute_output->value_length  = strlen(attribute_entry_scan_p->value)  +1;
         return CRM_SUCCESS;
      }
   }
   OF_LOG_MSG(OF_LOG_CRM, OF_LOG_ALL, "attribute name not found ");
   return CRM_FAILURE;
}

