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
 * File name:  clitrchl.c
 * Author: Freescale Semiconductor    
 * Date:   01/20/2013
 * Description: This file contains transport channel related APIs like
 *              create transport channel, free transport channel pointer etc.
*/

#ifdef CM_CLIENG_SUPPORT
#include "cliincld.h"

/******************************************************************************
 * Function Name : cm_cli_establish_transport_channel
 * Description   : This API is used to establish a transport channel with the UCM
 *                 JE.
 * Input params  : NONE
 * Output params : NONE
 * Return value  : Pointer to Transport channel on successful creation of Transport
 *                 channel.
 *                 NULL in any failure.
 *****************************************************************************/
extern struct cm_cli_session *cli_session_p;
void *cm_cli_establish_transport_channel (void)
{
	 struct cm_tnsprt_channel *tnsprt_channel_p;
   int32_t return_value;
   uint32_t addr_ui;


	 /*if((return_value = cm_cli_get_server_ip(&addr_ui))!= OF_SUCCESS)
       return NULL;

	 tnsprt_channel_p =
			(struct cm_tnsprt_channel *) cm_tnsprt_create_channel (CM_IPPROTO_TCP, addr_ui, 0,
						UCMJE_CONFIGREQUEST_PORT);*/

	 tnsprt_channel_p =
			(struct cm_tnsprt_channel *) cm_tnsprt_create_channel (CM_IPPROTO_TCP, 0, 0,
						UCMJE_CONFIGREQUEST_PORT);
	 if (!tnsprt_channel_p)
	 {
			UCMCLIDEBUG ("%s :: cm_tnsprt_create_channel failed\n\r", __FUNCTION__);
			return NULL;
	 }
	 return ((void *) tnsprt_channel_p);
}

/******************************************************************************
 * Function Name : cm_cli_set_transport_channel_attribs
 * Description   : This API is used to Set the transport channel attributes. The
 *                 Attributes can be Inactivity timeout, etc.
 * Input params  : tnsprt_channel_p - pointer to transport channel
 *                 uiAttribId - Attribute ID
 *                 attrib_data_p - Attribute data
 * Output params : NONE 
 * Return value  : OF_SUCCESS on success
 *                 OF_FAILURE on any failure
 *****************************************************************************/
int32_t cm_cli_set_transport_channel_attribs (void * tnsprt_channel_p,
                                          uint16_t attrib_id_ui,
                                          void * attrib_data_p,
                                          struct cm_role_info * role_info_p)
{
  struct cm_tnsprt_channel_attribs trans_attribs;

  trans_attribs.attrib_id_ui = attrib_id_ui;
  trans_attribs.attrib_data.inact_time_out = *(uint16_t *) attrib_data_p;

  return (cm_tnsprt_set_channel_attribs (tnsprt_channel_p,
                                         &trans_attribs, role_info_p));
}

/******************************************************************************
 * Function Name : ucmCliTarminateTransportChannel
 * Description   : This API is used to close a Transport channel.
 * Input params  : tnsprt_channel_p - pointer to transport channel node.
 * Output params : NONE
 * Return value  : NONE
 *****************************************************************************/
void cm_cli_terminate_transport_channel (struct cm_tnsprt_channel * tnsprt_channel_p)
{
  if (tnsprt_channel_p)
  {
    cm_tnsprt_close_channel (tnsprt_channel_p);
  }
}

#endif /* CM_CLIENG_SUPPORT */
