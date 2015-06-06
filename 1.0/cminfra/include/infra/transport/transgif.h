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
 * File name: transgif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains API declarations provided by
 *              UCM transport channel layer.
 * 
 */

#ifndef CM_TRANS_GIF_H
#define CM_TRANS_GIF_H

void *cm_tnsprt_create_channel (uint8_t trans_protocol_ui, uint32_t addr_ui,
                                   uint16_t src_port_ui, uint16_t dest_port_ui);

void cm_tnsprt_close_channel (struct cm_tnsprt_channel * tnsprt_channel_p);
int32_t cm_tnsprt_send_message (void * tnsprt_channel_p,
                                    struct cm_msg_generic_header * gen_hdr_p,
                                    char * input_buff_p,
                                    uint32_t input_buff_len_ui);

int32_t cm_tnsprt_recv_message (void * tnsprt_channel_p,
                                    struct cm_msg_generic_header * gen_hdr_p,
                                    unsigned char ** p_msg_p, uint32_t * msg_len_ui_p);

int32_t cm_tnsprt_set_channel_attribs (void * tnsprt_channel_p,
                                       struct cm_tnsprt_channel_attribs * attrib_p);

void *cm_tnsprt_alloc_message (uint32_t size_ui);

void cm_tnsprt_free_message (void * msg_p);

#endif /* CM_TRANS_GIF_H */
