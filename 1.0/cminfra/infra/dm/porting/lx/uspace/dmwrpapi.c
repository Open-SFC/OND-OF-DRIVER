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
 * File name: dmwrpapi.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/15/2013
 * Description:  
*/

#ifdef CM_DM_SUPPORT

//#include "dmincld.h"
#include "cmincld.h"
#include "cmdefs.h"
#include "cmutil.h"
#include "cmdll.h"
#include "cmerror.h"

#include "cmtransgdf.h"
#include "dmgdef.h"
#include "jegdef.h"
#include "dmgif.h"
#include "dmldef.h"

/*
*****************************************************************************
* Function prototype declarations:
******************************************************************************
*/
 int32_t UCMExtractDMResponseFromBuff (uint32_t command_id,
                                                 char * pRespBuff,
                                                 uint32_t * puiReqBufLen,
                                                 struct cm_result * cm_resp_msg_p);

 int32_t ucmdm_SendRequest (void * tnsprt_channel_p,
                                      char * dm_path_p,
                                      uint32_t sub_command_idID,
                                      uint32_t mgmt_engine_id,
                                      struct cm_role_info * role_info_p);

 int32_t ucmdm_ReceiveResponse (void * tnsprt_channel_p,
                                          struct cm_result ** ppResponseMsg);

 int32_t ucmdm_CopyNodeInfoFromResp (struct cm_dm_node_info * pResnode_info_p,
                                               uint32_t uiNodeCnt,
                                               uint32_t result_code,
                                               struct cm_dm_node_info ** node_info_pp);

int32_t ucmdm_CopyIDArrayFromRespBuff (uint32_t opaque_info_length,
                                       void * opaque_info_p,
                                       struct cm_array_of_uints ** pIDArray);

int32_t UcmExtractRolePermFromBuff( struct cm_dm_role_permission *role_perm_p, char* pBuff );

uint32_t uiInt32Size_g=CM_UINT32_SIZE;
uint32_t uiInt64Size_g=CM_UINT64_SIZE;
uint32_t uiOSIntSize_g=CM_UINT_OS_SIZE;
uint32_t uiOSLongIntSize_g=CM_ULINT_OS_SIZE;

/**
  \addtogroup DMWRPAPI DM Wrapper APIs
  This API fills Node Info structure for the given DM Path.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>node_info_pp:</i> Double pointer to Node Info structure.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_node_info (void * tnsprt_channel_p,
                           char * dm_path_p,
                           uint32_t mgmt_engine_id,
                           struct cm_role_info * role_info_p,
                           struct cm_dm_node_info ** node_info_pp)
{

  return OF_SUCCESS;

  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;

  return_value = ucmdm_SendRequest (tnsprt_channel_p, dm_path_p, CM_CMD_GET_DM_NODE_INFO,
                               mgmt_engine_id, role_info_p);

  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  *node_info_pp = pResponseMsg->data.dm_info.node_info_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
NOT USED/REQUIRED
   <b>Input Parameters: </b>\n
   <i>:</i>
   <b>Output Parameters: </b>\n
   <i>:</i>
   <b>Return Value: </b>\n
   <i></i>
 **/
 int32_t ucmdm_CopyNodeInfoFromResp (struct cm_dm_node_info * pResnode_info_p,
                                               uint32_t uiNodeCnt,
                                               uint32_t result_code,
                                               struct cm_dm_node_info ** node_info_pp)
{
  struct cm_dm_node_info *node_info_p, *tmp_node_info_p;
  int32_t ii;

  node_info_p =
    (struct cm_dm_node_info *) of_calloc (uiNodeCnt, sizeof (struct cm_dm_node_info));
  if (node_info_p == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT
      (" Memory Allocation failed for NodeInfo Structure");
    return OF_FAILURE;
  }

  switch (result_code)
  {
    case CM_JE_SUCCESS:
      if (pResnode_info_p)
      {
        for (ii = 0; ii < uiNodeCnt; ii++)
        {
          tmp_node_info_p = &pResnode_info_p[ii];
          of_memcpy (&node_info_p[ii], tmp_node_info_p, sizeof (struct cm_dm_node_info));
          if (tmp_node_info_p->dm_path_p)
          {
            node_info_p[ii].dm_path_p = (char *) of_calloc (1,
                                                          of_strlen
                                                          (tmp_node_info_p->dm_path_p)
                                                          + 1);
            if (node_info_p[ii].dm_path_p)
            {
              of_strcpy (node_info_p[ii].dm_path_p, tmp_node_info_p->dm_path_p);
            }
          }
          if (tmp_node_info_p->name_p)
          {
            node_info_p[ii].name_p =
              (char *) of_calloc (1, of_strlen (tmp_node_info_p->name_p) + 1);
            if (node_info_p[ii].name_p)
            {
              of_strcpy (node_info_p[ii].name_p, tmp_node_info_p->name_p);
            }
          }
	  if (tmp_node_info_p->friendly_name_p)
          {
            node_info_p[ii].friendly_name_p =
              (char *) of_calloc (1, of_strlen (tmp_node_info_p->friendly_name_p) + 1);
            if (node_info_p[ii].friendly_name_p)
            {
              of_strcpy (node_info_p[ii].friendly_name_p, tmp_node_info_p->friendly_name_p);
            }
          }
	  else
	    node_info_p[ii].friendly_name_p = NULL;

          if (tmp_node_info_p->description_p)
          {
            node_info_p[ii].description_p = (char *) of_calloc (1,
                                                               of_strlen
                                                               (tmp_node_info_p->description_p)
                                                               + 1);
            if (node_info_p[ii].description_p)
            {
              of_strcpy (node_info_p[ii].description_p, tmp_node_info_p->description_p);
            }
          }
        }
        *node_info_pp = node_info_p;
      }
      break;
    case CM_JE_FAILURE:
      of_free (node_info_p);
      return OF_FAILURE;
    default:
      of_free (node_info_p);
      break;
  }
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
NOT USED/REQUIRED
   <b>Input Parameters: </b>\n
   <i>:</i>
   <b>Output Parameters: </b>\n
   <i>:</i>
   <b>Return Value: </b>\n
   <i></i>
 **/
int32_t ucmdm_CopyIDArrayFromRespBuff (uint32_t opaque_info_length,
                                       void * opaque_info_p,
                                       struct cm_array_of_uints ** pIDArray)
{
  int32_t ii;
  struct cm_array_of_uints *pTmpAry;
  uint32_t *pTemp;

  pTmpAry = (struct cm_array_of_uints *) of_calloc (1, sizeof (struct cm_array_of_uints));
  if (!pTmpAry)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    return OF_FAILURE;
  }

  pTmpAry->count_ui = opaque_info_length;
  pTmpAry->uint_arr_p = (uint32_t *) of_calloc (opaque_info_length, sizeof (uint32_t));
  if (!pTmpAry->uint_arr_p)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pTmpAry);
    return OF_FAILURE;
  }

  pTemp = (uint32_t *) (opaque_info_p);

  for (ii = 0; ii < opaque_info_length; ii++)
  {
    pTmpAry->uint_arr_p[ii] = *(pTemp + ii);
  }

  *pIDArray = pTmpAry;
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
   This API returns Node Info and its Children Nodes info for given DM Path.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>node_info_pp:</i> Double pointer to Node Info structure.
  <i>child_info_array_pp:</i> Double pointer to array of Node Info structures.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_node_and_child_info (void * tnsprt_channel_p,
                                   char * dm_path_p,
                                   uint32_t mgmt_engine_id,
                                   struct cm_role_info * role_info_p,
                                   struct cm_dm_node_info ** node_info_pp,
                                   struct cm_array_of_structs ** child_info_array_pp)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;
  //struct cm_dm_node_info *node_info_p = NULL;

  return_value = ucmdm_SendRequest (tnsprt_channel_p, dm_path_p,
                               CM_CMD_GET_DM_NODE_AND_CHILD_INFO,
                               mgmt_engine_id, role_info_p);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }
