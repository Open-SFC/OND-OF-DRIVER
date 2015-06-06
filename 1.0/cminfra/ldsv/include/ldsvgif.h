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
 * File name:   ldsvgif.h
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: This file contains API declarations provided by LDSV.
*/

#ifndef CM_LDSV_GIF_H
#define CM_LDSV_GIF_H

/* API to Save the configuration */
//int32_t cm_ldsv_save_config(char* dm_path_p);

int32_t cm_ldsv_save_config (void * pTransportChannel,
                            struct cm_command * pCommand,
                            struct cm_result ** ppResult,
                            uint32_t uiMgmtEngineID, struct cm_role_info * pRoleInfo);

int32_t cm_ldsv_load_config (void * pTransportChannel,
                            struct cm_command * pCommand,
                            struct cm_result ** ppResult,
                            uint32_t uiMgmtEngineID, struct cm_role_info * pRoleInfo);

int32_t cm_ldsv_factory_reset (void * pTransportChannel,
                              struct cm_command * pCommand,
                              struct cm_result ** ppResult,
                              uint32_t uiMgmtEngineID,
                              struct cm_role_info * pRoleInfo);

int32_t cm_ldsv_flush (void * pTransportChannel,
                              struct cm_command * pCommand,
                              struct cm_result ** ppResult,
                              uint32_t uiMgmtEngineID,
                              struct cm_role_info * pRoleInfo);

#endif /* CM_LDSV_GIF_H */
