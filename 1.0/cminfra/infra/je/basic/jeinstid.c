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
 * File name: jeinstid.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: This file contains the code for generating Instance ID
*/

#ifdef CM_SUPPORT
#ifdef CM_JE_SUPPORT

#include "jeincld.h"
#include "jeldef.h"
#include "jelif.h"

/***************************************************************************
 * Global definitions Section
 ***************************************************************************/
#define CM_JE_MAX_DEFAULT_INSTANCEID_VALUE 2048        /* Maximum default instance value */
/* #define CM_JE_MAX_DEFAULT_RULE_SIZE  128  Minimum default instance size*/

#define CM_JE_MAX_BIT_POS    8 /* Number of bits */

/* The global variable usCM_JEMaxInstanceIdValue_f will
   be initialised to  MAX number of instance id  */

struct je_instance_info
{
  uint32_t max_instance_id;
  /* This will be initialised to max_instance_id_f/8 */
  uint32_t max_instance_size;
  /* Variable used for returning instance id's */
  uint32_t instance_id;
  /* Variable used for returning instance id when instance_id_g reaches 
     CM_JE_MAX_RULE_VALUE */
  uint32_t buf_instance_id;
  /*holder indicating the bit map corresponding to the Instance id.
     It will hold 1 if the corresponding Instance is present or else 0 */
  unsigned char *instance_id_valid;
};

 struct je_instance_info *je_instance_info_g = NULL;

/***************************************************************************
 * Function declaration Section
 ***************************************************************************/

 void je_isntance_id_global_data_init (void);
 int16_t je_check_instance_id (uint32_t ulInstanceId_p);

/***************************************************************************
 * Function definition Section
 ***************************************************************************/