#if 0
  return_value =
    ucmdm_CopyNodeInfoFromResp (pResponseMsg->data.dm_info.node_info_p,
                                pResponseMsg->data.dm_info.count_ui,
                                pResponseMsg->result_code, &node_info_p);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    of_free (pResponseMsg);
    return OF_FAILURE;
  }
#endif
  *node_info_pp = pResponseMsg->data.dm_info.node_info_p;
  *child_info_array_pp = pResponseMsg->data.dm_info.node_info_array_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
   This API returns array of Key Child Nodes for given DM Path.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>child_info_array_pp:</i> Double pointer to array of Key Child Nodes.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_key_childs (void * tnsprt_channel_p,
                                   char * dm_path_p,
                                   uint32_t mgmt_engine_id,
                                   struct cm_role_info * role_info_p,
                                   struct cm_array_of_structs ** child_info_array_pp)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;
  //struct cm_dm_node_info *node_info_p = NULL;

  return_value = ucmdm_SendRequest (tnsprt_channel_p, dm_path_p,
                                CM_CMD_GET_DM_KEY_CHILDS,
                               mgmt_engine_id, role_info_p);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }
  *child_info_array_pp = pResponseMsg->data.dm_info.node_info_array_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
   This API returns Key Child Node info for given DM Path.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>node_info_pp:</i> Double pointer to Key Child Node Info structure.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_key_child_info (void * tnsprt_channel_p,
                               char * dm_path_p,
                               uint32_t mgmt_engine_id,
                               struct cm_role_info * role_info_p,
                               struct cm_dm_node_info ** node_info_pp)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;

  return_value = ucmdm_SendRequest (tnsprt_channel_p, dm_path_p,
                               CM_CMD_GET_DM_KEY_CHILD_INFO,
                               mgmt_engine_id, role_info_p);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    
    if (pResponseMsg && pResponseMsg->result.error.error_code == CM_COMMON_NO_KEY_FOR_TABLE)
    {
	return_value = CM_COMMON_NO_KEY_FOR_TABLE;
        UCMFreeUCMResult (pResponseMsg);
    }
    else
    {
	    return_value = OF_FAILURE;
    }
    return return_value;
  }

  *node_info_pp = pResponseMsg->data.dm_info.node_info_p;
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
   This API used by Management Engines to Get Node Info and IDs for given DM Path.
  <b>Input Parameters: </b>\n
  <i>head_node_path_p:</i> Pointer to Data Model Path
  <i>traversal_type_ui:</i> Typeof Traversal in Data Model Tree: UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM or UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>node_info_pp:</i> Double Pointer to Node Info Structure.
  <i>opaque_info_pp:</i> Double Pointer to Opaque Info structure which contains Node IDs of Nodes across dm path.
  <i>opaque_len_p:</i> Number of Nodes across dm path.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_first_node_info (char * head_node_path_p,
                                uint32_t traversal_type_ui,
                                void * tnsprt_channel_p,
                                uint32_t mgmt_engine_id,
                                struct cm_role_info * role_info_p,
                                struct cm_dm_node_info ** node_info_pp,
                                void ** opaque_info_pp, uint32_t * opaque_len_p)
{
  unsigned char *pSocketBuf = NULL, *pTmp;
  int32_t return_value;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  uint32_t uiBuffLen = 0;

  if (!tnsprt_channel_p || !role_info_p)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return OF_FAILURE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_DM_FIRST_NODE_INFO,
                         head_node_path_p, 0, NULL, NULL, &uiBuffLen);

//  uiBuffLen += sizeof (uint32_t);     /* To copy Traversal Type */
  uiBuffLen += uiOSIntSize_g;     /* To copy Traversal Type */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append traversal type to SocketBuffer */
  pTmp = pSocketBuf;
//  of_mbuf_put_32 (pTmp + (uiBuffLen - sizeof (uint32_t)), traversal_type_ui);
  of_mbuf_put_32 (pTmp + (uiBuffLen - uiOSIntSize_g), traversal_type_ui);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    if (pResponseMsg)
      of_free (pResponseMsg);
    return OF_FAILURE;
  }

  *node_info_pp = pResponseMsg->data.dm_info.node_info_p;
  *opaque_len_p = pResponseMsg->data.dm_info.opaque_info_length;
  *opaque_info_pp = pResponseMsg->data.dm_info.opaque_info_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
   This API used by Management Engines to Get Next Node Info and IDs for given DM Path and Node IDs.
  <b>Input Parameters: </b>\n
  <i>head_node_path_p:</i> Pointer to Data Model Path
  <i>traversal_type_ui:</i> Typeof Traversal in Data Model Tree: UCMDM_TRAVERSAL_TYPE_TOP_TO_BOTTOM or UCMDM_TRAVERSAL_TYPE_BOTTOM_TO_TOP.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>node_info_pp:</i> Double Pointer to Node Info Structure.
  <b>In/Out Params</b>\n
  <i>opaque_info_pp:</i> Double Pointer to Opaque Info structure which contains Node IDs of Nodes across dm path.
  <i>opaque_len_p:</i> Number of Nodes across dm path.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_next_node_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               void * tnsprt_channel_p,
                               uint32_t mgmt_engine_id,
                               struct cm_role_info * role_info_p,
                               struct cm_dm_node_info ** node_info_pp,
                               void ** opaque_info_pp, uint32_t * opaque_len_p)
{
  unsigned char *pSocketBuf = NULL, *pTmp;
  int32_t return_value;
  uint32_t uiReqMsgLen, count_ui, *pTempIDAry, uiBuffLen = 0;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;

  if (!tnsprt_channel_p || !role_info_p || !(*opaque_info_pp) || (*opaque_len_p == 0))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return OF_FAILURE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_DM_NEXT_NODE_INFO,
                         head_node_path_p, 0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  count_ui = *opaque_len_p;
  uiBuffLen += uiOSIntSize_g +        /* for Traversal type */
    uiOSIntSize_g +           /* for OpaqueInfo Len */
    ((count_ui) * (uiOSIntSize_g));    /*For OpaqueInfo */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);

  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append traversal type to SocketBuffer */
  pTmp = pSocketBuf;
  pTmp = of_mbuf_put_32 (pTmp + uiReqMsgLen, traversal_type_ui);

  /* Copy OpaqueInfo Len  */
  pTmp = of_mbuf_put_32 (pTmp, *opaque_len_p);

  /* Copy OpaqueInfo  */
  of_memcpy (pTmp, *opaque_info_pp, ((*opaque_len_p) * uiOSIntSize_g));
  pTmp += ((*opaque_len_p) * uiOSIntSize_g);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT ("ucmdm_ReceiveResponse failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  //clean opaque info
  if ((*opaque_info_pp) != NULL)
  {
    of_free(*opaque_info_pp);
  }

  if (pResponseMsg->result_code == CM_JE_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Next Node Success");
    return_value = OF_SUCCESS;
  }
  else
  {
    CM_DMTRANS_DEBUG_PRINT ("Next Node failure");
    return_value = OF_FAILURE;
  }

  *node_info_pp = pResponseMsg->data.dm_info.node_info_p;
  *opaque_len_p = pResponseMsg->data.dm_info.opaque_info_length;
  *opaque_info_pp = pResponseMsg->data.dm_info.opaque_info_p;

  pTempIDAry = (uint32_t *) (*opaque_info_pp);
  UCMFreeUCMResult (pResponseMsg);
  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);
  return return_value;
}

