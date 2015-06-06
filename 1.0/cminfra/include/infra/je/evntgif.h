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
 * File name: evntgif.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/25/2013
 * Description: This file contains API declarations provided by
 *              UCM Notification Manager.
 * 
 */

#ifndef CM_NOTIFIMGR_GIF_H
#define CM_NOTIFIMGR_GIF_H

int32_t cm_evmgr_register_all_events(void);

int32_t cm_evmgr_register_interested_events (struct cm_evmgr_reg* cm_event_reg_p);

int32_t cm_evmgr_deregister_interested_events (struct cm_evmgr_reg* cm_event_reg_p);

int32_t cm_evmgr_generate_event(struct cm_evmgr_event* event_p);

int32_t cm_evmgr_dispatch_event ( uint32_t mgmt_engine_id,
                                void* tnsprt_channel_p, 
                                char* buf_p,
                                uint32_t buf_len_ui);

void cm_evmgr_init(void);

#endif /* CM_NOTIFIMGR_GIF_H */