/***************************************************************************
 * Function Name : je_instanceid_generator_init
 * Description   : This function will get the value of MAX instance-id's that
 *                 Configured and set the other global varibe accordingly
 *                  variables.
 * Input         : None.
 * Output        : None
 * Return value  : None.
***************************************************************************/
int32_t je_instanceid_generator_init (void)
{
  uint32_t max_instance_id;
  struct je_instance_info *instance_info = NULL;

  CM_JE_DEBUG_PRINT ("Entered");
  je_isntance_id_global_data_init ();
  cm_je_get_instandid_generator_cnf (&max_instance_id);

  if (CM_MODU32 (max_instance_id, CM_JE_MAX_BIT_POS) != 0)
  {
    max_instance_id = (CM_DIVU32 (max_instance_id, CM_JE_MAX_BIT_POS)
                       * CM_JE_MAX_BIT_POS) + CM_JE_MAX_BIT_POS;
  }

  instance_info = je_instance_info_g;
  instance_info->max_instance_id = max_instance_id;

  /* Checking its a multiple of CM_JE_MAX_BIT_POS or not */
  if (CM_MODU32 (instance_info->max_instance_id, CM_JE_MAX_BIT_POS) != 0)
    instance_info->max_instance_id = (CM_DIVU32 (instance_info->max_instance_id,
                                                CM_JE_MAX_BIT_POS) *
                                    CM_JE_MAX_BIT_POS);
  instance_info->max_instance_size =
    CM_DIVU32 (instance_info->max_instance_id, CM_JE_MAX_BIT_POS);
  instance_info->instance_id_valid =
    (unsigned char *) of_calloc (instance_info->max_instance_size, sizeof (unsigned char));
  if (instance_info->instance_id_valid == NULL)
  {
    CM_JE_DEBUG_PRINT ("Memory Allocation failure");
    return OF_FAILURE;
  }
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : je_instanceid_generator_deinit
 * Description   : This function will free the allocated memory during
 *                 Deinitialization.
 * Input         : uint32_t - Reason code for invocation of function
 * Output        : None
 * Return value  : None.
***************************************************************************/
void je_instanceid_generator_deinit (void)
{
  CM_JE_DEBUG_PRINT ("Entered");
  if (!je_instance_info_g)
    return;

  of_free ((je_instance_info_g)->instance_id_valid);
  (je_instance_info_g)->instance_id_valid = NULL;

  /* free the whole container */
  of_free (je_instance_info_g);
  je_instance_info_g = NULL;

  return;
}

/***************************************************************************
 * Function Name : je_get_new_instance_id
 * Description   : This function allocates an ID for a new Instance,when a Instance
 *                 is added. It checks for a free bit(having value 0) position
 *                 in the array JEInstanceIdValid_g and returns the position and
 *                 sets that bit to 1. The global variable instance_id_g will
 *                 be incremented and the corresponding bit will be set and the
 *                 position will be returned. If the instance_id_g reaches
                  CM_JE_MAX_RULE_VALUE then 0th bit of 0th byte will be set 
 *                 to 1 and from there onwards a search will be made for a free
 *                 bit position, if found buf_instance_id_g will be set to 
 *                 this position and the position will be returned. If no free
 *                 position found this function returns OF_FAILURE.
 * Input         : None.
 * Output        : None.
 * Return value  : Returns the new instance Id value on success
 *                 or OF_FAILURE if no free instance ID is present
 **************************************************************************/
int16_t je_get_new_instance_id (uint32_t * pInstanceId_p)
{
  uint32_t byte_position;
  uint32_t byte_counter, upper_num = 0;
  int16_t bit_counter = 1;
  int16_t mask = 1;
  unsigned char free_bit = TRUE;
  struct je_instance_info *instance_info = NULL;

  if (!pInstanceId_p)
    return OF_FAILURE;

  instance_info = je_instance_info_g;

  if (instance_info->instance_id < instance_info->max_instance_id)
  {
    ++(instance_info->instance_id);
    *pInstanceId_p = instance_info->instance_id;
    return OF_SUCCESS;
  }
  if (instance_info->buf_instance_id == instance_info->max_instance_id)
  {
    instance_info->buf_instance_id = 0;
  }

  byte_position = CM_DIVU32 (instance_info->buf_instance_id, CM_JE_MAX_BIT_POS);
  byte_counter = byte_position;
  upper_num = instance_info->max_instance_size;
  if (byte_position == 0)
    free_bit = FALSE;
  while (1)
  {
    for (; (byte_position < upper_num); byte_position++)
    {
      for (bit_counter = CM_JE_MAX_BIT_POS - 1;
           bit_counter >= 0; bit_counter--)
      {
        if (!
            (instance_info->instance_id_valid[byte_position] & (mask << bit_counter)))
        {
          if ((byte_position == 0) && (bit_counter == CM_JE_MAX_BIT_POS - 1))
          {
            instance_info->buf_instance_id = instance_info->max_instance_id;
          }
          else
          {
            instance_info->buf_instance_id = (CM_MULTU32 (byte_position,
                                                    CM_JE_MAX_BIT_POS))
              + (CM_JE_MAX_BIT_POS - 1 - bit_counter);
          }
          *pInstanceId_p = instance_info->buf_instance_id;
          return OF_SUCCESS;
        }
      }
    }
    if (free_bit)
    {
      free_bit = FALSE;
      byte_position = 0;
      upper_num = byte_counter;
    }
    else
      break;
  }
  return OF_FAILURE;
}

/***************************************************************************
 * Function Name : je_set_bit_for_instance_id
 * Description   : This function sets a particular bit position in the global
 *                 pointer instance_id_valid_g to either 0 or 1.It first
 *                 validates the values passed to it. If the arguments are not
 *                 having valid values then OF_FAILURE is returned. If the bit
 *                 position has the same value as supplied in the field 
 *                 setflag_b it returns OF_FAILURE. The function returns
 *                 OF_SUCCESS on success
 * Input         : sInstanceId_p - This field identifies the position which is to
 *                             be set to 0 or 1.
 *                 setflag_b - This field identifies the value for the bit.
 *                             If TRUE is supplied the bit will be set to 1.
 *                             If FALSE is supplied the bit will be set to 0.
 * Output        : None.
 * Return value  : Returns OF_SUCCESS on success
 *                 or OF_FAILURE on error
 **************************************************************************/
int16_t je_set_bit_for_instance_id (uint32_t ulInstanceId_p, unsigned char setflag_b)
{
  uint32_t bit_position;
  uint32_t byte_position;
  int16_t mask = 01;
  struct je_instance_info *instance_info = NULL;

  instance_info = je_instance_info_g;

  if (ulInstanceId_p > instance_info->max_instance_id)
  {
    /* User should be able to add a instanceId greater than max_instance_id_f.
     * No flag is set in this case.
     */
    if (ulInstanceId_p >= 0xffffffff)
      return OF_FAILURE;
    else
    {
      return OF_SUCCESS;
    }
  }
  if ((setflag_b == TRUE) || (setflag_b == FALSE))
  {
    byte_position = CM_DIVU32 (ulInstanceId_p, CM_JE_MAX_BIT_POS);
    bit_position = CM_MODU32 (ulInstanceId_p, CM_JE_MAX_BIT_POS);
    mask = mask << ((CM_JE_MAX_BIT_POS - 1) - bit_position);
    if ((byte_position == instance_info->max_instance_size) && (bit_position == 0))
    {
      if (setflag_b)
      {
        if (!(instance_info->instance_id_valid[0] & mask))
        {
          instance_info->instance_id_valid[0] = instance_info->instance_id_valid[0] | (mask);
          return OF_SUCCESS;
        }
      }
      else
      {
        if (instance_info->instance_id_valid[0] & mask)
        {
          instance_info->instance_id_valid[0] = instance_info->instance_id_valid[0] & (~mask);
          return OF_SUCCESS;
        }
      }
    }
    else
    {
      if (setflag_b)
      {
        if (!(instance_info->instance_id_valid[byte_position] & mask))
        {
          instance_info->instance_id_valid[byte_position] =
            instance_info->instance_id_valid[byte_position] | (mask);
          return OF_SUCCESS;
        }
      }
      else
      {
        if (instance_info->instance_id_valid[byte_position] & mask)
        {
          instance_info->instance_id_valid[byte_position] =
            instance_info->instance_id_valid[byte_position] & (~mask);
          return OF_SUCCESS;
        }
      }
    }
  }
  return OF_FAILURE;
}

/***************************************************************************
 * Function Name : je_set_instance_id_value
 * Description   : This function sets instance_id_g variable with the value
 *                 passed to it. This function checks the current value of
 *                 instance_id_g with the value present in the field
 *                 sInstanceVal_p. If it is greater than usIkav2InstanceId_g,
 *                 usIkav2InstanceId_g will be updated to the value present in
 *                 sInstanceVal_p.
 * Input         : iInstanceVal_p - This field identifies the value which updates
 *                 instance_id_g variable. The value should be between 1 and
 *                 CM_JE_MAX_RULE_VALUE, both inclusive.
 * Output        : None.
 * Return value  : None.
 **************************************************************************/
void je_set_instance_id_value (uint32_t ulInstanceVal_p)
{
  struct je_instance_info *instance_info = NULL;

  CM_JE_DEBUG_PRINT ("Entered");
  instance_info = je_instance_info_g;
  if ((ulInstanceVal_p > 0)
      && (ulInstanceVal_p <= instance_info->max_instance_id))
  {
    if (instance_info->instance_id < ulInstanceVal_p)
      instance_info->instance_id = ulInstanceVal_p;
  }
  return;
}

/***************************************************************************
 * Function Name : je_check_instance_id
 * Description   : This function checks instance_id_g variable with the value
 *                 passed to it. This function extracts the bit position
 *                 depending upon the value passed to it and checks wether
 *                 the bit set or not.If the bit is set this function returns
 *                 CM_JE_RULE_ID_SET. If the bit is not set this function
 *                 returns  CM_IKEv2_RULE_ID_NOT_SET. If the value passed is
 *                 invalid this function returns OF_FAILURE.
 * Input         : ulInstanceVal_p - This field identifies the value for which
 *                 the bit position has to be checked.
 * Output        : None.
 * Return value  : CM_JE_RULE_ID_NOT_SET if the bit is not set
 *               : CM_JE_RULE_ID_SET if the bit is set
 *                 OF_FAILURE if value passed is invalid
 **************************************************************************/
 int16_t je_check_instance_id (uint32_t ulInstanceId_p)
{
  int16_t sBitPosition;
  uint32_t byte_position;
  int16_t mask = 01;
  struct je_instance_info *instance_info = NULL;

  CM_JE_DEBUG_PRINT ("Entered");
  instance_info = je_instance_info_g;

  if (ulInstanceId_p > instance_info->max_instance_id)
    return OF_FAILURE;

  byte_position = CM_DIVU32 (ulInstanceId_p, CM_JE_MAX_BIT_POS);
  sBitPosition = CM_MODU32 (ulInstanceId_p, CM_JE_MAX_BIT_POS);
  mask = mask << ((CM_JE_MAX_BIT_POS - 1) - sBitPosition);

  if ((byte_position == instance_info->max_instance_size) && (sBitPosition == 0))
  {
    if (!(instance_info->instance_id_valid[0] & mask))
    {
      return CM_JE_RULE_ID_NOT_SET;
    }
    else
    {
      return CM_JE_RULE_ID_SET;
    }
  }
  else
  {
    if (!(instance_info->instance_id_valid[byte_position] & mask))
    {
      return CM_JE_RULE_ID_NOT_SET;
    }
    else
    {
      return CM_JE_RULE_ID_SET;
    }
  }
  return OF_FAILURE;
}

/***************************************************************************
 * Function Name : je_isntance_id_global_data_init
 * Description   : This function will initialize all global and file scope
 *                  variables.
 * Input         : None
 * Output        : None
 * Return value  : None.
***************************************************************************/
 void je_isntance_id_global_data_init ()
{
  CM_JE_DEBUG_PRINT ("Entered");
  je_instance_info_g =
    (struct je_instance_info *) of_calloc (1, sizeof (struct je_instance_info));
  return;
}

#endif
#endif