/**
  \ingroup DMWRPAPI
   Checks if the given Key field Information exists in the DM Path.
  <b>Input Parameters: </b>\n
  <i>node_path_p:</i> Pointer to Data Model Path
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <i>key_p:</i> Data Type of Key
  <i>key_type:</i> Role of Administrator
  <b>Return Value:</b>
  <i>TRUE:</i>In case it Exists.
  <i>FALSE:</i>In case it does not Exist.
 **/
unsigned char cm_dm_is_instance_map_entry_using_key_and_name_path (char * node_path_p,
                                                      void * tnsprt_channel_p,
                                                      uint32_t mgmt_engine_id,
                                                      struct cm_role_info * role_info_p,
                                                      void * key_p,
                                                      uint32_t key_type)
{
  unsigned char *pSocketBuf = NULL, *pTmp;
  int32_t return_value;
  uint32_t uiReqMsgLen, uiBuffLen = 0;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  unsigned char bInstExists = FALSE;

  if (!tnsprt_channel_p || !role_info_p)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return FALSE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return FALSE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return FALSE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY,
                         node_path_p, 0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += uiOSIntSize_g +        /* for Key type */
    uiOSIntSize_g +           /* for OpaqueInfo Len */
    of_strlen (key_p);            /*For OpaqueInfo */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return FALSE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Key type to SocketBuffer */
  pTmp = pSocketBuf;
  pTmp = of_mbuf_put_32 (pTmp + uiReqMsgLen, key_type);

  /* Copy Opaque Len */
  pTmp = of_mbuf_put_32 (pTmp, of_strlen (key_p));

  /* Copy OpaqueInfo  */
  of_memcpy (pTmp, key_p, of_strlen (key_p));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return FALSE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return FALSE;
  }
  bInstExists = (unsigned char) pResponseMsg->result.success.reserved;

  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  UCMFreeUCMResult (pResponseMsg);
  of_free (pSocketBuf);

  return bInstExists;
}

/**
  \ingroup DMWRPAPI
   Checks if the given Key array exists in the DM Path.
  <b>Input Parameters: </b>\n
  <i>node_path_p:</i> Pointer to Data Model Path
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <i>nv_pair_arr_p:</i> Key Array
  <b>Return Value:</b>
  <i>TRUE:</i>In case it Exists.
  <i>FALSE:</i>In case it does not Exist.
 **/
unsigned char cm_dm_is_instance_map_entry_using_key_array_and_name_path (char * node_path_p,
                                                      void * tnsprt_channel_p,
                                                      uint32_t mgmt_engine_id,
                                                      struct cm_role_info * role_info_p,
                                                      struct cm_array_of_nv_pair *nv_pair_arr_p)
{
  unsigned char *pSocketBuf = NULL, *pTmp;
  int32_t return_value;
  uint32_t uiReqMsgLen, uiBuffLen = 0;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  unsigned char bInstExists = FALSE;

  if (!tnsprt_channel_p || !role_info_p)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return FALSE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return FALSE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return FALSE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEYARRAY,
                         node_path_p, 0, nv_pair_arr_p, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;

	/* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return FALSE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Key type to SocketBuffer */
  pTmp = pSocketBuf;

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return FALSE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return FALSE;
  }
  bInstExists = (unsigned char) pResponseMsg->result.success.reserved;

  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  UCMFreeUCMResult (pResponseMsg);
  of_free (pSocketBuf);

  return bInstExists;
}

/**
  \ingroup DMWRPAPI

  <b>Input Parameters: </b>\n
  <i>child_path_p:</i>
  <i>parent_path_p:</i>
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>Return Value:</b>
  <i>TRUE:</i>In case it is.
  <i>FALSE:</i>In case it is not.
 **/
unsigned char cm_dm_is_child_element (char * child_path_p,
                              char * parent_path_p,
                              void * tnsprt_channel_p,
                              uint32_t mgmt_engine_id,
                              struct cm_role_info * role_info_p)
{
  unsigned char *pSocketBuf = NULL, *pTmp;
  int32_t return_value;
  uint32_t uiReqMsgLen, uiBuffLen = 0;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  unsigned char bIsChildExists = FALSE;

  if (!tnsprt_channel_p || !role_info_p)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return FALSE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return FALSE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return FALSE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_IS_DM_CHILD_NODE,
                         child_path_p, 0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += uiOSIntSize_g +        /* for OpaqueInfo Len */
    of_strlen (parent_path_p);     /*For OpaqueInfo */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);

  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return FALSE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Copy OpaqueInfo Len  */
  pTmp = pSocketBuf;
  pTmp = of_mbuf_put_32 (pTmp + uiReqMsgLen, of_strlen (parent_path_p));

  /* Copy OpaqueInfo  */
  of_memcpy (pTmp, parent_path_p, of_strlen (parent_path_p));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return FALSE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return FALSE;
  }

  bIsChildExists = (unsigned char) pResponseMsg->result.success.reserved;
  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);
  UCMFreeUCMResult (pResponseMsg);

  return bIsChildExists;
}

/**
  \ingroup DMWRPAPI
   This API used by Management Engines to Keys array from the given DM Path.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>keys_array_pp:</i> Double Pointer to Array of Keys Structure.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_keys_array_from_name_path (void * tnsprt_channel_p,
                                        char * dm_path_p,
                                        uint32_t mgmt_engine_id,
                                        struct cm_role_info * role_info_p,
                                        struct cm_array_of_nv_pair ** keys_array_pp)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;

  return_value = ucmdm_SendRequest (tnsprt_channel_p, dm_path_p,
                               CM_CMD_GET_DM_KEYS_ARRAY,
                               mgmt_engine_id, role_info_p);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  *keys_array_pp = pResponseMsg->data.dm_info.dm_node_nv_pairs_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
   This API used by Management Engines to get List of Map Entries from the given DM Path.
  <b>Input Parameters: </b>\n
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>role_info_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>pcount_ui:</i> Number of Instances.
  <i>map_pp:</i> Double Pointer to List of map entries.
  <b>Return Value:</b>
  <i>OF_SUCCESS:</i>In Success case
  <i>OF_FAILURE:</i>In Failure case
 **/
int32_t cm_dm_get_instance_map_entry_list_using_name_path (void * tnsprt_channel_p,
                                                     char * dm_path_p,
                                                     uint32_t mgmt_engine_id,
                                                     struct cm_role_info * role_info_p,
                                                     uint32_t * pcount_ui,
                                                     struct cm_dm_instance_map **
                                                     map_pp)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;

  return_value = ucmdm_SendRequest (tnsprt_channel_p, dm_path_p,
                               CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH,
                               mgmt_engine_id, role_info_p);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  *pcount_ui = pResponseMsg->data.dm_info.count_ui;
  *map_pp = pResponseMsg->data.dm_info.instance_map_list_p;

  UCMFreeUCMResult (pResponseMsg);

  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
NOT USED/REQUIRED
   <b>Input Parameters: </b>\n
   <i>:</i>
   <b>Output Parameters: </b>\n
   <i>:</i>
   <b>Return Value: </b>\n
   <i></i>
 **/
 int32_t ucmdm_SendRequest (void * tnsprt_channel_p,
                                      char * dm_path_p,
                                      uint32_t sub_command_idID,
                                      uint32_t mgmt_engine_id,
                                      struct cm_role_info * role_info_p)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  int32_t return_value;
  uint32_t uiBuffLen = 0;

  if (!tnsprt_channel_p || !role_info_p || !dm_path_p)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return OF_FAILURE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, sub_command_idID, dm_path_p,
                         0, NULL, NULL, &uiBuffLen);

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  return_value = cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                        pSocketBuf, uiBuffLen);
  if (return_value != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);
  return OF_SUCCESS;
}

/**
  \ingroup DMWRPAPI
NOT USED/REQUIRED
   <b>Input Parameters: </b>\n
   <i>:</i>
   <b>Output Parameters: </b>\n
   <i>:</i>
   <b>Return Value: </b>\n
   <i></i>
 **/
 int32_t ucmdm_ReceiveResponse (void * tnsprt_channel_p,
                                          struct cm_result ** ppUCMResponseMsg)
{
  unsigned char *pSocketBuf = NULL;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_result *pResponseMsg = NULL;
  uint32_t uiMsgLen=0;

  if (!tnsprt_channel_p)
  {
    CM_DMTRANS_DEBUG_PRINT ("Invalid input");
    return OF_FAILURE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return OF_FAILURE;
  }

  if ((cm_tnsprt_recv_message (tnsprt_channel_p,
                                   pUCMGenericHeader, &pSocketBuf,
                                   &uiMsgLen)) == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT ("cm_tnsprt_recv_message failed");
    of_free (pUCMGenericHeader);
    return OF_FAILURE;
  }

  /*Memory Allocation for Response Msg structure */
  pResponseMsg = (struct cm_result *) of_calloc (1, sizeof (struct cm_result));
  if (pResponseMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT
      (" Memory Allocation failed for Responce Message Structure");
    of_free (pUCMGenericHeader);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  /*Fill Response Msg from Recieve Buffer */
  if ((UCMExtractDMResponseFromBuff (CM_COMMAND_DM_REQUEST,
                                     pSocketBuf, &uiMsgLen,
                                     pResponseMsg)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Fill Response Msg struct Failed");
    of_free (pUCMGenericHeader);
    of_free (pSocketBuf);
    if(pResponseMsg->result.error.error_code == CM_COMMON_NO_KEY_FOR_TABLE && pResponseMsg->sub_command_id == CM_CMD_GET_DM_KEY_CHILD_INFO)
    {
       *ppUCMResponseMsg = pResponseMsg;
    }
    else
    {
       of_free (pResponseMsg);
    }
    return OF_FAILURE;
  }

  of_free (pUCMGenericHeader);
  of_free (pSocketBuf);

  *ppUCMResponseMsg = pResponseMsg;
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API is used to extract the Result information from the received message buffer. This will take the Message buffer received from Transport channel as input and tries to extract the expected information from the buffer and fills the struct cm_result structure with apropriate values.
  <b>Input Parameters:</b>\n
  <i>pRespBuff:</i> Pointer to the Message buffer received from Transport channel.
  <b>Output Parameters:</b>\n
  <i>puiReqBufLen:</i> Request buffer length.
  <i>resp_msg_p:</i> Pointer to the struct cm_result structure used to hold the extracted information.
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On successful extraction of Result structure information.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t UCMExtractDMResponseFromBuff (uint32_t command_id,
                                      char * pRespBuff,
                                      uint32_t * puiReqBufLen,
                                      struct cm_result * cm_resp_msg_p)
{
  char *pUCMTmpRespBuf = pRespBuff;
  uint32_t uiNvCnt = 0;
  uint32_t uiLen = 0;
  int32_t ii;
  struct cm_nv_pair *nv_pairs_p;
  struct cm_dm_instance_map *pTempInst;
  struct cm_dm_array_of_role_permissions *role_perm_ary_p;

  if (pUCMTmpRespBuf == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input Buffer");
    return OF_FAILURE;
  }

  if (puiReqBufLen <= 0)
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input Buffer Length");
    return OF_FAILURE;
  }

  if (cm_resp_msg_p == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Invalid Output Response Message");
    return OF_FAILURE;
  }
  /*presuming Generic Header is already extracted. */

  /* Copy Sub Command */
  cm_resp_msg_p->sub_command_id = of_mbuf_get_32 (pUCMTmpRespBuf);
  pUCMTmpRespBuf += uiOSIntSize_g;
//  pUCMTmpRespBuf += uiInt32Size_g;
  /* Copy Result code */
  cm_resp_msg_p->result_code = of_mbuf_get_32 (pUCMTmpRespBuf);
  //pUCMTmpRespBuf += uiInt32Size_g;
  pUCMTmpRespBuf += uiOSIntSize_g;
  switch (cm_resp_msg_p->sub_command_id)
  {
    case CM_CMD_GET_DM_NODE_INFO:
    case CM_CMD_GET_DM_FIRST_NODE_INFO:
    case CM_CMD_GET_DM_KEY_CHILD_INFO:
      CM_DMTRANS_DEBUG_PRINT (" Copy Response for DM NodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_DMTRANS_DEBUG_PRINT (" JE returned Success");
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
  	  	  pUCMTmpRespBuf += uiOSIntSize_g;

          if (cm_resp_msg_p->data.dm_info.count_ui != 1)
          {
            CM_DMTRANS_DEBUG_PRINT (" Invalid Node Count received");
            return OF_FAILURE;
          }

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.count_ui,
                                       &cm_resp_msg_p->data.dm_info.node_info_p);

          /* Get Opaque Length */
          if (cm_resp_msg_p->sub_command_id == CM_CMD_GET_DM_FIRST_NODE_INFO)
          {
            cm_resp_msg_p->data.dm_info.opaque_info_length = of_mbuf_get_32 (pUCMTmpRespBuf);
            //pUCMTmpRespBuf += sizeof (uint32_t);
  	        pUCMTmpRespBuf += uiOSIntSize_g;
            if (cm_resp_msg_p->data.dm_info.count_ui == 0)
            {
              CM_DMTRANS_DEBUG_PRINT ("Invalid Opaque length received");
              /* Free Node Attribs */
              // of_free(cm_resp_msg_p->data.dm_info.node_info_p);
              return OF_FAILURE;
            }

            UCMCopyOpaqueInfoFromRespBuff (&pUCMTmpRespBuf,
                                           cm_resp_msg_p->data.dm_info.opaque_info_length,
                                           &cm_resp_msg_p->data.dm_info.
                                           opaque_info_p);
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_DMTRANS_DEBUG_PRINT ("DM returned Failure");
//          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          //        pUCMTmpRespBuf += uiInt32Size_g;
          uiLen = 0;
          if (ucmJECopyErrorInfoFromoBuffer
              (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
          {
            CM_DMTRANS_DEBUG_PRINT
              ("Copying Error Info into UCMResult Failed");
            return OF_FAILURE;
          }
          pUCMTmpRespBuf += uiLen;
          return OF_FAILURE;
        }
      }
      break;
    case CM_CMD_GET_DM_NODE_AND_CHILD_INFO:
    {
      CM_DMTRANS_DEBUG_PRINT (" Copy Response for DM NodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
         // pUCMTmpRespBuf += sizeof (uint32_t);
  	 // pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.count_ui,
                                       &cm_resp_msg_p->data.dm_info.node_info_p);

          cm_resp_msg_p->data.dm_info.node_info_array_p =
            (struct cm_array_of_structs *) of_calloc (1, sizeof (struct cm_array_of_structs));
          if (cm_resp_msg_p->data.dm_info.node_info_array_p == NULL)
          {
            CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
            /* TODO  Freeing of allocated buffers */
            return OF_FAILURE;
          }

          cm_resp_msg_p->data.dm_info.node_info_array_p->count_ui =
            of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
  	 // pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.node_info_array_p->
                                       count_ui,
                                       (struct cm_dm_node_info **) & cm_resp_msg_p->
                                       data.dm_info.node_info_array_p->struct_arr_p);
          break;
        case CM_JE_FAILURE:
        {
          CM_DMTRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
//          pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
          return OF_FAILURE;
        }
      }
      break;
    }
   case CM_CMD_GET_DM_KEY_CHILDS:
   {
      CM_DMTRANS_DEBUG_PRINT (" Copy Response for DM NodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
//  	  pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;

          if(cm_resp_msg_p->data.dm_info.count_ui > 0 )
					{
            CM_DMTRANS_DEBUG_PRINT ("Node count is greater than zero");
            return OF_FAILURE;
					}

          cm_resp_msg_p->data.dm_info.node_info_array_p =
            (struct cm_array_of_structs *) of_calloc (1, sizeof (struct cm_array_of_structs));
          if (cm_resp_msg_p->data.dm_info.node_info_array_p == NULL)
          {
            CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
            /* TODO  Freeing of allocated buffers */
            return OF_FAILURE;
          }

          cm_resp_msg_p->data.dm_info.node_info_array_p->count_ui =
            of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
//  	  pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.node_info_array_p->
                                       count_ui,
                                       (struct cm_dm_node_info **) & cm_resp_msg_p->
                                       data.dm_info.node_info_array_p->struct_arr_p);
          break;
        case CM_JE_FAILURE:
        {
          CM_DMTRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
          return OF_FAILURE;
        }
      }
      break;
    }
    case CM_CMD_GET_DM_KEYS_ARRAY:
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_DMTRANS_DEBUG_PRINT (" JE returned Success");
          uiNvCnt = of_mbuf_get_32 (pUCMTmpRespBuf);

          cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p =
            (struct cm_array_of_nv_pair *) of_calloc (1, sizeof (struct cm_array_of_nv_pair));
          if (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p != NULL)
          {
            cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->count_ui = uiNvCnt;
            //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;

            cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs =
              (struct cm_nv_pair *) of_calloc (uiNvCnt, sizeof (struct cm_nv_pair));
            if (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs == NULL)
            {
              CM_DMTRANS_DEBUG_PRINT ("Memory Allocation Failed");
              of_free (cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p);
              return OF_FAILURE;
            }
            nv_pairs_p = cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->nv_pairs;
            /* Copy  Name Value Pair */
            for (ii = 0, uiLen = 0; ii < uiNvCnt;
                 ii++, pUCMTmpRespBuf += uiLen, uiLen = 0)
            {
              if (UCMCopyNvPairFromBuffer
                  (pUCMTmpRespBuf, &nv_pairs_p[ii], &uiLen) == OF_FAILURE)
              {
                CM_DMTRANS_DEBUG_PRINT ("UCMCopyNvPairFromBuffer failed");
		if(cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p)
		{
                	CM_FREE_PTR_NVPAIR_ARRAY
                  	(cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p,
                   	cm_resp_msg_p->data.dm_info.dm_node_nv_pairs_p->count_ui);
		}
                return OF_FAILURE;
              }
            }
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_DMTRANS_DEBUG_PRINT ("DM returned Failure");
          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
         return OF_FAILURE;
        }
      }
      break;

    case CM_CMD_GET_DM_INSTANCE_MAPLIST_BY_NAMEPATH:
      CM_DMTRANS_DEBUG_PRINT (" Copy DM InstanceMap");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
  	  //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
          cm_resp_msg_p->data.dm_info.instance_map_list_p =
            (struct cm_dm_instance_map *) of_calloc (cm_resp_msg_p->data.dm_info.count_ui,
                                              sizeof (struct cm_dm_instance_map));
          if (cm_resp_msg_p->data.dm_info.instance_map_list_p == NULL)
          {
            CM_DMTRANS_DEBUG_PRINT
              (" Memory allocation failed for InstanceMap");
            return OF_FAILURE;
          }

          for (ii = 0; ii < cm_resp_msg_p->data.dm_info.count_ui; ii++)
          {
            pTempInst = &cm_resp_msg_p->data.dm_info.instance_map_list_p[ii];

            /* Get Instance ID */
            pTempInst->instance_id = of_mbuf_get_32 (pUCMTmpRespBuf);
            //pUCMTmpRespBuf = pUCMTmpRespBuf + uiInt32Size_g;
  	    pUCMTmpRespBuf = pUCMTmpRespBuf + uiOSIntSize_g;

            /* Get Instance Type */
            pTempInst->key_type = *pUCMTmpRespBuf;
            pUCMTmpRespBuf++;

            /* Get Instance Key Length */
            pTempInst->key_length = of_mbuf_get_32 (pUCMTmpRespBuf);
//          pUCMTmpRespBuf = pUCMTmpRespBuf + uiInt32Size_g;
          pUCMTmpRespBuf = pUCMTmpRespBuf + uiOSIntSize_g;

            /* Get Key value */
            pTempInst->key_p = (char *) of_calloc (1, pTempInst->key_length + 1);
            if (!pTempInst->key_p)
            {
              int32_t j;
              for (j = 0; j < ii; j++)
              {
                of_free (cm_resp_msg_p->data.dm_info.instance_map_list_p[j].key_p);
              }

              of_free (cm_resp_msg_p->data.dm_info.instance_map_list_p);
              return OF_FAILURE;
            }
            of_memcpy (pTempInst->key_p, pUCMTmpRespBuf, pTempInst->key_length);
            pUCMTmpRespBuf = pUCMTmpRespBuf + pTempInst->key_length;
          }
          break;
        case CM_JE_FAILURE:
        {
          CM_DMTRANS_DEBUG_PRINT ("DM returned Failure");
          uiLen = 0;
          if (ucmJECopyErrorInfoFromoBuffer
              (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
          {
            CM_DMTRANS_DEBUG_PRINT
              ("Copying Error Info into UCMResult Failed");
            return OF_FAILURE;
          }
          pUCMTmpRespBuf += uiLen;
         return OF_FAILURE;
        }
      }
      break;

    case CM_CMD_GET_DM_NEXT_NODE_INFO:
    case CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO:
      CM_DMTRANS_DEBUG_PRINT (" Copy Response for DM GetNextNodeInfo");
      switch (cm_resp_msg_p->result_code)
      {
        case CM_JE_SUCCESS:
          CM_DMTRANS_DEBUG_PRINT ("JE returned Success");
          cm_resp_msg_p->data.dm_info.count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
  	  //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
          if (cm_resp_msg_p->data.dm_info.count_ui != 1)
          {
            CM_DMTRANS_DEBUG_PRINT (" Invalid Node Count received");
            return OF_FAILURE;
          }

          UCMCopyNodeInfoFromRespBuff (&pUCMTmpRespBuf,
                                       cm_resp_msg_p->data.dm_info.count_ui,
                                       &cm_resp_msg_p->data.dm_info.node_info_p);

          /* Get Opaque Length */
          cm_resp_msg_p->data.dm_info.opaque_info_length = of_mbuf_get_32 (pUCMTmpRespBuf);
          //pUCMTmpRespBuf += sizeof (uint32_t);
  	  //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
          if (cm_resp_msg_p->data.dm_info.count_ui == 0)
          {
            CM_DMTRANS_DEBUG_PRINT (" Invalid Opaque length received");
            return OF_FAILURE;
          }

          UCMCopyOpaqueInfoFromRespBuff (&pUCMTmpRespBuf,
                                         cm_resp_msg_p->data.dm_info.opaque_info_length,
                                         &cm_resp_msg_p->data.dm_info.opaque_info_p);
          break;
        case CM_JE_FAILURE:
        {
          CM_DMTRANS_DEBUG_PRINT ("DM returned Failure");
          //          cm_resp_msg_p->result.error.error_code = of_mbuf_get_32 (pUCMTmpRespBuf);
          //        pUCMTmpRespBuf += uiInt32Size_g;
          uiLen = 0;
          if (ucmJECopyErrorInfoFromoBuffer
              (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
          {
            CM_DMTRANS_DEBUG_PRINT
              ("Copying Error Info into UCMResult Failed");
            return OF_FAILURE;
          }
          pUCMTmpRespBuf += uiLen;
          return OF_FAILURE;
        }
      }
      break;

    case CM_CMD_IS_DM_INSTANCE_USING_NAMEPATH_AND_KEY:
    case CM_CMD_IS_DM_CHILD_NODE:
      cm_resp_msg_p->result.success.reserved = of_mbuf_get_32 (pUCMTmpRespBuf);
      //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
      if (cm_resp_msg_p->result_code == CM_JE_FAILURE)
      {
        uiLen = 0;
        if (ucmJECopyErrorInfoFromoBuffer
            (pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
        {
          CM_DMTRANS_DEBUG_PRINT ("Copying Error Info into UCMResult Failed");
          return OF_FAILURE;
        }
        pUCMTmpRespBuf += uiLen;
        break;
      }
      break;
    case CM_CMD_GET_SAVE_FILE_NAME:
    case CM_CMD_GET_SAVE_DIRECTORY_NAME:
      cm_resp_msg_p->data.dm_info.opaque_info_length = of_mbuf_get_32 (pUCMTmpRespBuf);
//      pUCMTmpRespBuf += sizeof (uint32_t);
  	  //pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
      cm_resp_msg_p->data.dm_info.opaque_info_p = (char *) of_calloc (1,
                                                       cm_resp_msg_p->data.dm_info.opaque_info_length + 1);
      if (!cm_resp_msg_p->data.dm_info.opaque_info_p)
      {
        CM_DMTRANS_DEBUG_PRINT ("Memory Allocation Failed");
        return OF_FAILURE;
      }
      of_strncpy ((char *) cm_resp_msg_p->data.dm_info.opaque_info_p,
                 pUCMTmpRespBuf, cm_resp_msg_p->data.dm_info.opaque_info_length);
      pUCMTmpRespBuf += cm_resp_msg_p->data.dm_info.opaque_info_length;
      break;
    case CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
      if (cm_resp_msg_p->result_code == CM_JE_FAILURE)
      {
        uiLen = 0;
        if (ucmJECopyErrorInfoFromoBuffer(pUCMTmpRespBuf, cm_resp_msg_p, &uiLen) != OF_SUCCESS)
        {
          CM_DMTRANS_DEBUG_PRINT ("Copying Error Info into UCMResult Failed");
          return OF_FAILURE;
        }
        pUCMTmpRespBuf += uiLen;
        break;
      }

    case CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      break;
    case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE:
    case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE:
      if (cm_resp_msg_p->result_code == CM_JE_SUCCESS)
      {
        cm_resp_msg_p->data.dm_info.opaque_info_p =
          (struct cm_dm_role_permission *) of_calloc (1,
                                               sizeof
                                               (struct cm_dm_role_permission));
        if (!cm_resp_msg_p->data.dm_info.opaque_info_p)
        {
          CM_DMTRANS_DEBUG_PRINT ("Memory Allocation Failed");
          return OF_FAILURE;
        }
        of_memcpy ((struct cm_dm_role_permission *) cm_resp_msg_p->data.dm_info.
                  opaque_info_p, pUCMTmpRespBuf,
                  sizeof (struct cm_dm_role_permission));
        pUCMTmpRespBuf += sizeof (struct cm_dm_role_permission);
      }
      break;

    case CM_CMD_GET_DM_NODE_ROLE_ATTRIBS:
    case CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS:
      if (cm_resp_msg_p->result_code == CM_JE_SUCCESS)
      {
        role_perm_ary_p = (struct cm_dm_array_of_role_permissions *) of_calloc (1,
                                                                    sizeof
                                                                    (struct cm_dm_array_of_role_permissions));
        if (!role_perm_ary_p)
        {
          CM_DMTRANS_DEBUG_PRINT ("Memory Allocation Failed");
          return OF_FAILURE;
        }
        role_perm_ary_p->count_ui = of_mbuf_get_32 (pUCMTmpRespBuf);
//        pUCMTmpRespBuf += uiInt32Size_g;
  	    pUCMTmpRespBuf += uiOSIntSize_g;
        if (role_perm_ary_p->count_ui)
        {
					role_perm_ary_p->role_permissions=(struct cm_dm_role_permission *) of_calloc(role_perm_ary_p->count_ui,
								sizeof(struct cm_dm_role_permission));
					if(! role_perm_ary_p->role_permissions)
					{
						 CM_DMTRANS_DEBUG_PRINT ("Memory Allocation Failed");
						 return OF_FAILURE;
					}
          for(ii = 0; ii < role_perm_ary_p->count_ui; ii++)
          {
            UcmExtractRolePermFromBuff(&(role_perm_ary_p->role_permissions[ii]),pUCMTmpRespBuf);
            pUCMTmpRespBuf += UCMDM_ADMIN_ROLE_MAX_LEN + 1 + uiOSIntSize_g; 
          }
          cm_resp_msg_p->data.dm_info.opaque_info_p = (void *) role_perm_ary_p;
        }
      }
      break;
    case CM_CMD_GET_DM_CHILD_NODE_COUNT:
      // copy into   cm_resp_msg_p->data.data_modelGet.count_ui:
      break;
    case CM_CMD_GET_NEW_DM_INSTANCE_ID:
    case CM_CMD_ADD_DM_INSTANCE_MAP:
    case CM_CMD_DEL_DM_INSTANCE_MAP:
      break;
  }
  return OF_SUCCESS;
}

int32_t UcmExtractRolePermFromBuff( struct cm_dm_role_permission *role_perm_p, char* pBuff )
{
  of_memcpy(role_perm_p->role, pBuff, UCMDM_ADMIN_ROLE_MAX_LEN + 1);
  pBuff = pBuff + UCMDM_ADMIN_ROLE_MAX_LEN + 1;
  role_perm_p->permissions = of_mbuf_get_32 (pBuff);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API is used to the file name from the given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <b>Output Parameters:</b>\n
  <i>file_name_p:</i> Name of the file.
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On successful extraction of the file name.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_file_name (void * tnsprt_channel_p,
                           uint32_t mgmt_engine_id,
                           char * dm_path_p,
                           char * role_name_p, char ** file_name_p)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };

  of_strcpy (role_info.admin_role, role_name_p);
  return_value =
    ucmdm_SendRequest (tnsprt_channel_p, dm_path_p, CM_CMD_GET_SAVE_FILE_NAME,
                       mgmt_engine_id, &role_info);

  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  *file_name_p = pResponseMsg->data.dm_info.opaque_info_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API is used to the directory name from the given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <b>Output Parameters:</b>\n
  <i>file_name_p:</i> Name of the file.
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On successful extraction of the directory name.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_directory_name (void * tnsprt_channel_p,
                                uint32_t mgmt_engine_id,
                                char * dm_path_p,
                                char * role_name_p, char ** directory_name_p)
{
  int32_t return_value;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };

  of_strcpy (role_info.admin_role, role_name_p);
  return_value =
    ucmdm_SendRequest (tnsprt_channel_p, dm_path_p, CM_CMD_GET_SAVE_FILE_NAME,
                       mgmt_engine_id, &role_info);

  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_SendRequest failed");
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  *directory_name_p = pResponseMsg->data.dm_info.opaque_info_p;

  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

#ifdef CM_ROLES_PERM_SUPPORT
/**
 \ingroup DMWRPAPI
  This API used by security application to set Roles and Permissions of a particular Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_set_role_permissions_by_role (void * tnsprt_channel_p,
                                        uint32_t mgmt_engine_id,
                                        char * dm_path_p, char * role_name_p,
                                        struct cm_dm_role_permission * role_perm_p,
					struct cm_result **result_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);
  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    *result_p = pResponseMsg;
    return OF_FAILURE;
  }
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to delete Roles and Permissions of a particular Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_delete_role_permissions_by_role (void * tnsprt_channel_p,
                                           uint32_t mgmt_engine_id,
                                           char * dm_path_p,
                                           char * role_name_p,
                                           struct cm_dm_role_permission * role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);
  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to get Roles and Permissions of a particular Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_role_permissions_by_role (void * tnsprt_channel_p,
                                        uint32_t mgmt_engine_id,
                                        char * dm_path_p, char * role_name_p,
                                        struct cm_dm_role_permission * role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);
  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }

  of_memcpy (role_perm_p,
            (struct cm_dm_role_permission *) pResponseMsg->data.dm_info.opaque_info_p,
            sizeof (struct cm_dm_role_permission));
  of_free (pResponseMsg->data.dm_info.opaque_info_p);
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to get array of Roles and Permissions of a particular Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>role_perm_p:</i> Pointer to array of Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_role_permissions (void * tnsprt_channel_p,
                                  uint32_t mgmt_engine_id, char * dm_path_p,
                                  char * role_name_p,
                                  struct cm_dm_array_of_role_permissions **
                                  role_perm_ary_pp)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  char *pSocketBuf;

  if ((!tnsprt_channel_p) || (!role_name_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_GET_DM_NODE_ROLE_ATTRIBS, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }

  *role_perm_ary_pp =
    (struct cm_dm_array_of_role_permissions *) pResponseMsg->data.dm_info.opaque_info_p;
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/* * * * * * * * * I N S T A N C E S * * * * * * * * * * * * * * * * * * * */

/**
 \ingroup DMWRPAPI
  This API used by security application to set Roles and Permissions for a particular Instance of a Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>role_perm_p:</i> Pointer to array of Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_set_instance_role_permissions_by_role (void * tnsprt_channel_p,
                                                uint32_t mgmt_engine_id,
                                                char * dm_path_p,
                                                char * role_name_p,
                                                struct cm_dm_role_permission *
                                                role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_SET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE, dm_path_p,
                         0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to get Roles and Permissions for a particular Instance of a Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_instance_role_permissions_by_role (void * tnsprt_channel_p,
                                                uint32_t mgmt_engine_id,
                                                char * dm_path_p,
                                                char * role_name_p,
                                                struct cm_dm_role_permission *
                                                role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE, dm_path_p,
                         0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  of_memcpy (role_perm_p,
            (struct cm_dm_role_permission *) pResponseMsg->data.dm_info.opaque_info_p,
            sizeof (struct cm_dm_role_permission));
  of_free (pResponseMsg->data.dm_info.opaque_info_p);
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to delete Roles and Permissions for a particular Instance of a Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>role_perm_p:</i> Pointer to array of Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_delete_instance_role_permissions_by_role (void * tnsprt_channel_p,
                                                   uint32_t mgmt_engine_id,
                                                   char * dm_path_p,
                                                   char * role_name_p,
                                                   struct cm_dm_role_permission *
                                                   role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_DEL_DM_INSTANCE_ROLE_ATTRIBS_BY_ROLE, dm_path_p,
                         0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to get Roles and Permissions for a particular Instance of a Data Element Node for given DM Path.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <b>OutputParams</b>\n
  <i>role_perm_ary_pp:</i> Pointer to array of Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_instance_role_permissions (void * tnsprt_channel_p,
                                          uint32_t mgmt_engine_id,
                                          char * dm_path_p,
                                          char * role_name_p,
                                          struct cm_dm_array_of_role_permissions **
                                          role_perm_ary_pp)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  char *pSocketBuf;

  if ((!tnsprt_channel_p) || (!role_name_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_GET_DM_INSTANCE_ROLE_ATTRIBS, dm_path_p,
                         0, NULL, NULL, &uiBuffLen);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  *role_perm_ary_pp =
    (struct cm_dm_array_of_role_permissions *) pResponseMsg->data.dm_info.opaque_info_p;
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

#if UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY
/**
 \ingroup DMWRPAPI
  This API used by security application to set Roles and Permissions for a particular Instance of a Data Element Node for given DM Path based on the given key.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>key_p:</i> The key
  <i>key_type:</i> The key type
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_set_instance_role_permissions (void * tnsprt_channel_p,
                                          uint32_t mgmt_engine_id,
                                          char * dm_path_p,
                                          char * role_name_p, void * key_p,
                                          uint32_t key_type,
                                          struct cm_dm_role_permission * role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;
  uint32_t key_length;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_SET_DM_NODE_ROLE_ATTRIBS_BY_ROLE, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);
  uiBuffLen += uiOSIntSize_g; /* Key Type */
  uiBuffLen += uiOSIntSize_g; /* Key Value Len */
  uiBuffLen += of_strlen (key_p);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));
  pTmp += (uiReqMsgLen + sizeof (struct cm_dm_role_permission));
  pTmp = of_mbuf_put_32 (pTmp, key_type);
  key_length = of_strlen (key_p);
  pTmp = of_mbuf_put_32 (pTmp, key_length);
  of_memcpy (pTmp, key_p, key_length);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to get Roles and Permissions for a particular Instance of a Data Element Node for given DM Path based on the given key.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>key_p:</i> The key
  <i>key_type:</i> The key type
  <b>OutputParams</b>\n
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_get_instance_role_permissions (void * tnsprt_channel_p,
                                          uint32_t mgmt_engine_id,
                                          char * dm_path_p,
                                          char * role_name_p, void * key_p,
                                          uint32_t key_type,
                                          struct cm_dm_role_permission * role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;
  uint32_t key_length;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_GET_DM_NODE_ROLE_ATTRIBS_BY_ROLE, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);
  uiBuffLen += uiOSIntSize_g; /* Key Type */
  uiBuffLen += uiOSIntSize_g; /* Key Value Len */
  uiBuffLen += of_strlen (key_p);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));
  pTmp += (uiReqMsgLen + sizeof (struct cm_dm_role_permission));
  pTmp = of_mbuf_put_32 (pTmp, key_type);
  key_length = of_strlen (key_p);
  pTmp = of_mbuf_put_32 (pTmp, key_length);
  of_memcpy (pTmp, key_p, key_length);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  of_memcpy (role_perm_p,
            (struct cm_dm_role_permission *) pResponseMsg->data.dm_info.opaque_info_p,
            sizeof (struct cm_dm_role_permission));
  of_free (pResponseMsg->data.dm_info.opaque_info_p);
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}

/**
 \ingroup DMWRPAPI
  This API used by security application to delete Roles and Permissions for a particular Instance of a Data Element Node for given DM Path based on the given key.
  <i>tnsprt_channel_p:</i> Pointer to the Transport channel node.
  <i>mgmt_engine_id:</i> Management Engine ID
  <i>dm_path_p:</i> Pointer to Data Model Path
  <i>role_name_p:</i> Role of Administrator
  <i>key_p:</i> The key
  <i>key_type:</i> The key type
  <i>role_perm_p:</i> Pointer to Roles and Permission structure
  <b>Return Value: </b>\n
  <i>OF_SUCCESS:</i> On success.
  <i>OF_FAILURE:</i> On any failure.
 **/
int32_t cm_dm_delete_instance_role_permissions (void * tnsprt_channel_p,
                                             uint32_t mgmt_engine_id,
                                             char * dm_path_p,
                                             char * role_name_p, void * key_p,
                                             uint32_t key_type,
                                             struct cm_dm_role_permission * role_perm_p)
{
  int32_t return_value;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;
  struct cm_role_info role_info = { };
  struct cm_msg_generic_header UCMGenericHeader = { };
  uint32_t uiBuffLen = 0;
  uint32_t uiReqMsgLen;
  char *pSocketBuf, *pTmp;
  uint32_t key_length;

  if ((!tnsprt_channel_p) || (!role_perm_p))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  of_strcpy (role_info.admin_role, role_name_p);
  /* Memory Allocation for Generic Header */

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        &role_info, &UCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg,
                         CM_CMD_DEL_DM_NODE_ROLE_ATTRIBS_BY_ROLE, dm_path_p, 0,
                         NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  uiBuffLen += sizeof (struct cm_dm_role_permission);
  uiBuffLen += uiOSIntSize_g; /* Key Type */
  uiBuffLen += uiOSIntSize_g; /* Key Value Len */
  uiBuffLen += of_strlen (key_p);

  /* size of RolePerm structure  */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);
  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append Role and Permissions  to SocketBuffer */
  pTmp = pSocketBuf;
  of_memcpy (pTmp + uiReqMsgLen, role_perm_p, sizeof (struct cm_dm_role_permission));
  pTmp += (uiReqMsgLen + sizeof (struct cm_dm_role_permission));
  pTmp = of_mbuf_put_32 (pTmp, key_type);
  key_length = of_strlen (key_p);
  pTmp = of_mbuf_put_32 (pTmp, key_length);
  of_memcpy (pTmp, key_p, key_length);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, &UCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT (" ucmdm_ReceiveResponse failed");
    return OF_FAILURE;
  }

  if (pResponseMsg->result_code == CM_JE_FAILURE)
  {
    UCMFreeUCMResult (pResponseMsg);
    return OF_FAILURE;
  }
  UCMFreeUCMResult (pResponseMsg);
  return OF_SUCCESS;
}
#endif /*UCMDM_INSTANCE_ROLE_AND_PERMISSIONS_USING_KEY */
#endif /*CM_ROLES_PERM_SUPPORT */

int32_t cm_dm_get_next_table_n_anchor_node_info (char * head_node_path_p,
                               uint32_t traversal_type_ui,
                               void * tnsprt_channel_p,
                               uint32_t mgmt_engine_id,
                               struct cm_role_info * role_info_p,
                               struct cm_dm_node_info ** node_info_pp,
                               void ** opaque_info_pp, uint32_t * opaque_len_p)
{
  unsigned char *pSocketBuf = NULL, *pTmp;
  int32_t return_value;
  uint32_t uiReqMsgLen, count_ui, *pTempIDAry, uiBuffLen = 0;
  struct cm_msg_generic_header *pUCMGenericHeader = NULL;
  struct cm_request_msg *pUCMRequestMsg = NULL;
  struct cm_result *pResponseMsg = NULL;

  if (!tnsprt_channel_p || !role_info_p || !(*opaque_info_pp) || (*opaque_len_p == 0))
  {
    CM_DMTRANS_DEBUG_PRINT (" Invalid input");
    return OF_FAILURE;
  }

  /* Memory Allocation for Generic Header */
  pUCMGenericHeader = (struct cm_msg_generic_header *) of_calloc (1,
                                                          sizeof
                                                          (struct cm_msg_generic_header));
  if (pUCMGenericHeader == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Generic Header failed");
    return OF_FAILURE;
  }

  /* Setting Generic Header Values */
  cm_fill_generic_header (mgmt_engine_id, CM_COMMAND_DM_REQUEST,
                        role_info_p, pUCMGenericHeader);

  /* Memory Allocation for Request Message */
  pUCMRequestMsg = (struct cm_request_msg *) of_calloc (1, sizeof (struct cm_request_msg));
  if (pUCMRequestMsg == NULL)
  {
    CM_DMTRANS_DEBUG_PRINT ("Memory Allocation for Request Message failed");
    of_free (pUCMGenericHeader);
    return OF_FAILURE;
  }

  /*Fill Request Message Structure */
  cm_fill_request_message (pUCMRequestMsg, CM_CMD_GET_DM_NEXT_TABLE_ANCHOR_NODE_INFO,
                         head_node_path_p, 0, NULL, NULL, &uiBuffLen);

  uiReqMsgLen = uiBuffLen;
  count_ui = *opaque_len_p;
  uiBuffLen += uiOSIntSize_g +        /* for Traversal type */
    uiOSIntSize_g +           /* for OpaqueInfo Len */
    ((count_ui) * (uiOSIntSize_g));    /*For OpaqueInfo */

  /* Allocate Memory for Send Buffer */
  pSocketBuf = (unsigned char *) of_calloc (1, uiBuffLen);

  if (!pSocketBuf)
  {
    CM_DMTRANS_DEBUG_PRINT (" Memory allocation failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    return OF_FAILURE;
  }

  /* Convert Request Message Structure into flat Buffer */
  cm_frame_tnsprt_request (pUCMRequestMsg, pSocketBuf);

  /* Append traversal type to SocketBuffer */
  pTmp = pSocketBuf;
  pTmp = of_mbuf_put_32 (pTmp + uiReqMsgLen, traversal_type_ui);

  /* Copy OpaqueInfo Len  */
  pTmp = of_mbuf_put_32 (pTmp, *opaque_len_p);

  /* Copy OpaqueInfo  */
  of_memcpy (pTmp, *opaque_info_pp, ((*opaque_len_p) * uiOSIntSize_g));
  pTmp += ((*opaque_len_p) * uiOSIntSize_g);

  /*Send Message through Transport Channel */
  if ((cm_tnsprt_send_message (tnsprt_channel_p, pUCMGenericHeader,
                                   pSocketBuf, uiBuffLen)) != OF_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Sending through transport channel failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  return_value = ucmdm_ReceiveResponse (tnsprt_channel_p, &pResponseMsg);
  if (return_value == OF_FAILURE)
  {
    CM_DMTRANS_DEBUG_PRINT ("ucmdm_ReceiveResponse failed");
    of_free (pUCMGenericHeader);
    of_free (pUCMRequestMsg);
    of_free (pSocketBuf);
    return OF_FAILURE;
  }

  //clean opaque info
  if ((*opaque_info_pp) != NULL)
  {
    of_free(*opaque_info_pp);
  }

  if (pResponseMsg->result_code == CM_JE_SUCCESS)
  {
    CM_DMTRANS_DEBUG_PRINT ("Next Node Success");
    return_value = OF_SUCCESS;
  }
  else
  {
    CM_DMTRANS_DEBUG_PRINT ("Next Node failure");
    return_value = OF_FAILURE;
  }

  *node_info_pp = pResponseMsg->data.dm_info.node_info_p;
  *opaque_len_p = pResponseMsg->data.dm_info.opaque_info_length;
  *opaque_info_pp = pResponseMsg->data.dm_info.opaque_info_p;

  pTempIDAry = (uint32_t *) (*opaque_info_pp);
  UCMFreeUCMResult (pResponseMsg);
  of_free (pUCMGenericHeader);
  of_free (pUCMRequestMsg);
  of_free (pSocketBuf);
  return return_value;
}


#endif /* CM_DM_SUPPORT */
